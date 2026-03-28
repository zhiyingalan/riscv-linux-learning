
# 在riscv64上编译运行
all: test

test: test.o
	ld a.out -o test -lc --dynamic-linker /lib/ld-linux-riscv64-lp64d.so.1
test.o: test.S
	as test.S -o a.out

clean:
	rm -f a.out test

run:
	./test