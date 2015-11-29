#define insn imull 

#define IMM  $0x87654321
#define GLO  global_variable
#define LOC  8(%ebp)
#define REG  %eax

    .data
    .comm global_variable,4

    .text
    .globl main	
main:
    insn IMM,IMM  # error 
    insn GLO,IMM  # error
    insn LOC,IMM  # error 
    insn REG,IMM  # error 

    insn IMM,REG 
    insn GLO,REG 
    insn LOC,REG 
    insn REG,REG 

    insn IMM,GLO 
    insn GLO,GLO   # error
    insn LOC,GLO   # error
    insn REG,GLO 

    insn IMM,LOC 
    insn GLO,LOC   # error 
    insn LOC,LOC   # error
    insn REG,LOC 

    divl REG
    divl LOC
    divl GLO
    divl IMM
