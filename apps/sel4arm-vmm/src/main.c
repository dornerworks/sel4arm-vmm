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

#include "vmlinux.h"

#include <sel4arm-vmm/plat/devices.h>
#include <sel4arm-vmm/devices/vgic.h>
#include <sel4arm-vmm/devices/vram.h>
#include <sel4arm-vmm/devices/vusb.h>

#define VM_PRIO      100
#define VM_NAME_LEN  32

#define NUM_VMS 2

typedef struct vm_conf {
  int priority;
  seL4_Word badge;
  char linux_name[VM_NAME_LEN];
  char dtb_name[VM_NAME_LEN];
  char vm_name[VM_NAME_LEN];
  uintptr_t linux_base;
  const struct device *linux_pt_devices[MAX_DEVICES_PER_VM];
  int num_devices;
  int linux_pt_irqs[MAX_PASSTHROUGH_IRQS];
  int num_irqs;
} vmconf_t;

vmconf_t vm_confs[NUM_VMS] =
{
  {
    .priority = VM_PRIO,
    .badge = 1U,
    .linux_name = "linux",
    .dtb_name = "linux-1-dtb",
    .vm_name = "Linux 1",
    .linux_base = 0x800000000,
    .linux_pt_devices = {
    },
    .num_devices = 0,
    .linux_pt_irqs = {
        INTERRUPT_CORE_VIRT_TIMER,
    },
    .num_irqs = 1
  },
  {
    .priority = VM_PRIO,
    .badge = 2U,
    .linux_name = "linux",
    .dtb_name = "linux-2-dtb",
    .vm_name = "Linux 2",
    .linux_base = 0x810000000,
    .linux_pt_devices = {
    },
    .num_devices = 0,
    .linux_pt_irqs = {
        INTERRUPT_CORE_VIRT_TIMER,
    },
    .num_irqs = 1
  }
};

#define IRQSERVER_PRIO      (VM_PRIO + 1)
#define IRQ_MESSAGE_LABEL   0xCAFE

#define DMA_VSTART  0x40000000

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
    static uint32_t _vaddr = DMA_VSTART;
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

      // TBD
#ifdef CONFIG_ARM_SMMU
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

      vm[i].dtb_name = vm_confs[i].dtb_name;
      vm[i].ondemand_dev_install = 0;
      vm[i].linux_base = vm_confs[i].linux_base;

      // TBD
#ifdef CONFIG_ARM_SMMU
      /* enable the direct device access for this VM */
      vmm_guest_iospace_map_set(&vm[i].vm_vspace);
#endif

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
            if (err) {
                /* Shutdown */
                vm_uninstall_vconsole(&vm[vm_id]);
                vm_stop(&vm[vm_id]);
                printf("vm (%s) halts\n", vm[vm_id].name);
            }
        }
    }

    return 0;
}
