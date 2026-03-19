#include "uart.h"

extern void load_store_test(void);
extern void pc_related_test(void);
extern void my_memcpy(void);

void asm_test(void)
{
	load_store_test();
	pc_related_test();
	my_memcpy();
}

void kernel_main(void)
{
	uart_init();
	uart_send_string("Welcome RISC-V!\r\n");

	asm_test();
	uart_send_string("TEST done\r\n");

	while (1) {
		;
	}
}
