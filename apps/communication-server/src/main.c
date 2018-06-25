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

#define VCHAN_BUF_LEN       4096

#define VCHAN_MAX_BAD_PACKETS 10

#define RETURN_NO_PCKT() return_args(0, 0)
#define RETURN_NO_MEM()  return_args(0, 0xEE)
#define RETURN_BAD_CHK() return_args(0, 0xFF)

static int bad_packets = 0;

struct vm_packet {
    void *data;
    uint8_t checksum;
    uint32_t len;
    struct vm_packet *next;
};
typedef struct vm_packet vm_packet_t;

vm_packet_t *vm_packet_head = NULL;

/* Create new node at the end of the linked list */
vm_packet_t* create_vm_packet(void)
{
    vm_packet_t* current = vm_packet_head;

    vm_packet_t* new_node = (vm_packet_t*)malloc(sizeof(vm_packet_t));
    if (new_node == NULL)
    {
        return NULL;
    }

    /* We could reuse memory, so we need to clear the node */
    memset(new_node, 0, sizeof(vm_packet_t));

    /* If there is no data in the list return the allocated head */
    if (current == NULL) {
        vm_packet_head = new_node;
        return vm_packet_head;
    }

    /* Else go to the end of the list and append new data */
    while (current->next != NULL) {
        current = current->next;
    }

    new_node->next = NULL;
    current->next = new_node;

    return new_node;
}

/* Remove head of Linked List and free used data
 */
void remove_vm_packet(void)
{
    vm_packet_t *temp = vm_packet_head;

    if(temp != NULL) {
        vm_packet_head = vm_packet_head->next;
        temp->next = NULL;
        if (temp->data) {
            free(temp->data);
        }
        free(temp);
    }
}

void return_args(unsigned int arg1, unsigned int arg2)
{
    seL4_SetMR(VCHAN_LEN_RET, arg1);
    seL4_SetMR(VCHAN_CHECKSUM_RET, arg2);
}

int main(int argc, char **argv)
{
    seL4_Word badge;
    seL4_MessageInfo_t tag = seL4_MessageInfo_new(0, 0, 0, VCHAN_NUM_RET);

    seL4_CPtr ep = (seL4_CPtr) atol(argv[COMM_SERVER_EP]);
    void * read_buffer = (void *)atol(argv[COMM_SERVER_READ_VADDR]);
    void * write_buffer = (void *)atol(argv[COMM_SERVER_WRITE_VADDR]);

    int i;
    unsigned char chk;

    while(1) {
        seL4_Recv(ep, &badge);

        int port = seL4_GetMR(VCHAN_PORT);
        int event = seL4_GetMR(VCHAN_EVENT);
        int checksum = seL4_GetMR(VCHAN_CHECKSUM);
        int len = seL4_GetMR(VCHAN_LEN);

        if (event == VCHAN_READ) {
            chk = 0;

            /* The endpoint has been badged by seL4. The Port gets passed in
             * from the VM. Therefore, if the port does not match, when the
             * virtual channels have already passed inspection, then something
             * is wrong with the VM and the comm server should be shut down.
             */
            assert(badge == port);

            if (len > VCHAN_BUF_LEN) {
                len = VCHAN_BUF_LEN;
            }

            vm_packet_t *packet = create_vm_packet();
            if (packet == NULL) {
                printf("WARNING: Could not create new packet entry\n");
                RETURN_NO_MEM();
                goto reply;
            }

            packet->data = malloc(len);
            if (packet->data == NULL) {
                printf("WARNING: Could not create new packet data\n");
                RETURN_NO_MEM();
                goto reply;
            }

            memcpy(packet->data, read_buffer, len);

            for (i = 0; i < len; i++) {
                chk = (chk + *(unsigned char *)(packet->data+i)) % 256;
            }

            if (chk != checksum) {
                printf("WARNING: Bad Checksum (%x), disregarding packet #%d\n", chk, ++bad_packets);
                assert(bad_packets < VCHAN_MAX_BAD_PACKETS);
                memset(packet->data, 0, len);

                RETURN_BAD_CHK();
            }
            else {
                packet->len = len;
                packet->checksum = chk;

                return_args(len, chk);
            }
        }
        else if (event == VCHAN_WRITE) {
            assert(badge == port);

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
            assert("We shouldn't have gotten here\n");
        }

    reply:
        seL4_Reply(tag);
    }

    return 0;
}
