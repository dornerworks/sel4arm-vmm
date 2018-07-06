/*
 * Copyright 2018, DornerWorks
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DORNERWORKS_BSD)
 */

#include <stdio.h>
#include <assert.h>

#include <sel4/sel4.h>
#include <sel4utils/process.h>

#include <sel4arm-vmm/vchan.h>

#define COMM_SERVER_EP          0
#define COMM_SERVER_READ_VADDR  1
#define COMM_SERVER_WRITE_VADDR 2
#define COMM_SERVER_TCB         3
#define COMM_SERVER_IPC_CAP     4
#define COMM_SERVER_IPC_ADDR    5

#define EP_THREAD_PRIO      99

#define VCHAN_BUF_LEN       4096

#define RETURN_NO_PCKT()  return_args(0, 0)
#define RETURN_NO_MEM()   return_args(0, 0xEE)
#define RETURN_BAD_CHK()  return_args(0, 0xFF)
#define RETURN_SHUTDOWN() return_args(VCHAN_LEN_SHUTDOWN, VCHAN_CHECKSUM_SHUTDOWN)

static seL4_MessageInfo_t tag;

static int bad_packets = 0;

struct vm_packet {
    void *data;
    uint8_t checksum;
    uint32_t len;
    struct vm_packet *next;
};
typedef struct vm_packet vm_packet_t;

vm_packet_t *vm_packet_head = NULL;

/* Allocate new node */
vm_packet_t* alloc_vm_packet(int len)
{
    vm_packet_t* new_node = (vm_packet_t*)malloc(sizeof(vm_packet_t));
    if (new_node == NULL)
    {
        return NULL;
    }

    /* We could reuse memory, so we need to clear the node */
    memset(new_node, 0, sizeof(vm_packet_t));

    new_node->data = malloc(len);
    if (new_node->data == NULL)
    {
        free(new_node);
        return NULL;
    }

    return new_node;
}

/* Free packet and allocated data. Warning: Use on packets in the linked
 * list will lose reference to all following packets unless appropriate
 * previsions are made. */
void free_vm_packet(vm_packet_t* packet)
{
    if(packet != NULL) {
        packet->next = NULL;
        if (packet->data) {
            free(packet->data);
        }
        free(packet);
    }
}

/* Link new node at the end of the linked list */
void link_vm_packet(vm_packet_t* new_node)
{
    if (new_node == NULL || new_node->data == NULL) {
        free_vm_packet(new_node);
        return;
    }

    vm_packet_t* current = vm_packet_head;

    new_node->next = NULL;

    /* If there is no data in the list return the allocated head */
    if (current == NULL) {
        vm_packet_head = new_node;
    }
    else
    {
        /* Else go to the end of the list and append new data */
        while (current->next != NULL) {
            current = current->next;
        }

        current->next = new_node;
    }
}

/* Remove head of Linked List and free used data
 */
void remove_vm_packet(void)
{
    vm_packet_t *temp = vm_packet_head;

    if(temp != NULL) {
        vm_packet_head = vm_packet_head->next;
    }

    free_vm_packet(temp);
}

void return_args(unsigned int arg1, unsigned int arg2)
{
    seL4_SetMR(VCHAN_LEN_RET, arg1);
    seL4_SetMR(VCHAN_CHECKSUM_RET, arg2);
}

void shutdown_comm_server(void)
{
    RETURN_SHUTDOWN();
    seL4_Reply(tag);
    seL4_TCB_Suspend(SEL4UTILS_TCB_SLOT);
    while(1);
}

/* stack for the new thread */
#define FAULT_THREAD_STACK_SIZE 256
static uint64_t fault_thread_stack[FAULT_THREAD_STACK_SIZE] __attribute__((__aligned__(sizeof(seL4_Word) * 2)));

/* function to run in the new thread */
static void fault_thread(void)
{
    seL4_MessageInfo_t fault_tag;
    seL4_UserContext regs;

    while(1) {
        fault_tag = seL4_Recv(SEL4UTILS_ENDPOINT_SLOT, NULL);

        /* If we get here, the communication-server has faulted. The VMM is blocked on a Call, so we still
         * need to reply, else the VMM will lock up. The shutdown_comm_server function just sets the
         * return args to dead & beef, replies, and suspends the communication-server.
         */
        seL4_TCB_ReadRegisters(SEL4UTILS_TCB_SLOT, false, 0, sizeof(regs) / sizeof(regs.pc), &regs);
        regs.pc = (seL4_Word) shutdown_comm_server;
        seL4_TCB_WriteRegisters(SEL4UTILS_TCB_SLOT, false, 0, sizeof(regs) / sizeof(regs.pc), &regs);

        fault_tag = seL4_MessageInfo_new(0, 0, 0, 0);
        seL4_Reply(fault_tag);
    }
}

int main(int argc, char **argv)
{
    seL4_Word badge;

    seL4_CPtr ep = (seL4_CPtr) atol(argv[COMM_SERVER_EP]);
    void * read_buffer = (void *)atol(argv[COMM_SERVER_READ_VADDR]);
    void * write_buffer = (void *)atol(argv[COMM_SERVER_WRITE_VADDR]);
    seL4_CPtr tcb = (seL4_CPtr) atol(argv[COMM_SERVER_TCB]);
    seL4_CPtr ipc_cap = (seL4_CPtr) atol(argv[COMM_SERVER_IPC_CAP]);
    seL4_Word ipc_buffer = (seL4_CPtr) atol(argv[COMM_SERVER_IPC_ADDR]);

    int i;
    unsigned char chk;

    /* We need to setup a thread to listen on our fault endpoint */
    int error;

    error = seL4_TCB_Configure(tcb, seL4_CapNull, SEL4UTILS_CNODE_SLOT, seL4_NilData,
                               SEL4UTILS_PD_SLOT, seL4_NilData, ipc_buffer, ipc_cap);
    assert(!error);

    uintptr_t fault_thread_stack_top = (uintptr_t)fault_thread_stack + sizeof(fault_thread_stack);

    seL4_UserContext regs = {
        .pc = (seL4_Word)fault_thread,
        .sp = (seL4_Word)fault_thread_stack_top
    };

    error = seL4_TCB_WriteRegisters(tcb, 0, 0, 2, &regs);
    assert(!error);

    seL4_TCB_SetSchedParams(tcb, SEL4UTILS_TCB_SLOT, EP_THREAD_PRIO, EP_THREAD_PRIO);

    error = seL4_TCB_Resume(tcb);
    assert(!error);

    tag = seL4_MessageInfo_new(0, 0, 0, VCHAN_NUM_RET);

    while(1) {
        seL4_Recv(ep, &badge);

        int port = seL4_GetMR(VCHAN_PORT);
        int event = seL4_GetMR(VCHAN_EVENT);
        int checksum = seL4_GetMR(VCHAN_CHECKSUM);
        int len = seL4_GetMR(VCHAN_LEN);

        /* The endpoint has been badged by seL4. The Port gets passed in
         * from the VM. Therefore, if the port does not match, when the
         * virtual channels have already passed inspection, then something
         * is wrong with the VM and the comm server should be shut down.
         */
        assert(badge == port);

        if (event == VCHAN_READ) {
            chk = 0;

            if (len > VCHAN_BUF_LEN) {
                len = VCHAN_BUF_LEN;
            }

            vm_packet_t *packet = alloc_vm_packet(len);

            if ((packet == NULL) || (packet->data == NULL)) {
                printf("WARNING: Could not create new packet\n");
                RETURN_NO_MEM();
                free_vm_packet(packet);
                goto reply;
            }

            memcpy(packet->data, read_buffer, len);

            for (i = 0; i < len; i++) {
                chk = (chk + *(unsigned char *)(packet->data+i)) % 256;
            }

            if (chk != checksum) {
                printf("WARNING: Bad Checksum (%x), expected (%x), disregarding packet #%d\n",
                       chk, checksum, bad_packets);
                bad_packets++;
                memset(packet->data, 0, len);

                RETURN_BAD_CHK();
                free_vm_packet(packet);
            }
            else {
                packet->len = len;
                packet->checksum = chk;

                link_vm_packet(packet);

                return_args(len, chk);
            }
        }
        else if (event == VCHAN_WRITE) {

            if(vm_packet_head != NULL) {
                memcpy(write_buffer, (void *)vm_packet_head->data, vm_packet_head->len);
                return_args(vm_packet_head->len, vm_packet_head->checksum);

                remove_vm_packet();
            }
            else {
                RETURN_NO_PCKT();
            }
        }
        else {
            printf("ERROR: Communication Server signalled without READ or WRITE event. Shutting Down\n");
            RETURN_SHUTDOWN();
            shutdown_comm_server();
        }

    reply:
        seL4_Reply(tag);
    }

    return 0;
}
