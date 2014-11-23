/* XXX: check these includes */
#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/atomic.h>
#include <sys/time.h>
#include <sys/timetc.h>
#include <sys/device.h>

#include <arm/cpufunc.h>
#include <machine/intr.h>

void
setstatclockrate(int new_hz)
{
	/* XXX */
}
 
void
cpu_initclocks(void)
{
	/* XXX */
}

void
delay(u_int n)
{
	/* XXX */
}
