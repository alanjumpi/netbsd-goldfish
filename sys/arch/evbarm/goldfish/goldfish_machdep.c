/* XXX: check these includes */
#include "opt_ddb.h"
#include "opt_pmap_debug.h"

#include <sys/param.h>
#include <sys/device.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/exec.h>
#include <sys/proc.h>
#include <sys/msgbuf.h>
#include <sys/reboot.h>
#include <sys/termios.h>
#include <sys/ksyms.h>
#include <sys/bus.h>
#include <sys/cpu.h>
#include <sys/intr.h>

#include <uvm/uvm_extern.h>

#include <dev/cons.h>

#include <machine/db_machdep.h>
#include <ddb/db_sym.h>
#include <ddb/db_extern.h>

#include <machine/bootconfig.h>
#include <arm/locore.h>
#include <arm/undefined.h>

#include <arm/arm32/machdep.h>

#include <evbarm/goldfish/goldfishvar.h>
#include <evbarm/goldfish/goldfish_com.h>

#include "ksyms.h" /* XXX: what's this? */

/* mostly copied from integrator/integrator_machdep.c */

/*
 * The range 0xc1000000 - 0xccffffff is available for kernel VM space
 * Goldfish I/O starts at 0xff000000
 */
#define KERNEL_VM_BASE		(KERNEL_BASE + 0x01000000)
#define KERNEL_VM_SIZE		0x0C000000

BootConfig bootconfig;		/* Boot config storage */
char *boot_args = NULL;
char *boot_file = NULL;

/* Prototypes */

void	consinit(void);

/*
 * void cpu_reboot(int howto, char *bootstr)
 *
 * Reboots the system
 *
 * Deal with any syncing, unmounting, dumping and shutdown hooks,
 * then reset the CPU.
 */
void
cpu_reboot(int howto, char *bootstr)
{
	/* XXX: no idea what to do here */
}

/* Statically mapped devices. */
static const struct pmap_devmap goldfish_devmap[] = {
	{
		GOLDFISH_IO_VBASE,	/* virtual */
		GOLDFISH_IO_HWBASE,	/* physical */
		1024*1024,
		VM_PROT_READ|VM_PROT_WRITE,
		PTE_NOCACHE
	},
	{
		0,
		0,
		0,
		0,
		0
	}
};

/*
 * u_int initarm(...)
 *
 * Initial entry point on startup. This gets called before main() is
 * entered.
 * It should be responsible for setting up everything that must be
 * in place when main is called.
 * This includes
 *   Taking a copy of the boot configuration structure.
 *   Initialising the physical console so characters can be printed.
 *   Setting up page tables for the kernel
 *   Relocating the kernel to the bottom of physical memory
 */

u_int
initarm(void *arg)
{
	extern int KERNEL_BASE_phys[];
	paddr_t memstart;
	psize_t memsize;

	/*
	 * Heads up ... Setup the CPU / MMU / TLB functions
	 */
	if (set_cpufuncs())
		panic("cpu not recognized!");

	/* map some peripheral registers */

	pmap_devmap_bootstrap((vaddr_t)armreg_ttbr_read() & -L1_TABLE_SIZE,
		goldfish_devmap);

	cpu_domains((DOMAIN_CLIENT << (PMAP_DOMAIN_KERNEL*2)) | DOMAIN_CLIENT);

	consinit();

	/* Talk to the user */
#define BDSTR(s)        _BDSTR(s)
#define _BDSTR(s)       #s
	printf("\nNetBSD/evbarm (" BDSTR(EVBARM_BOARDTYPE) ") booting ...\n");

	/* XXX: hardcoded for now */
	memstart = 0;
	memsize = 64*1024*1024; /* 64 MB */

#ifdef VERBOSE_INIT_ARM
	printf("initarm: Configuring system ...\n");
#endif

	/* Fake bootconfig structure for the benefit of pmap.c */
	/* XXX must make the memory description h/w independent */
	bootconfig.dramblocks = 1;
	bootconfig.dram[0].address = memstart;
	bootconfig.dram[0].pages = memsize / PAGE_SIZE;
	bootconfig.dram[0].flags = BOOT_DRAM_CAN_DMA | BOOT_DRAM_PREFER;

	arm32_bootmem_init(bootconfig.dram[0].address,
		bootconfig.dram[0].pages * PAGE_SIZE, (unsigned int)KERNEL_BASE_phys);

	arm32_kernel_vm_init(KERNEL_VM_BASE, ARM_VECTORS_LOW, 0, goldfish_devmap,
		false);

#ifdef VERBOSE_INIT_ARM
	printf("done.\n");
#endif

	return initarm_common(KERNEL_VM_BASE, KERNEL_VM_SIZE, NULL, 0);
}

void
consinit(void)
{
	extern struct bus_space goldfish_common_bs_tag;
	static int consinit_called = 0;

	if (consinit_called != 0)
		return;

	consinit_called = 1;

	if (goldfish_cnattach(&goldfish_common_bs_tag, GOLDFISH_UART0_HWBASE))
		panic("can't init serial console");
}
