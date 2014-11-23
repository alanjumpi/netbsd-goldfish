#ifndef _GOLDFISH_UART_H_
#define _GOLDFISH_UART_H_

int goldfish_cnattach(bus_space_tag_t iot, bus_addr_t iobase);
void goldfish_cndetach(void);

#endif /* _GOLDFISH_UART_H_ */
