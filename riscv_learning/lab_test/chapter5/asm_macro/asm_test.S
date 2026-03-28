
.macro op_func op, a, b
    mv a0, \a
    mv a1, \b
    call op_\()\op
.endm

.align 3
op_1:
    add a0, a0, a1
    ret

op_2:
    sub a0, a0, a1
    ret

.global macro_test_1
macro_test_1:
    addi sp, sp, -16
    sd ra, 8(sp)

    mv t0, a0
    mv t1, a1
    op_func 1, t0, t1

    ld ra, 8(sp)
    addi sp, sp, 16
    ret

.global macro_test_2
macro_test_2:
    addi sp, sp, -16
    sd ra, 8(sp)

    mv t0, a0
    mv t1, a1
    op_func 2, t0, t1

    ld ra, 8(sp)
    addi sp, sp, 16
    ret
