#include "root.h"
#include "insn.h"
#include "intern.h"
#include "opcode.h"
#include "resource.h"
#include "util.h"
#include "ast.h"
#include "output.h"
#include "dump.h"
#include "type.h"
#include "optimize.h"
#include "local.h"
#include "compile.h"

int operand_t::is_equal(operand_t *thiz, operand_t *that)
{
    if (thiz->rtti != that->rtti)
        return 0;
    switch (thiz->rtti) {
        case OPERAND_INTEGER:
            return thiz->value == that->value;
        case OPERAND_GLOBAL:
        case OPERAND_LOCAL:
        case OPERAND_TEMPORARY:
            return thiz == that;
        case OPERAND_GLOBAL_ADDRESS:
            return thiz->name == that->name && 
                   thiz->offset == that->offset;
        case OPERAND_INDIRECT:
            return is_equal(thiz->base, that->base) &&
                   is_equal(thiz->index, that->index) &&
                   thiz->scale == that->scale;
        default:
            assert(0);
    }
    return 0;
}

operand_t::operand_t()
{
    rtti = OPERAND_NULL;
    type = NULL;
    address = ADDRESS_DEFAULT;
    garbage0 = 0;
    garbage1 = 0;

    level = 0;
    access_count = 0;
    life_count = 0;
    is_addressed = 0;
    is_const = 0;
    is_volatile = 0;
    is_static = 0;
    is_extern = 0;
}

void operand_t::dump()
{
    push_tag("symbol");
    push_dump_pop("name", name);
    type->dump();
    pop_tag("symbol");
}

void operand_t::print_global_address(char *buffer)
{
    if (offset == 0)
        sprintf(buffer, "$%s", name); 
    else
        sprintf(buffer, "$%s+%d", name, offset);
}

void operand_t::print_local_address(char *buffer)
{
    if (offset > 0)
        sprintf(buffer, "%d(%%ebp)", offset);
    else
        sprintf(buffer, "%d(%%ebp)", offset - callee_saved_register_size);
}

void operand_t::print_indirect(char *buffer)
{
    if (index->is_integer())
        sprintf(buffer, "[%s]", base->print());
    else if (scale == 1)
        sprintf(buffer, "[%s+%s]", base->print(), index->print());
    else
        sprintf(buffer, "[%s+%s*%d]", base->print(), index->print(), scale);
}

char *operand_t::print()
{
    char *buffer = get_static_buffer();
    switch (rtti) {
        case OPERAND_INTEGER:
            sprintf(buffer, "%d", value);
            break;
        case OPERAND_GLOBAL:
            sprintf(buffer, "%s", name);
            break;
        case OPERAND_GLOBAL_ADDRESS:
            print_global_address(buffer);
            break;
        case OPERAND_LOCAL:
            sprintf(buffer, "%s", name);
            break;
        case OPERAND_LABEL:
            sprintf(buffer, ".L%d", value);
            break;
        case OPERAND_TEMPORARY:
            sprintf(buffer, "@t%d", temporary);
            break;
        case OPERAND_INDIRECT:
            print_indirect(buffer);
            break;
        default:
            assert(0);
    }
    return buffer;
}

// transient local callee-saved-register old-ebp return-address argument
//                                       ^  
//                                       ebp
char *operand_t::x86_address()
{
    char *buffer = get_static_buffer();

    if (in_register()) {
        sprintf(buffer, "%s", registor->name);
        return buffer;
    }

    if (in_transient()) {
        int depth = -4;
        depth -= callee_saved_register_size;
        depth -= current_function->max_local_size;
        depth -= transient->index * 4; 
        sprintf(buffer, "%d(%%ebp)", depth);
        return buffer;
    }

    switch (rtti) {
        case OPERAND_INTEGER:
            sprintf(buffer, "$%d", value);
            break;
        case OPERAND_GLOBAL:
            if (offset == 0)
                sprintf(buffer, "%s", name);
            else
                sprintf(buffer, "%s+%d", name, offset);
            break;
        case OPERAND_GLOBAL_ADDRESS:
            print_global_address(buffer);
            break;
        case OPERAND_LOCAL:
            print_local_address(buffer);
            break;
        case OPERAND_LABEL:
            sprintf(buffer, ".L%d", value);
            break;
        default:
            assert(0);
    }
    return buffer;
}

insn_t::insn_t(int opcode)
{
    this->opcode = opcode;
    this->operand_count = 0;
    is_signed = 0;
}

void insn_t::add_operand(operand_t *operand)
{
    assert(operand_count < MAX_OPERAND);
    operand_array[operand_count++] = operand;
}

void insn_t::dump()
{
    if (opcode == OPCODE_LABEL) {
        printf("%s:\n", operand_array[0]->print());
        return;
    }

    printf("\t%s ", opcode_name_array[opcode]);
    for (int i = 0; i < operand_count; i++) {
        operand_t *operand = operand_array[i];
        printf("%s ", operand->print());
    }
    printf("\n");
}

void insn_t::increase_life()
{
    for (int i = 0; i < operand_count; i++)
        operand_array[i]->life_count++;
}

void insn_t::decrease_life()
{
    for (int i = 0; i < operand_count; i++) {
        operand_t *operand = operand_array[i];
        if (--operand->life_count == 0)
            free_temporary(operand);
    }
}

// %o: operand
// %d: immed 
// %L: label
// %S: is_signed
void emit_insn(int opcode, char *format, ...)
{
    char *line = (char*) alloca(strlen(format) + 1);
    strcpy(line, format);
    char *fields[4];
    int fields_count = split_line(line, " ", fields);
    insn_t *insn = new insn_t(opcode);

    va_list ap;
    va_start(ap, format);
    for (int i = 0; i < fields_count; i++) {
        char *field = fields[i];
        assert(field[0] == '%');

        operand_t *operand = NULL;
        switch (field[1]) {
            case 'S':
                insn->is_signed = va_arg(ap, int);
                continue;
            case 'o':
                operand = va_arg(ap, operand_t*);
                break;
            case 'd':
                operand = new operand_t();
                operand->set_integer(NULL, va_arg(ap, int));
                break;
            case 'L':
                operand = new operand_t();
                operand->set_label(NULL, va_arg(ap, int));
                break;
            default:
                assert(0);
        }
        insn->add_operand(operand);
    }
    va_end(ap);

    insn->increase_life();
    access_variable(insn);
    current_function->add_insn(insn);
}

void emit_jump(int label)
{
    emit_insn(OPCODE_JUMP, "%L", label);
}

void emit_label(int label)
{
    emit_insn(OPCODE_LABEL, "%L", label);
}

int allocate_label()
{
    static int label;
    return label++;
}
