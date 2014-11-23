#ifndef _GOLDFISH_INTR_H_
#define _GOLDFISH_INTR_H_

#define	ARM_IRQ_HANDLER	_C_LABEL(goldfish_intr_dispatch)

/* mostly copied from ifpga/ifpga_intr.h */

#ifndef _LOCORE

#include <arm/cpu.h>
#include <arm/armreg.h>
#include <arm/cpufunc.h>

static inline void __attribute__((__unused__))
goldfish_splx(int new)
{
#if 0
	extern volatile uint32_t intr_enabled;
	extern volatile int goldfish_ipending;
	int oldirqstate, hwpend;

	__insn_barrier();

	oldirqstate = disable_interrupts(I32_bit);
	set_curcpl(new);

	hwpend = (goldfish_ipending & GOLDFISH_INTR_HWMASK) & ~new;
	if (hwpend != 0) {
		intr_enabled |= hwpend;
		goldfish_set_intrmask();
	}

	restore_interrupts(oldirqstate);
#endif

#ifdef __HAVE_FAST_SOFTINTS
	cpu_dosoftints();
#endif
}

static inline int __attribute__((__unused__))
goldfish_splraise(int ipl)
{
	/* extern int goldfish_imask[]; */
	const int old = curcpl();
	/* set_curcpl(old | goldfish_imask[ipl]); */

	__insn_barrier();

	return (old);
}

static inline int __attribute__((__unused__))
goldfish_spllower(int ipl)
{
	/* extern int goldfish_imask[]; */
	const int old = curcpl();

	/* goldfish_splx(goldfish_imask[ipl]); */
	return(old);
}

#if !defined(EVBARM_SPL_NOINLINE)

#define splx(new)		goldfish_splx(new)
#define	_spllower(ipl)		goldfish_spllower(ipl)
#define	_splraise(ipl)		goldfish_splraise(ipl)

#else

int	_splraise(int);
int	_spllower(int);
void	splx(int);

#endif /* ! EVBARM_SPL_NOINLINE */

#endif /* _LOCORE */

#endif /* _GOLDFISH_INTR_H_ */
