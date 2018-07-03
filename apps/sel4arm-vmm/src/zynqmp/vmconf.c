/*
 * Copyright 2018, DornerWorks
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DORNERWORKS_BSD)
 */

#include "../vmlinux.h"

vmconf_t vm_confs[NUM_VMS] =
{
  {
    .priority = VM_PRIO,
    .badge = 1U,
    .linux_name = "linux-1",
    .dtb_name = "linux-1-dtb",
    .vm_name = "Linux 1",
    .linux_base = 0x800000000,
    .linux_size = 0x10000000,
    .linux_pt_devices = {
        &dev_gem3,
    },
    .num_devices = 1,
    .linux_pt_irqs = {
        INTERRUPT_CORE_VIRT_TIMER,
        INTERRUPT_GEM3,
    },
    .num_irqs = 2,
    .map_unity = 0
  },
  {
    .priority = VM_PRIO,
    .badge = 2U,
    .linux_name = "linux-1",
    .dtb_name = "linux-2-dtb",
    .vm_name = "Linux 2",
    .linux_base = 0x810000000,
    .linux_size = 0x10000000,
    .linux_pt_devices = {
    },
    .num_devices = 0,
    .linux_pt_irqs = {
        INTERRUPT_CORE_VIRT_TIMER,
    },
    .num_irqs = 1,
    .map_unity = 0
  }
};

struct vchan_device dev_vchan0 = {
    .name = "vchan0",
    .pread = 0x80ffff000,
    .pwrite = 0x81fffe000,
    .source = {
        .vmid = 0,
        .registered = -1
    },
    .destination = {
        .vmid = 1,
        .registered = -1
    },
    .port = 80
};

struct vchan_device dev_vchan1 = {
    .name = "vchan1",
    .pread = 0x81ffff000,
    .pwrite = 0x80fffe000,
    .source = {
        .vmid = 1,
        .registered = -1
    },
    .destination = {
        .vmid = 0,
        .registered = -1
    },
    .port = 50
};

struct vchan_device *vchan_config[NUM_VCHANS] =
{
    &dev_vchan0,
    &dev_vchan1,
};
