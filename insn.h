#ifndef _INSN_H
#define _INSN_H

#include "list.h"
enum {
    OPERAND_NULL = 123,
    OPERAND_INTEGER,
    OPERAND_GLOBAL,
    OPERAND_GLOBAL_ADDRESS,
    OPERAND_LOCAL,
    OPERAND_TEMPORARY,
    OPERAND_INDIRECT,
    OPERAND_LABEL,
};

enum {
    ADDRESS_DEFAULT,
    ADDRESS_REGISTER,
    ADDRESS_TRANSIENT,
};

struct transient_t;
struct registor_t;
struct type_t;

struct operand_t {
    int rtti;
    type_t *type;

    int level;
    int access_count;
    int life_count;
    char is_addressed;
    char is_const;
    char is_volatile;
    char is_static;
    char is_extern;

    char *name;
    union {
        int offset;
        int value;
        int temporary;
        int garbage0;
    };
    operand_t *base;
    operand_t *index;
    int scale;

    int address;
    union {
        registor_t *registor;
        transient_t *transient;
        long garbage1;
    };

    static int is_equal(operand_t *thiz, operand_t *that);
    operand_t();
    void dump();

    int is_null()
    {
        return rtti == OPERAND_NULL;
    }

    int is_integer()
    {
        return rtti == OPERAND_INTEGER;
    }

    int is_integer(int value)
    {
        return rtti == OPERAND_INTEGER && this->value == value;
    }

    int is_global()
    {
        return rtti == OPERAND_GLOBAL;
    }

    int is_global_address()
    {
        return rtti == OPERAND_GLOBAL_ADDRESS;
    }

    int is_immed()
    {
        return is_integer() || is_global_address();
    }

    int is_local()
    {
        return rtti == OPERAND_LOCAL;
    }

    int is_temporary()
    {
        return rtti == OPERAND_TEMPORARY;
    }

    int is_indirect()
    {
        return rtti == OPERAND_INDIRECT;
    }

    int is_direct()
    {
        return rtti != OPERAND_INDIRECT;
    }

    int is_label()
    {
        return rtti == OPERAND_LABEL;
    }

    void set_null()
    {
        rtti = OPERAND_NULL;
    }

    void set_integer(char *name, int value)
    {
        rtti = OPERAND_INTEGER;
        this->name = name;
        this->value = value;
    }

    void set_integer(int value)
    {
        rtti = OPERAND_INTEGER;
        this->name = NULL;
        this->value = value;
    }

    void set_global(char *name, int offset = 0)
    {
        rtti = OPERAND_GLOBAL;
        this->name = name;
        this->offset = offset;
    }

    void set_global_address(char *name, int offset = 0)
    {
        rtti = OPERAND_GLOBAL_ADDRESS;
        this->name = name;
        this->offset = offset;
    }

    void set_local(char *name, int offset)
    {
        rtti = OPERAND_LOCAL;
        this->name = name;
        this->offset = offset;
    }

    void set_temporary(int temporary)
    {
        rtti = OPERAND_TEMPORARY;
        this->temporary = temporary;
    }

    void set_label(char *name, int value)
    {
        rtti = OPERAND_LABEL;
        this->name = name;
        this->value = value;
    }

    // scale must be power of two
    void set_indirect(operand_t *base, operand_t *index, int scale = 1, int offset = 0)
    {
        rtti = OPERAND_INDIRECT;
        this->base = base;
        this->index = index;
        this->scale = scale;
        this->offset = offset;
    }

    void set_indirect(operand_t *base, int offset = 0)
    {
        rtti = OPERAND_INDIRECT;
        this->base = base;
        this->index = NULL;
        this->scale = 0;
        this->offset = offset;
    }

    int in_register()
    {
        return address == ADDRESS_REGISTER;
    }

    int in_register(registor_t *registor)
    {
        return address == ADDRESS_REGISTER && this->registor == registor;
    }

    int in_transient()
    {
        return address == ADDRESS_TRANSIENT;
    }

    int in_memory()
    {
        return !is_immed() && !in_register();
    }

    int in_recycle_register()
    {
        return is_temporary() && in_register() && life_count == 1;
    }

    int in_recycle_register(registor_t *registor)
    {
        return is_temporary() && in_register(registor) && life_count == 1;
    }

    void write_register(registor_t *registor)
    {
        address = ADDRESS_REGISTER;
        this->registor = registor;
    }

    void write_transient(transient_t *transient)
    {
        address = ADDRESS_TRANSIENT;
        this->transient = transient;
    }

    void print_global_address(char *buffer);
    void print_local_address(char *buffer);
    void print_indirect(char *buffer);
    char *print();
    char *x86_address();
};

#define MAX_OPERAND 4
// OPCODE SOURCE0 SOURCE1 TARGET
struct insn_t {
    int opcode;
    int operand_count;
    operand_t *operand_array[4]; 
    int is_signed;

    insn_t(int opcode);

    operand_t *get_operand(int index)
    {
        assert(index < operand_count);
        return operand_array[index];
    }

    operand_t *get_last_operand()
    {
        return operand_array[operand_count - 1];
    }

    void change_last_operand(operand_t *operand)
    {
        operand_array[operand_count - 1] = operand;
    }

    void add_operand(operand_t *operand);
    void dump();
    void increase_life();
    void decrease_life();
};

extern void emit_insn(int opcode, char *format, ...);
extern void emit_jump(int label);
extern void emit_label(int label);
extern int allocate_label();

#endif
