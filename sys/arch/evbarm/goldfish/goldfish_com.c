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

#include <dev/cons.h>

#include <evbarm/goldfish/goldfishvar.h>
#include <evbarm/goldfish/goldfish_com.h>

struct goldfish_com_instance {
	bus_space_tag_t		iot;
	bus_space_handle_t	ioh;
	bus_addr_t		iobase;
};

static struct goldfish_com_instance cons_info;

static int goldfish_com_match(device_t parent, cfdata_t cf, void *aux);
static void goldfish_com_attach(device_t parent, device_t self, void *aux);

CFATTACH_DECL_NEW(goldfish_com, 0, goldfish_com_match, goldfish_com_attach, NULL, NULL);

static int
goldfish_com_match(device_t parent, cfdata_t cf, void *aux)
{
	return 1;
}

static void
goldfish_com_attach(device_t parent, device_t self, void *aux)
{
	/* XXX: map bus space */

	/* XXX: do something if this is the console */

	/* XXX: goldfish_intr_establish */
}

#if 0
void
goldfish_com_intr(void *arg)
{
}
#endif

static void
goldfish_com_putc(struct goldfish_com_instance *ci, int c)
{
	bus_space_write_1(ci->iot, ci->ioh, GOLDFISH_UART_PUT_CHAR, c);
}

static int
goldfish_com_getc(struct goldfish_com_instance *ci)
{
	uint32_t bytes_ready;
	uint8_t c;
	int s;

	s = splserial();

	if (!(bytes_ready = bus_space_read_4(ci->iot, ci->ioh, GOLDFISH_UART_BYTES_READY))) {
		splx(s);
		return -1;
	}

	bus_space_write_4(ci->iot, ci->ioh, GOLDFISH_UART_DATA_PTR, (uint32_t)&c);
	bus_space_write_4(ci->iot, ci->ioh, GOLDFISH_UART_DATA_LEN, 1);
	bus_space_write_4(ci->iot, ci->ioh, GOLDFISH_UART_CMD, GOLDFISH_UART_CMD_READ_BUFFER);

	splx(s);

	return c;
}

static void
cons_putc(dev_t dev, int c)
{
	goldfish_com_putc(&cons_info, c);
}

static int
cons_getc(dev_t dev)
{
	return goldfish_com_getc(&cons_info);
}

static void
cons_pollc(dev_t dev, int on)
{ }

static struct consdev goldfish_consdev = {
	NULL, NULL, cons_getc, cons_putc, cons_pollc, NULL,
	NULL, NULL, NODEV, CN_NORMAL
};

int
goldfish_cnattach(bus_space_tag_t iot, bus_addr_t iobase)
{
	cons_info.iot = iot;
	cons_info.iobase = iobase;

	if (bus_space_map(iot, iobase, GOLDFISH_UART_IO_SIZE, 0, &cons_info.ioh))
		return ENOMEM; /* WTF */

	cn_tab = &goldfish_consdev;

	return 0;
}

void
goldfish_cndetach(void)
{
	bus_space_unmap(cons_info.iot, cons_info.ioh, GOLDFISH_UART_IO_SIZE);

	cons_info.iot = NULL;
	cn_tab = NULL;
}
