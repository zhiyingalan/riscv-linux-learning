#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void *my_memcpy(void *dest, const void *src, size_t n) {
    // void *dest_ori = dest; // 保存原始目标地址以便返回
    __asm__ __volatile__(
        "1:             \n"
        "lb t0, 0(%1)   \n"  // 从源地址加载一个word
        "sb t0, 0(%0)   \n"  // 存储到目标地址
        "addi %0, %0, 1 \n"  // 目标地址+1
        "addi %1, %1, 1 \n"  // 源地址+1
        "addi %2, %2, -1 \n" // 计数器
        "bnez %2, 1b    \n"  // 不为0则继续循环
        : "=r"(dest), "=r"(src), "=r"(n)
        : "0"(dest), "1"(src), "2"(n)
        : "t0", "memory"
    );
    return dest;
}

void *my_memcpy_optimized(void *dest, const void *src, size_t n) {
    size_t dest_ori = (size_t)dest; // 保存原始目标地址以便返回
    size_t word_size = 8; // 每个word为8字节
    size_t word_cnt = n / word_size;
    size_t byte_cnt = n % word_size;
    // 采用汇编符合编写内嵌汇编
    __asm__ __volatile__(
        "1:             \n"
        "ld t0, 0(%[src])   \n"  // 从源地址加载一个word
        "sd t0, 0(%[dest])   \n"  // 存储到目标地址
        "addi %[src], %[src], 8 \n"  // 目标地址+8
        "addi %[dest], %[dest], 8 \n"  // 源地址+8
        "addi %[cnt], %[cnt], -1 \n" // 计数器
        "bnez %[cnt], 1b    \n"  // 不为0则继续循环
        : [dest]"+r"(dest), [src]"+r"(src), [cnt]"+r"(word_cnt)
        : 
        : "t0", "memory"
    );
    // 处理剩余的字节
    if(byte_cnt > 0) {
        my_memcpy(dest, src, byte_cnt);
    }

    return (void *)dest_ori;
}

// 测试函数
void test_memcpy_performance() {
    const size_t SIZE = 1024 * 1024 * 100; // 100MB 数据
    char *src = malloc(SIZE);
    char *dest1 = malloc(SIZE);
    char *dest2 = malloc(SIZE);
    struct timespec start, end;
    double elapsed1, elapsed2;

    // 检查内存分配
    if (src == NULL || dest1 == NULL || dest2 == NULL) {
        printf("Error: Memory allocation failed\n");
        if (src) free(src);
        if (dest1) free(dest1);
        if (dest2) free(dest2);
        return;
    }
    printf("src=0x%p, dest1=0x%p, dest2=0x%p\n", (void*)src, (void*)dest1, (void*)dest2);

    // 初始化源数据
    for (size_t i = 0; i < SIZE; i++) {
        src[i] = i % 256;
    }
    // 测试 my_memcpy
    clock_gettime(CLOCK_MONOTONIC, &start);
    my_memcpy(dest1, src, SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed1 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("my_memcpy: %.6f seconds\n", elapsed1);

    printf("src=0x%p, dest1=0x%p, dest2=0x%p\n", (void*)src, (void*)dest1, (void*)dest2);

    // 测试 my_memcpy_optimized
    clock_gettime(CLOCK_MONOTONIC, &start);
    my_memcpy_optimized(dest2, src, SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed2 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("my_memcpy_optimized: %.6f seconds\n", elapsed2);

    // 验证复制正确性
    for (size_t i = 0; i < SIZE; i++) {
        if (dest1[i] != src[i] || dest2[i] != src[i]) {
            printf("Error: Data mismatch at index %zu\n", i);
            break;
        }
    }

    free(src);
    free(dest1);
    free(dest2);
}



int main() {
    char src[] = "Hello, RISC-V memcpy!";
    char dest[50];
    char dest2[50];


    printf("&src=0x%p, &dest=0x%p\n", (void*)&src, (void*)&dest);
    printf("Source: %s sizeof(src)=%lu\n", src, sizeof(src));
    my_memcpy(dest, src, sizeof(src));
    my_memcpy_optimized(dest2, src, sizeof(src));


    printf("Testing memcpy performance...\n");
    test_memcpy_performance();

    return 0;
}