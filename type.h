#ifndef _TYPE_H
#define _TYPE_H

#include "list.h"
#include "x86.h"

struct ast_list_t;

enum {
    VOID_TYPE_T,
    INTEGER_TYPE_T,
    ARRAY_TYPE_T,
    STRUCT_TYPE_T,
    POINTER_TYPE_T,
    FUNCTION_TYPE_T,
    NAMED_TYPE_T,
};

struct type_t {
    int rtti;
    int size;

    static int is_compatible(type_t *left, type_t *right);
    type_t();
    virtual ~type_t();
    virtual void dump() = 0;

    int is_array()
    {
        return rtti == ARRAY_TYPE_T;
    }

    int is_struct()
    {
        return rtti == STRUCT_TYPE_T;
    }

    int is_function()
    {
        return rtti == FUNCTION_TYPE_T;
    }

    int is_integer()
    {
        return rtti == INTEGER_TYPE_T;
    }

    int is_void()
    {
        return rtti == VOID_TYPE_T;
    }

    int is_pointer()
    {
        return rtti == POINTER_TYPE_T;
    }
};

struct void_type_t: type_t {
    void_type_t();
    ~void_type_t();
    void dump();
};

struct atom_type_t;
struct integer_type_t: type_t {
    int is_signed;

    integer_type_t(int is_signed, int size);
    integer_type_t(int type_name);
    ~integer_type_t();
    void dump();
};

struct array_type_t: type_t {
    type_t *base_type;
    int capacity;

    array_type_t(type_t *base_type, int capacity);
    ~array_type_t();
    void dump();
};

struct field_t {
    char *name;
    type_t *type;
    int offset;
};

struct struct_type_t: type_t {
    list_t field_list;
    int max_field_size;

    struct_type_t();
    ~struct_type_t();
    void dump();
    field_t *add_field(char *name, type_t *type);
    field_t *lookup(char *name);
    void compute_max_field_size();
    void compute_offset_and_size(int offset);
    void compute_layout();
};

struct pointer_type_t: type_t {
    type_t *base_type;

    pointer_type_t(type_t *base_type);
    ~pointer_type_t();
    void dump();
};

struct function_type_t: type_t {
    int with_ellipsis;
    type_t *base_type;
    list_t argument_type_list;

    function_type_t(type_t *base_type);
    ~function_type_t();
    void dump();
};

struct named_type_t: type_t {
    char *name;
    type_t *base_type;

    named_type_t(char *name);
    ~named_type_t();
    void dump();
};

#endif
