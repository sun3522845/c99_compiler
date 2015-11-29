#include "root.h"
#include "symbol.h"
#include "dump.h"
#include "type.h"
#include "ast.h"
#include "main.h"
#include "compile.h"
#include "util.h"
#include <ctype.h>

#define MAX_SCOPE_LEVEL 16 
scope_t *scope_stack[MAX_SCOPE_LEVEL];
int scope_stack_size;
scope_t *current_scope;

scope_t::scope_t(int level, int current_offset)
{
    this->level = level;
    this->current_offset = current_offset;
}

scope_t::~scope_t()
{
}

list_t *scope_t::hash_function(char *name)
{
    return symbol_table + (tolower(name[0]) - 'a');
}

symbol_t *scope_t::insert(symbol_t *new_symbol)
{
    symbol_t *old_symbol = lookup(new_symbol->name);
    if (old_symbol)
        return NULL;
    list_t *symbol_list = hash_function(new_symbol->name);
    symbol_list->push_back(new_symbol);
    return new_symbol;
}

symbol_t *scope_t::lookup(char *name)
{
    list_t *symbol_list = hash_function(name);
    symbol_t *symbol;
    for_each (symbol, *symbol_list) {
        if (symbol->name == name)
            return symbol;
    }
    return NULL;
}

void scope_t::dump()
{
    push_tag("scope");

    for (int i = 0; i < 26; i++) {
        list_t *symbol_list = symbol_table + i;
        symbol_t *symbol;
        for_each (symbol, *symbol_list)
            symbol->dump();
    }

    pop_tag("scope");
}

symbol_t *lookup_symbol(char *name)
{
    symbol_t *symbol;
    for (int i = scope_stack_size - 1; i >= 0; i--) {
        scope_t *scope = scope_stack[i];
        if (symbol = scope->lookup(name))
            return symbol;
    }
    return NULL;
}

void scope_t::insert_declare(decorator_t *decorator, declarator_t *declarator, 
                             int delta)
{
    symbol_t *symbol = new symbol_t();
    symbol->rtti = (level == 0) ? OPERAND_GLOBAL : OPERAND_LOCAL;
    symbol->name = declarator->get_name();
    symbol->type = declarator->deduce_type(decorator);
    int symbol_size = round_up(symbol->type->size, 4);

    if (level == 0) {
        insert(symbol);
        return;
    }

    switch (delta) {
        case +1:
            symbol->offset = current_offset;
            current_offset += symbol_size;
            break;
        case -1:
            current_offset -= symbol_size;
            symbol->offset = current_offset;
            if (!current_function)
                break;
            if (-current_offset > current_function->max_local_size)
                current_function->max_local_size = -current_offset;
            break;
        default:
            assert(0);
    }
    insert(symbol);
}

void scope_t::insert_function(function_t *function)
{
    insert_declare(function->decorator, function->declarator, +1);
}

void scope_t::insert_integer(char *name, int value)
{
    type_t *type = new integer_type_t(1, CPU_INT_SIZE);
    symbol_t *symbol = new symbol_t();
    symbol->type = type;
    symbol->set_integer(name, value);
    insert(symbol);
}

void enter_scope()
{
    assert(scope_stack_size < MAX_SCOPE_LEVEL);
    int level, current_offset;
    switch (scope_stack_size) {
        // global scope
        case 0:     
            level = 0;
            current_offset = 0;
            break;
        
        // function scope
        case 1:     
            level = 1;
            current_offset = 0; 
            break;

        // block scope
        default:    
            level = current_scope->level + 1;
            current_offset = current_scope->current_offset;
    }
    scope_stack[scope_stack_size++] = new scope_t(level, current_offset);
    current_scope = scope_stack[scope_stack_size - 1];
}

void leave_scope()
{
    if (option_dump_symbol) {
        printf("Scope level %d:\n", current_scope->level);
        current_scope->dump();
    }

    assert(scope_stack_size > 0);
    delete current_scope;
    --scope_stack_size;
    if (scope_stack_size == 0)
        current_scope = NULL;
    else
        current_scope = scope_stack[scope_stack_size - 1];
}
