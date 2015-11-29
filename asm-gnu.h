#ifndef _ASM_GNU_H
#define _ASM_GNU_H

#define X86_OR      "orl %0, %1"
#define X86_XOR     "xorl %0, %1"
#define X86_AND     "andl %0, %1"
#define X86_LSH     "lshl %0, %1"
#define X86_RSH     "rshl %0, %1"
#define X86_ADD     "addl %0, %1"
#define X86_SUB     "subl %0, %1"
#define X86_MUL     "mull %0, %1"
#define X86_DIV     "divl %0, %1"
#define X86_MOD     "modl %0, %1"
#define X86_NEG     "negl %0, %1"
#define X86_NOT     "notl %0, %1"
#define X86_INC     "incl %0, %1"
#define X86_DEC     "decl %0, %1"
#define X86_ADD     "addl %0, %1"
#define X86_JMP     "jmpl %0, %1"
#define X86_JE      "je %0, %1"
#define X86_JNE     "jne %0, %1"
#define X86_JL      "jl %0, %1"
#define X86_JG      "jg %0, %1"
#define X86_JLE     "jle %0, %1"
#define X86_JGE     "jge %0, %1"
#define X86_MOVSB   "movsbl %0, %1"
#define X86_MOVSW   "movswl %0, %1"
#define X86_MOVZB   "movzbl %0, %1"
#define X86_MOVZW   "movzwl %0, %1"
#define X86_MOVB    "movb %0, %1"
#define X86_MOVW    "movw %0, %1"
#define X86_MOV     "movl %0, %1"
#define X86_PUSH    "pushl %0, %1"
#define X86_POP     "popl %0, %1"
#define X86_CALL    "call %0, %1"

extern void emit_x86_insn(char *format, ...);
extern void define_label(operand_t *label);
#endif
