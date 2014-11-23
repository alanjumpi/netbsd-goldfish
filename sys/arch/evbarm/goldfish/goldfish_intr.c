#ifndef EVBARM_SPL_NOINLINE
#define	EVBARM_SPL_NOINLINE
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/bus.h>
#include <sys/intr.h>
#include <arm/cpufunc.h>

struct intrhand {
	TAILQ_ENTRY(intrhand) list;	/* link on intrq list */
	int (*func)(void *);		/* handler */
	void *arg;			/* arg for handler */
	int irq;			/* IRQ number */
};

struct intrq {
	TAILQ_HEAD(, intrhand) list;
};

#define NIRQ 32
static struct intrq intrq[NIRQ];

void goldfish_intr_dispatch(struct clockframe *frame);

void goldfish_intr_init(void);
void *goldfish_intr_establish(int, int, int(*)(void *), void *);
void goldfish_intr_disestablish(void *);

void
splx(int new)
{
	goldfish_splx(new);
}

int
_spllower(int ipl)
{
	return goldfish_spllower(ipl);
}

int
_splraise(int ipl)
{
	return goldfish_splraise(ipl);
}

void
goldfish_intr_init(void)
{
	for (int i = 0; i < NIRQ; i++) {
		struct intrq *iq = &intrq[i];
		TAILQ_INIT(&iq->list);
		/* XXX: evcnt_attach_dynamic?! */
	}

	enable_interrupts(I32_bit);
}

void *
goldfish_intr_establish(int irq, int ipl, int (*func)(void *), void *arg)
{
	struct intrq *iq;
	struct intrhand *ih;
	uint32_t prev_irqstate;

	if (irq < 0 || irq > NIRQ)
		panic("%s: IRQ %d out of range", __FUNCTION__, irq);

	ih = malloc(sizeof *ih, M_DEVBUF, M_NOWAIT);
	if (ih == NULL)
		return NULL;

	ih->func = func;
	ih->arg = arg;
	ih->irq = irq;

	iq = &intrq[irq];

	prev_irqstate = disable_interrupts(I32_bit);
	TAILQ_INSERT_TAIL(&iq->list, ih, list);
	restore_interrupts(prev_irqstate);

	return NULL;
}

void
goldfish_intr_disestablish(void *cookie)
{
	struct intrhand *ih = cookie;
	struct intrq *iq = &intrq[ih->irq];
	uint32_t prev_irqstate;

	prev_irqstate = disable_interrupts(I32_bit);
	TAILQ_REMOVE(&iq->list, ih, list);
	restore_interrupts(prev_irqstate);
}

void
goldfish_intr_dispatch(struct clockframe *frame)
{
	/* XXX */
}
