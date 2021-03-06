#define __ASM__
#include <sel4/arch/constants.h>

    .global _start

    .text

#ifdef X86_64

.align 0x1000
_start:
    leaq    _stack_top, %rsp

    /* Setup segment selector for IPC buffer access. */
    movw    $IPCBUF_GDT_SELECTOR, %ax
    movw    %ax, %gs

#   setup the bootinfo
    movq    %rbx, %rdi
    call    seL4_InitBootInfo

    call    main
# the return value is in rax
    movq    %rax, %rdi
    call    exit

1:  jmp     1b

#else

_start:
    leal    _stack_top, %esp

    /* Setup segment selector for IPC buffer access. */
    movw    $IPCBUF_GDT_SELECTOR, %ax
    movw    %ax, %gs

    /* Setup the global "bootinfo" structure. */
    pushl   %ebx
    call    seL4_InitBootInfo
    addl    $4, %esp

    /* Start main. */
    call    main

    /* Call exit with the return value from main. */
    pushl   %eax
    call    exit
1:  jmp     1b

#endif

    .bss
    .align  4

_stack_bottom:
    .space  65536
_stack_top:
