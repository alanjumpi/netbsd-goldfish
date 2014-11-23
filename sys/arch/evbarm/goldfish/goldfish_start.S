#include <machine/asm.h>
#include <arm/armreg.h>
#include "assym.h"

	/* most of this was copied from integrator/intmmu.S */

	.section .start,"ax",%progbits

ASENTRY_NP(goldfish_start)
/*
 * At this time the MMU is off.
 * We build up an initial memory map at 0x8000 that we can use to get
 * the kernel running from the top of memory.  All mappings in this table
 * use L1 section maps.
 */

/*
 * Set Virtual == Physical
 */
	mov	r3, #(L1_S_AP_KRW)
	add	r3, r3, #(L1_TYPE_S)
	mov	r2, #0x100000		/* advance by 1MB */
	mov	r1, #0x8000		/* page table start */
	mov	r0, #0x1000		/* page table size */

Lflat:
	str	r3, [r1], #0x0004
	add	r3, r3, r2
	subs	r0, r0, #1
	bgt	Lflat

/*
 * Map VA 0xc0000000->0xc03fffff to PA 0x00000000->0x003fffff
 */
	mov	r3, #(L1_S_AP_KRW)
	add	r3, r3, #(L1_TYPE_S)
	mov	r1, #0x8000		/* page table start */
	add	r1, r1, #(0xc00 * 4)	/* offset to 0xc00xxxxx */
	mov	r0, #63
Lkern:
	str	r3, [r1], #0x0004	/* 0xc000000-0xc03fffff */
	add	r3, r3, r2
	subs	r0, r0, #1
	bgt	Lkern

/*
 * We now have our page table ready, so load it up and light the blue
 * touch paper.
 */

	/* set the location of the L1 page table */
	mov	r1, #0x8000
	mcr	p15, 0, r1, c2, c0, 0

	/* Flush the old TLBs (just in case) */
	mcr	p15, 0, r1, c8, c7, 0
	/* And the caches */
	mov	r0, #0
	mcr	p15, 0, r1, c7, c6, 0

	/* Set the Domain Access register.  Very important! */
	mov	r1, #1
	mcr	p15, 0, r1, c3, c0, 0

	/*
	 * set mmu bit
	 */
	mov	r1, #CPU_CONTROL_MMU_ENABLE

	/* fetch current control state */
	mrc	p15, 0, r2, c1, c0, 0
	orr	r2, r2, r1

	/* set new control state */
	mcr	p15, 0, r2, c1, c0, 0

	mov	r0, r0
	mov	r0, r0
	mov	r0, r0

	/* jump to kernel space */
	mov	r0, #0x0200

	/* Switch to kernel VM and really set the ball rolling.  */
	ldr	pc, Lstart

Lstart:	.long	start
