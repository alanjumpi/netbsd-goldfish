#ifndef _GOLDFISHVAR_H_
#define _GOLDFISHVAR_H_

#define GOLDFISH_IO_VBASE		0xff000000
#define GOLDFISH_IO_HWBASE		0xff000000

#define GOLDFISH_PIC_HWBASE		0xff000000
#define GOLDFISH_UART0_HWBASE		0xff002000

/* UART registers */
#define	GOLDFISH_UART_PUT_CHAR		0x00 /* W: Write single 8-bit value to the serial port */
#define	GOLDFISH_UART_BYTES_READY	0x04 /* R: read the number of available buffered input bytes */
#define	GOLDFISH_UART_CMD		0x08 /* W: send command */
#define	GOLDFISH_UART_DATA_PTR		0x10 /* W: write kernel buffer address */
#define	GOLDFISH_UART_DATA_LEN		0x14 /* W: write kernel buffer size */

#define GOLDFISH_UART_IO_SIZE		0x18

#define GOLDFISH_UART_CMD_INT_DISABLE	0x00 /* disable device */
#define GOLDFISH_UART_CMD_INT_ENABLE	0x01 /* enable device */
#define GOLDFISH_UART_CMD_WRITE_BUFFER	0x02 /* write buffer from kernel to device */
#define GOLDFISH_UART_CMD_READ_BUFFER	0x03 /* read buffer from device to kernel */

/* PIC registers */
#define GOLDFISH_PIC_STATUS		0x00 /* R: read the number of pending interrupts (0 to 32) */
#define GOLDFISH_PIC_NUMBER		0x04 /* R: read the lowest pending interrupt index, or 0 if none */
#define	GOLDFISH_PIC_DISABLE_ALL	0x08 /* W: clear all pending interrupts (does not disable them!) */
#define GOLDFISH_PIC_DISABLE		0x0c /* W: disable a given interrupt, value must be in [0..31] */
#define GOLDFISH_PIC_ENABLE		0x10 /* W: enable a given interrupt, value must be in [0..31] */

#define GOLDFISH_PIC_IO_SIZE		0x14

struct goldfish_softc {
	bus_space_tag_t iot;		/* bus tag */
	bus_space_handle_t pic_ioh;	/* PIC handle */
};

struct goldfish_attach_args {
	bus_space_tag_t iot;		/* bus space tag for io */
};

void goldfish_intr_init(void);
void *goldfish_intr_establish(int, int, int(*)(void *), void *);
void goldfish_intr_disestablish(void *);

#endif /* _GOLDFISHVAR_H_ */
