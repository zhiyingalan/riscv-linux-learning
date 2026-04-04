#include <stdio.h>

extern int macro_test_1(int a, int b); // 声明汇编函数
extern int macro_test_2(int a, int b); // 声明汇编函数

#define MY_OP(op, asm_op) \
static inline int my_##op(int i, int j) \
{                                           \
    int res = 0;                            \
    __asm__ __volatile__ (                   \
         #asm_op" %[result], %[input1], %[input2]" \
        : [result]"=r"(res)                    \
        : [input1]"r"(i), [input2]"r"(j)        \
    );                                          \
    return res;                             \
}                                           \

MY_OP(add, add)
MY_OP(sub, sub)

/**
 * 主函数：演示基本的宏定义和函数调用
 * @return int 程序正常退出时返回0
 */
int main(void)
{
    printf("Hello RISCV!\n");
    // 定义并初始化两个整型变量a和b
    int a = 66;
    int b = 20;
    // 调用宏定义macro_test_1，传入a和b的值，并打印结果
    int ret = macro_test_1(a, b);
    printf("macro_test_1(%d, %d) = %d\n",a,b, ret);
    // 调用宏定义macro_test_2，传入a和b的值，并打印结果
    ret = macro_test_2(a, b);
    printf("macro_test_2(%d, %d) = %d\n",a,b, ret);

    // 修改变量a的值
    a = 55;
    // 调用自定义函数my_add，传入a和b的值，并打印结果
    ret = my_add(a, b);
    printf("my_add(%d, %d) = %d\n",a,b, ret);
    // 调用自定义函数my_sub，传入a和b的值，并打印结果
    ret = my_sub(a, b);
    printf("my_sub(%d, %d) = %d\n",a,b, ret);
    // 程序正常退出
    return 0;
}