#ifndef _SYMBOL_H
#define _SYMBOL_H

#include "list.h"
#include "insn.h"

struct type_t;
struct scope_t;
struct decorator_t;
struct declarator_t;
struct function_t;

typedef operand_t symbol_t;

struct scope_t {
    int level;
    int current_offset;
    list_t symbol_table[26];

    scope_t(int level, int offset);
    ~scope_t();
    list_t *hash_function(char *name);
    symbol_t *insert(symbol_t *symbol);
    void insert_declare(decorator_t *decorator, declarator_t *declarator, 
                        int delta);
    void insert_function(function_t *function);
    void insert_integer(char *name, int value);
    symbol_t *lookup(char *name);
    void dump();
};

extern scope_t *current_scope;
extern symbol_t *lookup_symbol(char *name);

extern void enter_scope();
extern void leave_scope();

#endif
