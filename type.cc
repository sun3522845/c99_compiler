#include "root.h"
#include "type.h"
#include "ast.h"
#include "gram.h"
#include "dump.h"
#include "util.h"

int type_t::is_compatible(type_t *left, type_t *right)
{
    return 1;
}

type_t::type_t()
{
    size = 0;
}

type_t::~type_t()
{
}

void_type_t::void_type_t()
{
    rtti = VOID_TYPE_T;
}

void_type_t::~void_type_t()
{
}

void void_type_t::dump()
{
}

integer_type_t::integer_type_t(int is_signed, int size)
{
    rtti = INTEGER_TYPE_T;
    this->is_signed = is_signed;
    this->size = size;
}

integer_type_t::integer_type_t(int type_name)
{
    rtti = INTEGER_TYPE_T;
    switch (type_name) {
        case TOKEN_CHAR:
            size = 1;
            break;
        case TOKEN_SHORT:
            size = 2;
            break;
        case TOKEN_INTEGER:
            size = 4;
            break;
        default:
            assert(0);
    }
}

integer_type_t::~integer_type_t()
{
}

void integer_type_t::dump()
{
    indent("integer");
}

array_type_t::array_type_t(type_t *base_type, int capacity)
{
    rtti = ARRAY_TYPE_T;
    this->base_type = base_type;
    this->capacity = capacity;
    this->size = base_type->size * capacity; 
}

array_type_t::~array_type_t()
{
}

void array_type_t::dump()
{
    push_tag("array");
    base_type->dump();
    push_dump_pop("capacity", capacity);
    pop_tag("array");
}

struct_type_t::struct_type_t()
{
    rtti = STRUCT_TYPE_T;
}

struct_type_t::~struct_type_t()
{
}

void struct_type_t::dump()
{
    push_tag("struct");
    push_dump_pop("size", size);
    push_dump_pop("max_field_size", max_field_size);

    field_t *field;
    for_each (field, field_list) {
        push_tag("field");
        push_dump_pop("name", field->name);
        field->type->dump();;
        push_dump_pop("offset", field->offset);
        pop_tag("field");
    }

    pop_tag("struct");
}

field_t *struct_type_t::add_field(char *name, type_t *type)
{
    field_t *field = new field_t();
    field->name = name;
    field->type = type;
    field->offset = 0;
    field_list.push_back(field);
    return field;
}

field_t *struct_type_t::lookup(char *name)
{
    field_t *field;
    for_each (field, field_list) {
        if (field->name == name)
            return field;
    }
    return NULL;
}

void struct_type_t::compute_max_field_size()
{
    max_field_size = 0;

    field_t *field;
    for_each (field, field_list) {
        int field_size;
        if (field->type->is_struct()) {
            struct_type_t *sub_struct = (struct_type_t*) field->type;
            sub_struct->compute_max_field_size();
            field_size = sub_struct->max_field_size;
        } else if (field->type->is_array()) {
            array_type_t *sub_array = (array_type_t*) field->type;
            field_size = sub_array->base_type->size;
        } else
            field_size = field->type->size;
        if (field_size > max_field_size)
            max_field_size = field_size;
    }
}

static int round_up(int offset, type_t *type)
{
    if (type->is_array())
        return round_up(offset, ((array_type_t*)type)->base_type->size);
    else
        return round_up(offset, type->size);
}

void struct_type_t::compute_offset_and_size(int offset)
{
    field_t *field;
    for_each (field, field_list) {
        if (!field->type->is_struct()) {
            offset = round_up(offset, field->type);
            field->offset = offset;
            offset += field->type->size;
            continue;
        }

        struct_type_t *sub_struct = (struct_type_t*) field->type;
        offset = round_up(offset, sub_struct->max_field_size);
        sub_struct->compute_offset_and_size(offset);
        offset += sub_struct->size;
    }
    size = round_up(offset, max_field_size);
}

// struct {                // offset
//     char a;             // 0
//     struct {             
//         char x;         // 4
//         int y;          // 8
//         char z;         // 12 
//     } b; 
// } data;                 // sizeof == 16 

void struct_type_t::compute_layout()
{
    compute_max_field_size();
    compute_offset_and_size(0);
}

pointer_type_t::pointer_type_t(type_t *base_type)
{
    rtti = POINTER_TYPE_T;
    this->base_type = base_type;
}

pointer_type_t::~pointer_type_t()
{
}

void pointer_type_t::dump()
{
    push_dump_pop("pointer", base_type);
}

function_type_t::function_type_t(type_t *base_type)
{
    rtti = FUNCTION_TYPE_T;
    this->base_type = base_type;
}

function_type_t::~function_type_t()
{
}

void function_type_t::dump()
{
    push_tag("function_type");
    push_dump_pop("return", base_type);

    type_t *type;
    for_each (type, argument_type_list)
        type->dump();
    pop_tag("function_type");
}

named_type_t::named_type_t(char *name)
{
    rtti = NAMED_TYPE_T;
    this->name = name;
}

named_type_t::~named_type_t()
{
}

void named_type_t::dump()
{
}
