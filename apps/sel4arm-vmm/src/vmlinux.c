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

#include <vmlinux.h>

#include <string.h>

#include <vka/capops.h>

#include <sel4arm-vmm/vm.h>
#include <sel4arm-vmm/images.h>
#include <sel4arm-vmm/plat/devices.h>
#include <sel4arm-vmm/devices/vgic.h>
#include <sel4arm-vmm/devices/vram.h>
#include <sel4arm-vmm/devices/vusb.h>
#include <sel4utils/irq_server.h>
#include <cpio/cpio.h>

#define DTB_OFFSET           (0x0C000000)
#define MACH_TYPE_SPECIAL    ~0
#define MACH_TYPE            MACH_TYPE_SPECIAL

extern char _cpio_archive[];

extern vka_t _vka;
extern vspace_t _vspace;
extern irq_server_t _irq_server;
extern seL4_CPtr _fault_endpoint;

static struct irq_data* linux_irq_data[MAX_IRQ + 1] = { 0 };

struct pwr_token {
    const char* linux_bin;
    const char* device_tree;
} pwr_token;

#ifdef VUART_ENABLED
static struct ps_chardevice _char_dev;

static void
uart_irq_handler(struct irq_data* irq_data)
{
    struct ps_chardevice* char_dev = (struct ps_chardevice*)irq_data->token;

    ps_cdev_handle_irq(char_dev, irq_data->irq);
    vuart_handle_irq();
    irq_data_ack_irq(irq_data);
}
#endif

static int
install_vuart(vm_t* vm)
{
    int err = 0;
#ifdef VUART_ENABLED
    /* Install the virtual device */
    err = vm_install_vconsole(vm, VUART_IRQ);
    assert(!err);
#endif
    return err;
}

int virtual_devices_init(struct ps_io_ops* io_ops)
{
#ifdef VUART_ENABLED
  struct ps_chardevice *temp_device;
  irq_server_t irq_server;
  struct irq_data* irq_data;

  irq_server = _irq_server;

  temp_device = vuart_init(io_ops);
  assert(NULL != temp_device);
  if(NULL == temp_device)
  {
    return -1;
  }
  _char_dev = *temp_device;

  /* Route physical IRQs */
  irq_data = irq_server_register_irq(irq_server, INTERRUPT_VCONSOLE, uart_irq_handler, &_char_dev);
  if (!irq_data) {
      return -1;
  }
#endif

  return 0;
}

static int
vm_install_map_ram(vm_t *vm)
{
    struct device d;
    d = dev_vram;
    d.pstart = vm->linux_base;
    return vm_install_ram_only_device(vm, &d);
}

static int
install_linux_devices(vm_t* vm, const struct device **linux_pt_devices, int num_devices)
{
    int err;
    int i;

    /* Install virtual devices */
    err = vm_install_vgic(vm);
    assert(!err);

    err = vm_install_map_ram(vm);
    assert(!err);

    err = install_vuart(vm);
    assert(!err);

    /* Install pass through devices */
    for (i = 0; i < num_devices; i++) {
        assert(NULL != linux_pt_devices[i]);
        err = vm_install_passthrough_device(vm, linux_pt_devices[i]);
        assert(!err);
    }

    return 0;
}

static void
do_irq_server_ack(void* token)
{
    struct irq_data* irq_data = (struct irq_data*)token;
    irq_data_ack_irq(irq_data);
}

static void
irq_handler(struct irq_data* irq_data)
{
    virq_handle_t virq;
    int err = 0;
    virq = (virq_handle_t)irq_data->token;
    while (virq != NULL) {
        err = vm_inject_IRQ(virq);
        virq = (virq_handle_t)virq->next;
    }
    assert(!err);
}

static int
route_irqs(vm_t* vm, irq_server_t irq_server, int *linux_pt_irqs, int num_irqs)
{
    int i;
    int *irq_array = NULL;
    int nirqs = 0;

    if (vm->npassthrough_irqs > 0) {
        /* use the per-VM configuration */
        irq_array = vm->passthrough_irqs;
        nirqs = vm->npassthrough_irqs;
    } else {
        /* or the global configuration */
        irq_array = linux_pt_irqs;
        nirqs = num_irqs;
    }

    for (i = 0; i < nirqs; i++) {
        irq_t irq = irq_array[i];
        struct irq_data* irq_data = linux_irq_data[irq];
        virq_handle_t virq;
        void (*handler)(struct irq_data*);
        handler = &irq_handler;
        if (irq_data == 0) {
            irq_data = irq_server_register_irq(irq_server, irq, handler, NULL);
            if (!irq_data) {
                return -1;
            }
            linux_irq_data[irq] = irq_data;
        }
        virq = vm_virq_new(vm, irq, &do_irq_server_ack, irq_data);
        if (virq == NULL) {
            return -1;
        }
        virq->next = irq_data->token;
        irq_data->token = (void*)virq;
    }
    return 0;
}

static uint64_t
install_linux_dtb(vm_t* vm, const char* dtb_name)
{
    void* file;
    unsigned long size;
    uint64_t dtb_addr;

    /* Retrieve the file data */
    file = cpio_get_file(_cpio_archive, dtb_name, &size);
    if (file == NULL) {
        printf("Error: Linux dtb file \'%s\' not found\n", dtb_name);
        return 0;
    }
    if (image_get_type(file) != IMG_DTB) {
        printf("Error: \'%s\' is not a device tree\n", dtb_name);
        return 0;
    }

    /* Copy the tree to the VM */
    dtb_addr = vm->linux_base + DTB_OFFSET;
    if (vm_copyout(vm, file, dtb_addr, size)) {
        printf("Error: Failed to load device tree \'%s\'\n", dtb_name);
        return 0;
    } else {
        return dtb_addr;
    }

}

static void*
install_linux_kernel(vm_t* vm, const char* kernel_name)
{
    void* file;
    unsigned long size;
    uintptr_t entry;

    /* Retrieve the file data */
    file = cpio_get_file(_cpio_archive, kernel_name, &size);
    if (file == NULL) {
        printf("Error: Unable to find kernel image \'%s\'\n", kernel_name);
        return NULL;
    }

    /* Determine the load address */
    switch (image_get_type(file)) {
    case IMG_BIN:
        entry = vm->linux_base + 0x80000;
        break;
    case IMG_ZIMAGE:
        entry = zImage_get_load_address(file, vm->linux_base);
        break;
    default:
        printf("Error: Unknown Linux image format for \'%s\'\n", kernel_name);
        return NULL;
    }

    /* Load the image */
    if (vm_copyout(vm, file, entry, size)) {
        printf("Error: Failed to load \'%s\'\n", kernel_name);
        return NULL;
    } else {
        return (void*)entry;
    }
}

int
load_linux(vm_t* vm, const char* kernel_name, const char* dtb_name, const struct device **linux_pt_devices,
           int num_devices, int *linux_pt_irqs, int num_irqs)
{
    void* entry;
    uint64_t dtb;
    int err;
    if (vm->dtb_name != NULL) dtb_name = vm->dtb_name;

    pwr_token.linux_bin = kernel_name;
    pwr_token.device_tree = dtb_name;

    /* Install devices */
    err = install_linux_devices(vm, linux_pt_devices, num_devices);
    if (err) {
        printf("Error: Failed to install Linux devices\n");
        return -1;
    }
    /* Route IRQs */
    err = route_irqs(vm, _irq_server, linux_pt_irqs, num_irqs);
    if (err) {
        return -1;
    }
    /* Load kernel */
    entry = install_linux_kernel(vm, kernel_name);
    if (!entry) {
        return -1;
    }
    /* Load device tree */
    dtb = install_linux_dtb(vm, dtb_name);
    if (!dtb) {
        return -1;
    }
    /* Set boot arguments */
    err = vm_set_bootargs(vm, entry, MACH_TYPE, dtb);
    if (err) {
        printf("Error: Failed to set boot arguments\n");
        return -1;
    }

    return 0;
}
