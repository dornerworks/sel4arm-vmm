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
    .linux_base = 0x90000000,
    .linux_size = 0x10000000,
    .linux_pt_devices = {
          /* The following devices we don't want to pass through, but need to for functionality
           * TODO: Investigate removing
           */
          &dev_mu0,
          &dev_lpcg_enet1,
          &dev_enet1,
    },
    .num_devices = 3,
    .linux_pt_irqs = {
        INTERRUPT_CORE_VIRT_TIMER,
        INTERRUPT_LS_MU0,
        INTERRUPT_ENET0_FRAME1,
        INTERRUPT_ENET0_FRAME2,
        INTERRUPT_ENET0_FRAME0_EVENT,
        INTERRUPT_ENET0_TIMER,
    },
    .num_irqs = 6,
    .map_unity = 0
  },
  {
    .priority = VM_PRIO,
    .badge = 2U,
    .linux_name = "linux-1",
    .dtb_name = "linux-2-dtb",
    .vm_name = "Linux 2",
    .linux_base = 0xA0000000,
    .linux_size = 0x10000000,
    .linux_pt_devices = {
        &dev_mu2,
    },
    .num_devices = 1,
    .linux_pt_irqs = {
        INTERRUPT_CORE_VIRT_TIMER,
        INTERRUPT_LS_MU2,
    },
    .num_irqs = 2,
    .map_unity = 0
  },
  {
    .priority = VM_PRIO,
    .badge = 3U,
    .linux_name = "linux-1",
    .dtb_name = "linux-3-dtb",
    .vm_name = "Linux 3",
    .linux_base = 0xB0000000,
    .linux_size = 0x10000000,
    .linux_pt_devices = {
        &dev_mu3,
    },
    .num_devices = 1,
    .linux_pt_irqs = {
        INTERRUPT_CORE_VIRT_TIMER,
        INTERRUPT_LS_MU3,
    },
    .num_irqs = 2,
    .map_unity = 0
  },
};
