#define IMM  $0x87654321
#define GLO  global_variable
#define LOC  8(%ebp)
#define REG  %eax

    .data
    .comm global_variable,4

    .text
    .globl main	
main:
	movl (%eax,%ebx), %eax
