.section .text
.global compare_data
compare_data:
    # a0为入参第一个参数
    # a1为入参第二个参数
    bltu a0, a1, .L1 # 如果a0 < a1，跳转到.L1
    # 若a0 >= a1，则返回a0
    ret
.L1:
    # 若a0 < a1，则将a1赋值给a0，并返回a0
    mv a0, a1
    ret