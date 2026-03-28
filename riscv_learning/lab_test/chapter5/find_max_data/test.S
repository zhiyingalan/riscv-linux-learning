 # 数据段
.section .data
    .align 2 # 对齐到4字节边界
    .global array # 声明全局变量
array: # 定义一个整数数组，包含20个元素
    .word 1, 201, 3, 41, 51, 16, 27, 38, 9, 10
    .word 22, 12, 13, 14, 23, 16, 57, 18, 209, 20
my_data: # 定义一个整数
    .word 100
my_data2:
    .word 50

# 定义格式化打印字符串
print_data:
    .string "data: %d+%d=%d\n"
print_max_data:
    .string "max data: %d\n"
print_exit:
    .string "exit\n"

# 代码段
.align 3 # 对齐到8字节边界
.section .text
# 定义入口函数
.global _start
_start:
    # 申请栈空间
    addi sp, sp, -16
    # 保存返回地址
    sd ra, 8(sp)

    # 加载my_data和my_data2数据到寄存器a1和a2
    ld a1, my_data
    ld a2, my_data2
    add a3, a1, a2

    # 加载print_data地址到a0寄存器，a0寄存器保存着要打印的字符串的地址
    la a0, print_data
    # 调用printf函数
    call printf

    # 调用find_max_data函数查找数组中的最大值
    call find_max_data
    # 加载print_max_data地址到a0寄存器
    # a1寄存器保存着find_max_data函数返回的最大值
    la a0, print_max_data
    call printf

    la a0, print_exit
    call printf

    # 设置返回值
    li a0, 0
    # 恢复返回地址
    ld ra, 8(sp)
    # 恢复栈指针
    addi sp, sp, 16

    # 退出程序
    li a7, 93
    ecall

find_max_data:
    la a0, array
    lw a1, 0(a0)
    li a2, 19
    addi a0, a0, 4

compare_next:
    beqz a2, done # 如果a2为0，跳转到done标签
    lw t0, 0(a0)
    bltu a1, t0, update_max # 如果a1小于t0，跳转到update_max标签
    addi a0, a0, 4
    addi a2, a2, -1
    j compare_next

update_max:
    mv a1, t0
    addi a0, a0, 4
    addi a2, a2, -1
    j compare_next
    
done:
    ret
