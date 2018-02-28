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
    .linux_name = "linux",
    .dtb_name = "linux-1-dtb",
    .vm_name = "Linux 1",
    .linux_base = 0x800000000,
    .linux_pt_devices = {
        &dev_gem3,
    },
    .num_devices = 1,
    .linux_pt_irqs = {
        INTERRUPT_CORE_VIRT_TIMER,
        INTERRUPT_GEM3,
    },
    .num_irqs = 2
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
