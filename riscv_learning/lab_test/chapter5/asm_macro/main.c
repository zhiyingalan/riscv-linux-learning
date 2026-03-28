#include <stdio.h>

extern int macro_test_1(int a, int b); // 声明汇编函数
extern int macro_test_2(int a, int b); // 声明汇编函数

int main(void)
{
    printf("Hello RISCV!\n");
    int a = 66;
    int b = 20;
    int ret = macro_test_1(a, b);
    printf("macro_test_1(%d, %d) = %d\n",a,b, ret);
    ret = macro_test_2(a, b);
    printf("macro_test_2(%d, %d) = %d\n",a,b, ret);
    return 0;
}