/*
 * Copyright 2014, NICTA
 * Copyright 2018, DornerWorks
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_DORNERWORKS_BSD)
 */

#include <autoconf.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <allocman/bootstrap.h>
#include <allocman/vka.h>
#include <vka/capops.h>
#include <vka/object.h>

#include <vspace/vspace.h>
#include <simple/simple.h>
#include <simple-default/simple-default.h>
#include <platsupport/io.h>
#include <sel4platsupport/platsupport.h>
#include <sel4platsupport/io.h>

#include <cpio/cpio.h>

#include <sel4arm-vmm/vm.h>
#include <sel4utils/irq_server.h>
#include <dma/dma.h>

#include <vmlinux.h>

#include <sel4arm-vmm/plat/devices.h>
#include <sel4arm-vmm/devices/vgic.h>
#include <sel4arm-vmm/devices/vram.h>
#include <sel4arm-vmm/devices/vusb.h>
#include <sel4arm-vmm/guest_vspace.h>

extern vmconf_t vm_confs[NUM_VMS];
extern struct vchan_device *vchan_config[NUM_VCHANS];

#define IRQSERVER_PRIO      (VM_PRIO + 1)
#define IRQ_MESSAGE_LABEL   0xCAFE

/* allocator static pool */
/* Each VM takes 256MB in individual 4kB pages, so the mem pool needs to increase */
#define ALLOCATOR_STATIC_POOL_SIZE ((1 << seL4_PageBits) * 6294 * NUM_VMS)
static char allocator_mem_pool[ALLOCATOR_STATIC_POOL_SIZE];

#define seL4_GetBootInfo() platsupport_get_bootinfo()

vka_t _vka;
simple_t _simple;
vspace_t _vspace;
sel4utils_alloc_data_t _alloc_data;
allocman_t *_allocator;
seL4_CPtr _fault_endpoint;
irq_server_t _irq_server;

struct ps_io_ops _io_ops;

extern char _cpio_archive[];

static void
print_cpio_info(void)
{
    struct cpio_info info;
    const char* name;
    unsigned long size;
    int i;

    cpio_info(_cpio_archive, &info);

    printf("\nCPIO: %d files found.\n", info.file_count);
    assert(info.file_count > 0);
    for (i = 0; i < info.file_count; i++) {
        void * addr;
        char buf[info.max_path_sz + 1];
        buf[info.max_path_sz] = '\0';
        addr = cpio_get_entry(_cpio_archive, i, &name, &size);
        assert(addr);
        strncpy(buf, name, info.max_path_sz);
        printf("%d) %-20s  %p, %8ld bytes\n", i, buf, addr, size);
    }
    printf("\n");
}

static void
print_boot_info(void)
{
    seL4_BootInfo* bi;
    int n_ut;
    int i;

    bi = platsupport_get_bootinfo();
    assert(bi);
    n_ut = bi->untyped.end - bi->untyped.start;
    printf("\n");
    printf("-------------------------------\n");
    printf("|  Boot info untyped regions  |\n");
    printf("-------------------------------\n");
    for (i = 0; i < n_ut; i++) {
        seL4_Word start, end;
        int bits;
        int is_dev;
        start = bi->untypedList[i].paddr;
        bits = bi->untypedList[i].sizeBits;
        end = start + (1U << bits);
        is_dev = bi->untypedList[i].isDevice;
        if (is_dev) {
            printf("dev| 0x%016lx->0x%016lx (%2d) |\n", start, end, bits);
        } else {
            printf("mem| 0x%016lx->0x%016lx (%2d) |\n", start, end, bits);
        }
    }
    printf("-------------------------------\n\n");
}

static int
_dma_morecore(size_t min_size, int cached, struct dma_mem_descriptor* dma_desc)
{
    static seL4_Word _vaddr = DMA_VSTART;
    struct seL4_ARM_Page_GetAddress getaddr_ret;
    seL4_CPtr frame;
    seL4_CPtr pd;
    vka_t* vka;
    int err;

    pd = simple_get_pd(&_simple);
    vka = &_vka;

    /* Create a frame */
    frame = vka_alloc_frame_leaky(vka, 12);
    assert(frame);
    if (!frame) {
        return -1;
    }

    /* Try to map the page */
    err = seL4_ARM_Page_Map(frame, pd, _vaddr, seL4_AllRights, 0);
    if (err) {
        seL4_CPtr pt;
        /* Allocate a page table */
        pt = vka_alloc_page_table_leaky(vka);
        if (!pt) {
            printf("Failed to create page table\n");
            return -1;
        }
        /* Map the page table */
        err = seL4_ARM_PageTable_Map(pt, pd, _vaddr, 0);
        if (err) {
            printf("Failed to map page table\n");
            return -1;
        }
        /* Try to map the page again */
        err = seL4_ARM_Page_Map(frame, pd, _vaddr, seL4_AllRights, 0);
        if (err) {
            printf("Failed to map page\n");
            return -1;
        }
    }

    /* Find the physical address of the page */
    getaddr_ret = seL4_ARM_Page_GetAddress(frame);
    assert(!getaddr_ret.error);
    /* Setup dma memory description */
    dma_desc->vaddr = _vaddr;
    dma_desc->paddr = getaddr_ret.paddr;
    dma_desc->cached = 0;
    dma_desc->size_bits = 12;
    dma_desc->alloc_cookie = (void*)frame;
    dma_desc->cookie = NULL;
    /* Advance the virtual address marker */
    _vaddr += BIT(12);
    return 0;
}



static int
vmm_init(void)
{
    vka_object_t fault_ep_obj;
    vka_t* vka;
    simple_t* simple;
    vspace_t* vspace;
    int err;

    vka = &_vka;
    vspace = &_vspace;
    simple = &_simple;
    fault_ep_obj.cptr = 0;
    simple_default_init_bootinfo(simple, seL4_GetBootInfo());
    _allocator = bootstrap_use_current_simple(simple, ALLOCATOR_STATIC_POOL_SIZE,
                                              allocator_mem_pool);
    assert(_allocator);
    allocman_make_vka(vka, _allocator);

    for (int i = 0; i < simple_get_untyped_count(simple); i++) {
        size_t size;
        uintptr_t paddr;
        bool device;
        seL4_CPtr cap = simple_get_nth_untyped(simple, i, &size, &paddr, &device);
        cspacepath_t path;
        vka_cspace_make_path(vka, cap, &path);
        int utType = device ? ALLOCMAN_UT_DEV : ALLOCMAN_UT_KERNEL;
        if (utType == ALLOCMAN_UT_DEV &&
            paddr >= (LINUX_RAM_BASE + LINUX_RAM_OFFSET) &&
            paddr <= (LINUX_RAM_BASE + LINUX_RAM_OFFSET + (LINUX_RAM_SIZE - 1))) {
            utType = ALLOCMAN_UT_DEV_MEM;

            err = allocman_utspace_add_uts(_allocator, 1, &path, &size, &paddr, utType);
            assert(!err);
        }
    }

    err = sel4utils_bootstrap_vspace_with_bootinfo_leaky(vspace,
                                                         &_alloc_data,
                                                         seL4_CapInitThreadPD,
                                                         vka,
                                                         seL4_GetBootInfo());
    assert(!err);

    /* Initialise device support */
    err = sel4platsupport_new_io_mapper(*vspace, *vka, &_io_ops.io_mapper);
    assert(!err);

    /* Setup debug port: printf() is only reliable after this */
    platsupport_serial_setup_simple(NULL, simple, vka);

    /* Initialise DMA */
    err = dma_dmaman_init(&_dma_morecore, NULL, &_io_ops.dma_manager);
    assert(!err);

    /* Allocate an endpoint for listening to events */
    err = vka_alloc_endpoint(vka, &fault_ep_obj);
    assert(!err);
    _fault_endpoint = fault_ep_obj.cptr;

    /* Create an IRQ server */
    err = irq_server_new(vspace, vka, simple_get_cnode(simple), IRQSERVER_PRIO,
                         simple, fault_ep_obj.cptr,
                         IRQ_MESSAGE_LABEL, 256, &_irq_server);
    assert(!err);

    return 0;
}

static bool
all_vm_shutdown(void)
{
  static int num_vm_shutdown = 0;
  num_vm_shutdown++;
  return (num_vm_shutdown >= NUM_VMS);
}

static int
check_vchans(struct vchan_device **linux_vchan_devices, int num_vchans)
{
    assert(num_vchans < MAX_COMM_CHANNELS);

    for (int i = 0; i < num_vchans; i++)
    {
        if (linux_vchan_devices[i]->source.vmid == linux_vchan_devices[i]->destination.vmid)
        {
            printf("Warning: Vchan connects VM to itself\n");
        }

        /* We need to make sure the destination and source VMs exists... */
        if ((linux_vchan_devices[i]->source.vmid >= NUM_VMS) && (linux_vchan_devices[i]->source.vmid >= 0))
        {
            printf("Error - Source VM doesn't exist\n");
            return 1;
        }

        if ((linux_vchan_devices[i]->destination.vmid >= NUM_VMS) && (linux_vchan_devices[i]->destination.vmid >= 0))
        {
            printf("Error - Destination VM doesn't exist\n");
            return 1;
        }

        for (int j = 0; j < num_vchans; j++)
        {
            if ((linux_vchan_devices[i]->port == linux_vchan_devices[j]->port) && (i != j)) {
                printf("Error - Vchan port %d not unique\n", linux_vchan_devices[i]->port);
                return 1;
            }
        }
    }
    return 0;
}

static void
comm_server_init(vm_t *vm, struct vchan_device **linux_vchan_devices, int num_vchans)
{
    int error UNUSED;

    assert(check_vchans(linux_vchan_devices, num_vchans) == 0);

    for(int i = 0; i < num_vchans; i++) {

        struct vchan_device *curr_device = linux_vchan_devices[i];
        int source_vmid = curr_device->source.vmid;
        int dest_vmid = curr_device->destination.vmid;

        sel4utils_process_t comm_process;
        sel4utils_process_config_t config;

        config = process_config_default_simple(&_simple, COMM_SERVER_NAME, COMM_SERVER_PRIO);
        error = sel4utils_configure_process_custom(&comm_process, &_vka, &_vspace, config);
        assert(error == 0);

        /* create an endpoint */
        vka_object_t ep_object = {0};
        error = vka_alloc_endpoint(&_vka, &ep_object);
        assert(error == 0);

        /* make a cspacepath for the new endpoint cap */
        cspacepath_t vm_channel_sig, vm_channel_sig_signed;
        seL4_CPtr comm_server_cap = 0;

        unsigned int badge = curr_device->port;

        vka_cspace_make_path(&_vka, ep_object.cptr, &vm_channel_sig);

        vka_cspace_alloc_path(&_vka, &vm_channel_sig_signed);
        vka_cnode_mint(&vm_channel_sig_signed, &vm_channel_sig, seL4_AllRights, badge);

        /* copy the endpont cap and add a badge to the new cap */
        comm_server_cap = sel4utils_mint_cap_to_process(&comm_process, vm_channel_sig, seL4_AllRights, badge);

        /* Delete the original so we don't have two copies of the same capability */
        vka_cnode_delete(&vm_channel_sig);

        /* Now we need to get the capability to the read page and copy
         * it to the communication server. Pages should be non-cached.
         */
        void * comm_server_read_page = vspace_share_mem(&vm[source_vmid].vm_vspace, &comm_process.vspace,
                                                        (void*)PAGE_ALIGN_4K(curr_device->pread),
                                                        1, 12, seL4_CanRead, 0);

        void * comm_server_write_page = vspace_share_mem(&vm[dest_vmid].vm_vspace, &comm_process.vspace,
                                                         (void*)PAGE_ALIGN_4K(curr_device->pwrite),
                                                         1, 12, seL4_AllRights, 0);

        assert(comm_server_read_page != NULL);
        assert(comm_server_write_page != NULL);

        /* Create a TCB object so that the comm server can create a new thread */
        vka_object_t tcb;
        error = vka_alloc_tcb(&_vka, &tcb);
        assert(error == 0);

        seL4_CPtr tcb_slot = sel4utils_copy_cap_to_process(&comm_process, &_vka, tcb.cptr);
        seL4_CPtr ipc_slot = sel4utils_copy_cap_to_process(&comm_process, &_vka, comm_process.thread.ipc_buffer);

        /* Spawn the process */
        seL4_Word argc = 6;
        char string_args[argc][WORD_STRING_SIZE];
        char* argv[argc];
        sel4utils_create_word_args(string_args, argv, argc, comm_server_cap, comm_server_read_page, comm_server_write_page,
                                   tcb_slot, ipc_slot, comm_process.thread.ipc_buffer_addr);

        error = sel4utils_spawn_process_v(&comm_process, &_vka, &_vspace, argc, (char**) &argv, 1);
        assert(error == 0);

        /* We need to set the comm server's priority because somehow its not happening and the comm server needs to
         * spawn a new thread!
         */
        seL4_TCB_SetSchedParams(comm_process.thread.tcb.cptr, simple_get_tcb(&_simple), COMM_SERVER_PRIO - 1, COMM_SERVER_PRIO - 1);

        curr_device->comm_ep = vm_channel_sig_signed.capPtr;

        assert(add_vchan(curr_device) == 0);
    }
}

int get_vm_id_by_badge(seL4_Word sender_badge)
{
  for(int i = 0; i < NUM_VMS; i++)
  {
    if(vm_confs[i].badge == sender_badge)
    {
      return i;
    }
  }

  return -1;
}

int main(void)
{
    vm_t vm[NUM_VMS];
    int err;
    int i;

    err = vmm_init();
    assert(!err);

    (void)print_boot_info;

    print_cpio_info();

    err = virtual_devices_init(&_io_ops);
    assert(!err);

    for(i = 0; i<NUM_VMS; i++)
    {
      /* Create the VM */
      err = vm_create(vm_confs[i].vm_name, vm_confs[i].priority, _fault_endpoint, vm_confs[i].badge,
                      &_vka, &_simple, &_vspace, &_io_ops, &vm[i]);
      if (err) {
          printf("Failed to create VM\n");
          seL4_DebugHalt();
          return -1;
      }

#ifdef CONFIG_TK1_SMMU
      int iospace_caps;
      err = simple_get_iospace_cap_count(&_simple, &iospace_caps);
      if (err) {
          ZF_LOGF("Failed to get iospace count");
      }
      for (int j = 0; j < iospace_caps; j++) {
          seL4_CPtr iospace = simple_get_nth_iospace_cap(&_simple, j);
          err = vmm_guest_vspace_add_iospace(&_vspace, &vm[i].vm_vspace, iospace);
          if (err) {
              ZF_LOGF("Fail to add iospace");
          }
      }
#endif

      vm[i].linux_name = vm_confs[i].linux_name;
      vm[i].dtb_name = vm_confs[i].dtb_name;
      vm[i].ondemand_dev_install = 0;
      vm[i].linux_base = vm_confs[i].linux_base;
      vm[i].map_unity = vm_confs[i].map_unity;
      vm[i].linux_size = vm_confs[i].linux_size;

      /* Load system images */
      printf("Loading Linux: \'%s\' dtb: \'%s\'\n", vm_confs[i].linux_name, vm[i].dtb_name);

      err = load_linux(&vm[i], vm_confs[i].linux_name, vm[i].dtb_name, vm_confs[i].linux_pt_devices,
                       vm_confs[i].num_devices, vm_confs[i].linux_pt_irqs, vm_confs[i].num_irqs);

      if (err) {
          printf("Failed to load VM image\n");
          seL4_DebugHalt();
          return -1;
      }

      printf("Loaded Linux\n");

      /* Power on */
      err = vm_start(&vm[i]);
      if (err) {
          printf("Failed to start VM %d\n", i);
          seL4_DebugHalt();
          return -1;
      }

    }

    /* We need to make sure both VSpaces are setup if we want to map shared pages */
    comm_server_init(&vm[0], vchan_config, NUM_VCHANS);

    /* Loop forever, handling events */
    while (1) {
        seL4_MessageInfo_t tag;
        seL4_Word sender_badge;
        int vm_id;

        tag = seL4_Recv(_fault_endpoint, &sender_badge);

        if (sender_badge == 0) {
            seL4_Word label;
            label = seL4_MessageInfo_get_label(tag);
            if (label == IRQ_MESSAGE_LABEL) {
                irq_server_handle_irq_ipc(_irq_server);
            } else {
                printf("Unknown label (%d) for IPC badge %d\n", (int)label, (int)sender_badge);
            }
        } else {
            vm_id = get_vm_id_by_badge(sender_badge);
            assert(vm_id >= 0);
            err = vm_event(&vm[vm_id], tag);
            if (err == RESTART_VM) {
                restart_linux(&vm[vm_id]);
                printf("vm (%s) restart\n", vm[vm_id].name);
            } else if (err || err == SHUTDOWN_VM) {
                /* Shutdown */
#ifdef VUART_ENABLED
                vm_uninstall_vconsole(&vm[vm_id]);
#endif
                vm_stop(&vm[vm_id]);
                printf("vm (%s) halts\n", vm[vm_id].name);

                if (all_vm_shutdown()) {
                    break;
                }
            }
        }
    }

    return 0;
}
