[TOC]

# 代码示例

# SBI（M模式）

sbi_boot.S

```assembly
.section ".text.boot"


.globl _start
_start:
/* 关闭M模式的中断*/
csrw mie, zero

/* 设置栈, 栈的大小为4KB */
la sp, stacks_start
li t0, 4096
add sp, sp, t0
/* 
   把M模式的SP设置到mscratch寄存器，
   下次陷入到M模式可以获取SP
*/
csrw mscratch, sp

/* 跳转到C语言 */
tail sbi_main
```

sbi_entry.S
```assembly
/*
   sbi_exception_vector 
   M模式的异常向量入口
   8字节对齐
 */
.align 3
.global sbi_exception_vector
sbi_exception_vector:
	/* 从mscratch获取M模式的sp，把S模式的SP保存到mscratch*/
	csrrw sp, mscratch, sp

	addi sp, sp, -(PT_SIZE)

	sd x1,  PT_RA(sp)
	sd x3,  PT_GP(sp)
	...保存寄存器值到M模式下的栈窗中
	sd x31, PT_T6(sp)

	/*保存mepc*/
	csrr t0, mepc
	sd t0, PT_MEPC(sp)

	/*保存mstatus*/
	csrr t0, mstatus
	sd t0, PT_MSTATUS(sp)

	/*
	   这里有两个目的:
	   1. 保存S模式的SP保存到 sbi_trap_regs->sp
	   2. 把M模式的SP保存到mscratch, 以便下次陷入到M模式时候可以得到SP
	 */
	addi t0, sp, PT_SIZE /* 此时的SP为M模式的SP, mscratch保存的是S模式的SP */
	/* 把M模式的SP保存到mscratch，把S模式的SP保存到 栈框sbi_trap_regs->sp里*/
	csrrw   t0, mscratch, t0
	sd t0, PT_SP(sp)

	/* 调用C语言的sbi_trap_handler */
	mv a0, sp /* sbi_trap_regs */
	call sbi_trap_handler

	/* save context*/
	ld t0, PT_MSTATUS(sp)
	csrw mstatus, t0

	ld t0, PT_MEPC(sp)
	csrw mepc, t0

	ld x1,  PT_RA(sp)
	ld x3,  PT_GP(sp)
	...从M模式下的栈窗中恢复出寄存器值
	ld x31, PT_T6(sp)

	ld sp,  PT_SP(sp)
	mret
```



sbi_trap.c
```c
static int sbi_ecall_handle(unsigned int id, struct sbi_trap_regs *regs)
{
	int ret = 0;

	switch (id) {
	case SBI_CONSOLE_PUTCHAR:
		putchar(regs->a0);
		ret = 0;
		break;
	case SBI_CONSOLE_GETCHAR:
		regs->a0 = uart_get();
		ret = 0;
		break;
	}

	/* 系统调用返回的是系统调用指令（例如ECALL指令）的下一条指令 */
	if (!ret)
		regs->mepc += 4;

	return ret;
}

void sbi_trap_handler(struct sbi_trap_regs *regs)
{
	unsigned long mcause = read_csr(mcause);
	unsigned long ecall_id = regs->a7;
	int rc = SBI_ENOTSUPP;
	const char *msg = "trap handler failed";

	switch (mcause) {
	case CAUSE_SUPERVISOR_ECALL:
		rc = sbi_ecall_handle(ecall_id, regs);
		msg = "ecall handler failed";
		break;
	default:
		break;
	}

	if (rc) {
		sbi_trap_error(regs, msg, rc);
	}
}

extern void sbi_exception_vector(void);

void sbi_trap_init(void)
{
	/* 设置异常向量表地址 */
	write_csr(mtvec, sbi_exception_vector);
	/* 关闭所有中断 */
	write_csr(mie, 0);
}
```

sbi_main.c

```c
void sbi_main(void)
{
    unsigned long val;

    sbi_trap_init();
    /* 设置异常向量表地址 */
    write_csr(mtvec, sbi_exception_vector);
    /* 关闭所有中断 */
    write_csr(mie, 0);

    /* 设置跳转模式为S模式 */
    val = read_csr(mstatus);
    val = INSERT_FIELD(val, MSTATUS_MPP, PRV_S);
    val = INSERT_FIELD(val, MSTATUS_MPIE, 0);
    write_csr(mstatus, val);

    /* 设置M模式的Exception Program Counter，用于mret跳转 */
    write_csr(mepc, FW_JUMP_ADDR);
    /* 设置S模式异常向量表入口*/
    write_csr(stvec, FW_JUMP_ADDR);
    /* 关闭S模式的中断*/
    write_csr(sie, 0);
    /* 关闭S模式的页表转换 */
    write_csr(satp, 0);

    /* 切换到S模式 */
    asm volatile("mret");
}
```

## mepc

mepc: 记录执行mret时跳转的地址。

## mret

mret: 高特权级->低特权级。目标特权级由mstatus.MPP位决定

以上例子执行mret时：

1. PC = mepc（跳内核）
2. 权限：M → S
3. 内核开始运行

## mtvec
mtvec：执行ecall时跳转的目标地址

# Kernal（S模式）

```c
static inline char sbi_getchar(void)
{
 return ({ 
     register unsigned long a0 asm ("a0") = (unsigned long)(0); 
     register unsigned long a1 asm ("a1") = (unsigned long)(0); 
     register unsigned long a2 asm ("a2") = (unsigned long)(0); 
     register unsigned long a7 asm ("a7") = (unsigned long)(0x2); 
     asm volatile ("ecall" : "+r" (a0) : "r" (a1), "r" (a2), "r" (a7) : "memory"); a0; }
     );
}
```

## ecall

ecall：低特权级->高特权级。跳转规则如下：

| 指令    | 行为          | 权限由谁决定               | 关键寄存器     |
| :------ | :------------ | :------------------------- | :------------- |
| `ecall` | 陷入（低→高） | **当前特权级**（硬件固定） | 无（SPP 无效） |
| `sret`  | 返回（高→低） | **sstatus.SPP**            | SPP            |
| `mret`  | 返回（高→低） | **mstatus.MPP**            | MPP            |

以上例子执行ecall时(没有配置委托)：

1. 当前PC写入mepc，保存当前特权级到mstatus.MPP（硬件自动完成）
2. 跳转到SBI的mtvec目标地址，权限S → M
3. SBI完成异常处理（ **保存寄存器至栈窗，执行sbi_trap_handler，从栈窗恢复寄存器，mepc+=4[避免返回后循环执行ecall]，执行mret返回执行ecall的下一条指令** ）
4. 退出异常，直接执行ecall的下一条指令

**结合上述代码，执行ecall时：**

1. 会跳转到M模式下的mtvec中的地址（write_csr(mtvec, sbi_exception_vector);）即异常向量表**sbi_exception_vector**函数入口，此函数在开始会保存寄存器到栈窗，离开此函数时会将寄存器值从栈窗中恢复出来
2. 异常向量表函数会调用（call sbi_trap_handler）**sbi_trap_handler()**，在此函数中会根据异常原因寄存器**mcause**中的（Exception Code，EC）来执行相应的异常处理函数，此例中为**sbi_ecall_handle()**
3. 执行完异常处理函数后，会恢复寄存器值，同时由于执行的是ecall系统调用，所以mepc中所保持的PC值应+4更新为S模式下执行ecall后的下一条指令
4. 执行mret，跳转到更新后mepc中的地址
5. 完成异常处理，继续执行S模式下的程序



## 区别mret和ecall

区别mret和ecall:

1. mret不是陷入（trap），不是异常（exception）,只是返回指令。

  只做两件事：
	a.把PC设置为mepc
	b.把特权级降低（目标特权级由mstatus.MPP决定）
  切换到低特权级后，就正常往下执行，没有新的ecall/trap/exception，就不会放回到原高特权级模式

2. ecall是陷入（trap）正常陷入后流程处理完后一定会返回到原特权级。除非陷入后的流程panic，死循环等无法退出。



