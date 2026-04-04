# Makefile for RISC-V memcpy implementation

# RISC-V交叉编译器
CC = gcc

# 编译选项
CFLAGS = -Wall

# 目标文件
TARGET = my_memcpy

# 源文件
SRCS = my_memcpy.c

# 默认目标 
all: $(TARGET)

# 编译规则
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# 运行规则（直接执行）
run:
	./$(TARGET)

# 清理规则
clean:
	rm -f $(TARGET)

# 伪目标
.PHONY: all run clean