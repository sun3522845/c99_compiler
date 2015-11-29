	.file	"common-ugly.c"
	.text
.globl test1
	.type	test1, @function
test1:
	pushl	%ebp
	movl	%esp, %ebp
	movl	a, %eax
	subl	b, %eax
	cmpl	$2, %eax
	jne	.L3
	movl	$123, c
.L3:
	popl	%ebp
	ret
	.size	test1, .-test1
.globl test2
	.type	test2, @function
test2:
	pushl	%ebp
	movl	%esp, %ebp
	movl	b, %eax
	addl	a, %eax
	je	.L5
	movl	%eax, c
	jmp	.L7
.L5:
	movl	$0, d
.L7:
	popl	%ebp
	ret
	.size	test2, .-test2
	.ident	"GCC: (Ubuntu 4.3.2-1ubuntu12) 4.3.2"
	.section	.note.GNU-stack,"",@progbits
