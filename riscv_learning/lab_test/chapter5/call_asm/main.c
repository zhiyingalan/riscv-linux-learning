#include <stdio.h>

extern int compare_data(int a, int b); // 声明汇编函数

int main(void)
{
    printf("Hello RISCV!\n");
    int a = 66;
    int b = 20;
    int ret = compare_data(a, b);
    printf("compare_data(%d, %d) and the max data is = %d\n",a,b, ret);
    return 0;
}