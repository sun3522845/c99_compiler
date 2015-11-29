// Generated by opcode.py
#ifndef _OPCODE_H
#define _OPCODE_H

enum {
    OPCODE_LABEL,
    OPCODE_OR,
    OPCODE_XOR,
    OPCODE_AND,
    OPCODE_LEFT_SHIFT,
    OPCODE_RIGHT_SHIFT,
    OPCODE_ADD,
    OPCODE_SUBTRACT,
    OPCODE_MULTIPLY,
    OPCODE_DIVIDE,
    OPCODE_MODULAR,
    OPCODE_NEGATIVE,
    OPCODE_COMPLEMENT,
    OPCODE_INCREASE,
    OPCODE_DECREASE,
    OPCODE_ADDRESS,
    OPCODE_LOAD,
    OPCODE_STORE,
    OPCODE_JUMP,
    OPCODE_JUMP_EQUAL,
    OPCODE_JUMP_NOT_EQUAL,
    OPCODE_JUMP_LITTLE,
    OPCODE_JUMP_GREAT,
    OPCODE_JUMP_LITTLE_EQUAL,
    OPCODE_JUMP_GREAT_EQUAL,
    OPCODE_EXTEND,
    OPCODE_SHRINK,
    OPCODE_MOVE,
    OPCODE_COPY,
    OPCODE_PUSH,
    OPCODE_POP,
    OPCODE_CALL,
    OPCODE_RETURN,
    OPCODE_NUMBER,
};

struct insn_t;
typedef void (*insn_handler_t)(insn_t*);
extern insn_handler_t insn_handler_array[];
extern char *opcode_name_array[];

extern void insn_label(insn_t *);
extern void insn_or(insn_t *);
extern void insn_xor(insn_t *);
extern void insn_and(insn_t *);
extern void insn_left_shift(insn_t *);
extern void insn_right_shift(insn_t *);
extern void insn_add(insn_t *);
extern void insn_subtract(insn_t *);
extern void insn_multiply(insn_t *);
extern void insn_divide(insn_t *);
extern void insn_modular(insn_t *);
extern void insn_negative(insn_t *);
extern void insn_complement(insn_t *);
extern void insn_increase(insn_t *);
extern void insn_decrease(insn_t *);
extern void insn_address(insn_t *);
extern void insn_load(insn_t *);
extern void insn_store(insn_t *);
extern void insn_jump(insn_t *);
extern void insn_jump_equal(insn_t *);
extern void insn_jump_not_equal(insn_t *);
extern void insn_jump_little(insn_t *);
extern void insn_jump_great(insn_t *);
extern void insn_jump_little_equal(insn_t *);
extern void insn_jump_great_equal(insn_t *);
extern void insn_extend(insn_t *);
extern void insn_shrink(insn_t *);
extern void insn_move(insn_t *);
extern void insn_copy(insn_t *);
extern void insn_push(insn_t *);
extern void insn_pop(insn_t *);
extern void insn_call(insn_t *);
extern void insn_return(insn_t *);

extern void output_epilogue(char *name, int local_size);
extern void output_prologue();
#endif