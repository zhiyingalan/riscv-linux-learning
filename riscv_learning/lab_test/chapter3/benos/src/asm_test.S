.align 3
.globl my_test_data
my_test_data:
	.dword 0x12345678abcdabcd

#define MY_OFFSET -2048

.align 3
.global load_store_test
load_store_test:
	li t0, 0x80200000

	lb t1, (t0)
	lb t1, 4(t0)
	lb t1, -4(t0)
	ld t1, (t0)
	lb t1, 4(t0)

	lui t0, 0x80200
	lui t1, 0x40200

	la t0, my_test_data
	lla t1, my_test_data

	ret

.global pc_related_test
pc_related_test:
	auipc t0, 1
	addi t0, t0, MY_OFFSET
	ld t1, MY_OFFSET(t0)
	ret

.global my_memcpy
my_memcpy:
	li t0, 0x80200000
	li t1, 0x123456789ABCDEF0
	sd t1, 0(t0)
	li t1, 0xFFDC432187654321
	sd t1, 8(t0)
	li t1, 0x4444555566667777
	sd t1, 16(t0)
	li t1, 0x8888666644442222
	sd t1, 24(t0)

	li t1, 0x80210000
	li t2, 0
	li t3, 4 # loop number

loop_copy:
	ld t4, 0(t0)
	sd t4, 0(t1)

	addi t0, t0, 8
	addi t1, t1, 8
	addi t3, t3, -1

	bnez t3, loop_copy

	ret