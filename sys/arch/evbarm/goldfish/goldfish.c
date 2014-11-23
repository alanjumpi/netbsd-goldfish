/* XXX: check these includes */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/device.h>
#include <sys/systm.h>
#include <sys/extent.h>
#include <sys/malloc.h>
#include <sys/null.h>
#include <sys/bus.h>

#include <arm/cpufunc.h>
#include <evbarm/goldfish/goldfishvar.h>

static int goldfish_match(device_t, cfdata_t, void *);
static void goldfish_attach(device_t, device_t, void *);

CFATTACH_DECL_NEW(goldfish, sizeof(struct goldfish_softc), goldfish_match, goldfish_attach, NULL, NULL);

struct goldfish_softc *goldfish_sc;

static int
goldfish_print(void *aux, const char *pnp)
{
	/* struct goldfish_attach_args *args = aux */

	return UNCONF;
}

static int
goldfish_search(device_t parent, cfdata_t cf, const int *ldesc, void *aux)
{
	struct goldfish_softc *sc = device_private(parent);
	struct goldfish_attach_args args;
	int tryagain;

	do {
		args.iot = sc->iot;

		tryagain = 0;
		if (config_match(parent, cf, &args) > 0) {
			config_attach(parent, cf, &args, goldfish_print);
			tryagain = cf->cf_fstate == FSTATE_STAR;
		}
	} while (tryagain);

	return 0;
}

static int
goldfish_match(device_t parent, cfdata_t cf, void *aux)
{
	return 1;
}

static void
goldfish_attach(device_t parent, device_t self, void *aux)
{
	extern struct bus_space goldfish_common_bs_tag;
	struct goldfish_softc *sc = device_private(self);

	sc->iot = &goldfish_common_bs_tag;

	/* map the PIC */
	if (bus_space_map(sc->iot, GOLDFISH_PIC_HWBASE, GOLDFISH_PIC_IO_SIZE, 0, &sc->pic_ioh))
		panic("%s: Failed to map PIC registers", device_xname(self));

	/* actually enable interrupts */
	goldfish_intr_init();

	/* search for children */
	config_search_ia(goldfish_search, self, "goldfish", NULL);
}
