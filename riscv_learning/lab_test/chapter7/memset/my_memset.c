#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void *my_memset(void *dest, const char data, size_t n) {
    if(n == 0 || dest == NULL) {
        return dest;
    }
    void *dest_ori = dest; // 保存原始目标地址以便返回
    asm volatile (
        "1:             \n"
        "sb %2, 0(%0)      \n"  // 存储到目标地址
        "addi %0, %0, 1 \n"  // 目标地址+1
        "addi %1, %1, -1 \n"  // 计数器-1
        "bnez %1, 1b    \n"  // 不为0则继续循环
        : "+r"(dest), "+r"(n)
        : "r"(data)
        :"memory"
    );
    return dest_ori;
}

void *my_memset_16byte(void *dest, const long long data, size_t n) {
    if(n == 0 || dest == NULL) {
        return dest;
    }
    size_t dest_ori = (size_t)dest; // 保存原始目标地址以便返回
    size_t word_size = 16; // 每个word为16字节
    size_t word_cnt = n / word_size;
    size_t byte_cnt = n % word_size;
    printf("word_cnt=%lu, byte_cnt=%lu, dest=0x%p, data=0x%llx\n", word_cnt, byte_cnt, dest, data);
    if(word_cnt>0) {
        // 采用汇编符合编写内嵌汇编
        __asm__ __volatile__(
            "1:             \n"
            "sd %[data], 0(%[dest])   \n"  // 从源地址加载一个word
            "sd %[data], 8(%[dest])   \n"  // 存储到目标地址
            "addi %[dest], %[dest], 16 \n"  // 目标地址+16
            "addi %[cnt], %[cnt], -1 \n" // 计数器
            "bnez %[cnt], 1b    \n"  // 不为0则继续循环
            : [dest]"+r"(dest), [cnt]"+r"(word_cnt)
            : [data]"r"(data)
            : "memory"
        );
    }
    // 处理剩余的字节
    printf("after word loop, dest=0x%p\n", dest);
    if(byte_cnt > 0) {
        my_memset(dest, (char)data, byte_cnt);
    }

    return (void *)dest_ori;
}


int main() {
    char src[] = "Hello, RISC-V memcpy!";
    char dest[50];


    // 打印源字符串和目标数组的地址
    printf("&src=0x%p, &dest=0x%p\n", (void*)&src, (void*)&dest);
    // 打印源字符串的内容和大小
    printf("Source: %s sizeof(src)=%lu\n", src, sizeof(src));
    // 使用my_memset函数将源字符串清零（注释掉）
    // my_memset(src, 0, sizeof(src));
    // 使用my_memset_16byte函数填充源字符串，填充值为0x3041424344450023
    // 其中0x30是字符'0'的ASCII码，0x41-0x45是'A'-'E'的ASCII码, 0x23是字符'#'的ASCII码
    my_memset_16byte(src, 0x3041424344450023, sizeof(src));
    // 打印填充后的源字符串内容
    // 更新注释：由于填充后的内容包含非ASCII字符，直接打印可能会导致乱码。我们可以逐字节打印源字符串的内容和对应的ASCII码。
    printf("Source after memset: ");
    for(size_t i=0; i<sizeof(src); i++) {
        if(src[i] == 0) {
            printf("\\0");
        } else {
            printf("%c", src[i]);
        }
    }
    printf("\n");

    return 0;
}