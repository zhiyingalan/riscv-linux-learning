# 在riscv64上编译运行

all: call_asm

call_asm: main.c asm_test.S
	gcc -g -o call_asm main.c asm_test.S

clean:
	rm -f call_asm

run:
	./call_asm