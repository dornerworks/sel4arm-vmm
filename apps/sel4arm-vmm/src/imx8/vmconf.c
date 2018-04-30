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
    .linux_base = 0x970000000,
    .linux_pt_devices = {
          &dev_uart0,
          /* The following devices we don't want to pass through, but need to for functionality
           * TODO: Investigate removing
           */
          &dev_uart0_dma,
          &dev_mu,
    },
    .num_devices = 2,
    .linux_pt_irqs = {
        INTERRUPT_CORE_VIRT_TIMER,
        INTERRUPT_UART0_MOD,
        INTERRUPT_LS_MU0,
    },
    .num_irqs = 3
  },
};
