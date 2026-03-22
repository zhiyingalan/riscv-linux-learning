#include "uart.h"

extern void load_store_test(void);
extern void pc_related_test(void);
extern void my_memcpy(void);
extern int compare_test(int a, int b);

void asm_test(void)
{
	int ret = 0;
	load_store_test();
	pc_related_test();
	my_memcpy();
	ret = compare_test(1, 2);
	if(ret==0) {
		uart_send_string("compare_test(1, 2) OK!\r\n");
	} else {
		uart_send_string("compare_test(1, 2) FAIL!\r\n");
	}
	ret = compare_test(2, 1);
	if(ret==0) {
		uart_send_string("compare_test(2, 1) OK!\r\n");
	} else {
		uart_send_string("compare_test(2, 1) FAIL!\r\n");
	}
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
