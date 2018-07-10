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

#ifndef VMLINUX_H
#define VMLINUX_H

#include <sel4arm-vmm/vm.h>
#include <sel4arm-vmm/plat/devices.h>

#include <vmconf.h>

#define VM_NAME_LEN  32
#define VM_PRIO      100

/* Configuration for the communication server */
#define COMM_SERVER_NAME "communication-server"
#define COMM_SERVER_PRIO VM_PRIO

typedef struct vm_conf {
  int priority;
  seL4_Word badge;
  char linux_name[VM_NAME_LEN];
  char dtb_name[VM_NAME_LEN];
  char vm_name[VM_NAME_LEN];
  uintptr_t linux_base;
  uintptr_t linux_size;
  const struct device *linux_pt_devices[MAX_DEVICES_PER_VM];
  int num_devices;
  int linux_pt_irqs[MAX_PASSTHROUGH_IRQS];
  int num_irqs;
  int map_unity;
} vmconf_t;

int load_linux(vm_t* vm, const char* kernel_name, const char* dtb_name, const struct device **linux_pt_devices,
               int num_devices, int *linux_pt_irqs, int num_irqs);

int restart_linux(vm_t* vm);
int virtual_devices_init(struct ps_io_ops* io_ops);

#endif /* VMLINUX_H */
