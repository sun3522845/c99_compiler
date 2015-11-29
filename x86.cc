#include "root.h"
#include "x86.h"
#include "opcode.h"
#include "insn.h"
#include "intern.h"
#include "asm-gnu.h"
#include "output.h"
#include "util.h"
#include "compile.h"
#include "ast.h"

#define FETCH_INSN1                                                        \
    operand_t *source = insn->get_operand(0);                              \

#define FETCH_INSN2                                                        \
    operand_t *source = insn->get_operand(0);                              \
    operand_t *target = insn->get_operand(1);

#define FETCH_INSN3                                                        \
    operand_t *source0 = insn->get_operand(0);                             \
    operand_t *source1 = insn->get_operand(1);                             \
    operand_t *target = insn->get_operand(2);                              

#define SWITCH_OPERAND                                                     \
    if (source1->in_recycle_register()) {                                  \
        operand_t *temp = source1;                                         \
        source1 = source0;                                                 \
        source0 = temp;                                                    \
    }

void insn_label(insn_t *insn)
{
    FETCH_INSN1;
    define_label(source);
}

int insn_binary_delta(insn_t *insn, const char *x86_opcode)
{
    int opcode = insn->opcode;
    FETCH_INSN3;

    operand_t *delta = NULL;
    if (operand_t::is_equal(source0, target))
        delta = source1;
    if (operand_t::is_equal(source1, target))
        delta = source0;
    if (!delta)
        return 0;

    if (delta->in_memory() && target->in_memory()) {
        registor_t *registor = grab_register();
        emit_x86_insn("movl %o,%r", delta, registor);
        emit_x86_insn("%s %r,%o", x86_opcode, registor, target);
        return 1;
    }

    if (delta->is_integer(1) && opcode == OPCODE_ADD)
        emit_x86_insn("incl %o", target);
    else if (delta->is_integer(1) && opcode == OPCODE_SUBTRACT)
        emit_x86_insn("decl %o", target);
    else
        emit_x86_insn("%s %o,%o", x86_opcode, delta, target);
    return 1;
}

// opcode source,target
// addl reg/mem/imm,reg
// addl reg/imm,mem
void insn_binary(insn_t *insn, const char *x86_opcode)
{
    FETCH_INSN3;
    int opcode = insn->opcode;

    if (insn_binary_delta(insn, x86_opcode))
        return;

    // source0 != target && source1 != target
    if (target->in_register()) {
        emit_x86_insn("movl %o,%o", source0, target); 
        emit_x86_insn("%s %o,%o", x86_opcode, source1, target);
        return;
    }

    if (opcode != OPCODE_SUBTRACT)
        SWITCH_OPERAND;
    registor_t *registor = load_operand_to_target(source0);
    emit_x86_insn("%s %o,%r", x86_opcode, source1, registor);
    store_operand(registor, target);
}

void insn_or(insn_t *insn)
{
    insn_binary(insn, "orl");
}

void insn_xor(insn_t *insn)
{
    insn_binary(insn, "xorl");
}

void insn_and(insn_t *insn)
{
    insn_binary(insn, "andl");
}

void insn_left_shift(insn_t *insn)
{
    insn_binary(insn, "lshl");
}

void insn_right_shift(insn_t *insn)
{
    insn_binary(insn, insn->is_signed ? "rshl" : "rarl");
}

void insn_add(insn_t *insn)
{
    insn_binary(insn, "addl");
}

void insn_subtract(insn_t *insn)
{
    insn_binary(insn, "subl");
}

void insn_multiply(operand_t *source0, operand_t *source1, registor_t *target)
{
    if (source1->is_immed()) {
        operand_t *temp;
        temp = source1;
        source1 = source0;
        source0 = temp;
    }
    if (source0->is_immed()) {
        emit_x86_insn("imull %o,%o,%r", source0, source1, target);
        return;
    }
    assert(!source0->is_immed() && !source1->is_immed());

    if (source0->in_register(target)) {
        emit_x86_insn("imull %o,%r", source1, target);
        return;
    }

    if (source1->in_register(target)) {
        emit_x86_insn("imull %o,%r", source0, target);
        return;
    }

    emit_x86_insn("movl %o,%r", source0, target); 
    emit_x86_insn("imull %o,%r", source1, target);
}

// opcode source,target
// imull mem/reg,reg
// imull imm,mem/reg,reg
void insn_multiply(insn_t *insn)
{
    FETCH_INSN3;

    if (target->in_register()) {
        insn_multiply(source0, source1, target->registor);
        return;
    }

    SWITCH_OPERAND;
    registor_t *registor = load_operand_to_target(source0);
    if (source1->is_immed())
        emit_x86_insn("imull %o,%r,%r", source1, registor, registor);
    else
        emit_x86_insn("imull %o,%r", source1, registor);
    store_operand(registor, target);
}

// divl reg/mem32
// EDX:EAX / reg/mem32 = EAX:EDX
void insn_divide(insn_t *insn)
{
    int opcode = insn->opcode;
    FETCH_INSN3;
    const char *x86_opcode = insn->is_signed ? "idivl" : "divl";

    assert(register_eax->in_use <= 1 && register_edx->in_use <= 1);
    if (source0->in_recycle_register(register_edx) ||
        source1->in_recycle_register(register_edx))
        register_edx->in_use++;
    else
        grab_register(register_edx);

    if (source0->in_recycle_register(register_eax))
        register_eax->in_use++;
    else {
        grab_register(register_eax);
        emit_x86_insn("movl %o,%r", source0, register_eax);
    }

    if (source1->is_immed()) {
        grab_register(register_ecx);
        emit_x86_insn("movl %o,%r", source1, register_ecx);
    }

    if (insn->is_signed)
        emit_x86_insn("cltd");
    else
        emit_x86_insn("xor %%edx,%%edx");

    if (source1->is_immed())
        emit_x86_insn("%s %%ecx", x86_opcode);
    else
        emit_x86_insn("%s %o", x86_opcode, source1);

    if (opcode == OPCODE_DIVIDE) {
        store_operand(register_eax, target);
        free_register(register_edx);
    } else {
        store_operand(register_edx, target);
        free_register(register_eax);
    }

    if (source1->is_immed())
        free_register(register_ecx);
}

void insn_modular(insn_t *insn)
{
    insn_divide(insn);
}

// notl mem/reg
void insn_unary(insn_t *insn, char *x86_code)
{
    FETCH_INSN2;
    assert(!source->is_immed());
    
    if (operand_t::is_equal(source, target)) {
        emit_x86_insn("%s %o", x86_code, source);
        return;
    }

    if (target->in_register()) {
        emit_x86_insn("movl %o,%o", source, target);
        emit_x86_insn("%s %o", x86_code, target);
        return;
    }

    registor_t *registor = load_operand_to_target(source);
    emit_x86_insn("%s %r", x86_code, registor);
    store_operand(registor, target);
}

void insn_negative(insn_t *insn)
{
    insn_unary(insn, "negl");
}

void insn_complement(insn_t *insn)
{
    insn_unary(insn, "notl");
}

// register = source
// source = source + 1
void insn_delta(insn_t *insn, char *x86_opcode)
{
    FETCH_INSN2;

    registor_t *registor = grab_register();
    emit_x86_insn("movl %o,%r", source, registor);
    store_operand(registor, target);
    emit_x86_insn("%s %o", x86_opcode, source);
}

void insn_increase(insn_t *insn)
{
    insn_delta(insn, "incl");
}

void insn_decrease(insn_t *insn)
{
    insn_delta(insn, "decl");
}

// leal mem,reg
void insn_address(insn_t *insn)
{
    FETCH_INSN2;
    assert(!source->in_register());
    if (target->in_register()) {
        emit_x86_insn("leal %o,%o", source, target);
        return;
    }

    registor_t *registor = grab_register();
    emit_x86_insn("leal %o,%r", source, registor);
    store_operand(registor, target);
}

void print_indirect_address(registor_t *base, registor_t *index, 
                            int scale, int offset, string_buffer_t *buffer)
{
    if (offset)
        buffer->printf("%d", offset);
    buffer->append("(");
    buffer->printf("%s", base->name);
    if (index)
        buffer->printf(",%s", index->name);
    if (scale > 1)
        buffer->printf(",%d", scale);
    buffer->append(")");
}

// offset(base, index, sacle)
void insn_indirect_move(int opcode, operand_t *indirect, operand_t *direct)
{
    registor_t *base, *index = NULL;

    base = load_operand(indirect->base);
    base->lock();
    if (indirect->index) {
        index = load_operand(indirect->index);
        index->lock();
    }
    int scale = indirect->scale;
    int offset = indirect->offset;

    string_buffer_t buffer;
    print_indirect_address(base, index, scale, offset, &buffer);

    if (direct->in_register()) {
        if (opcode == OPCODE_LOAD)      // indirect -> direct
            emit_x86_insn("movl %s,%o", buffer.data, direct);
        else                            // direct -> indirect
            emit_x86_insn("movl %o,%s", direct, buffer.data);
    } else {
        if (opcode == OPCODE_LOAD) {    // indirect -> direct
            registor_t *registor = grab_register();
            emit_x86_insn("movl %s,%r", buffer.data, registor);
            store_operand(registor, direct);
        } else {                        // direct -> indirect
            registor_t *registor = load_operand_to_target(direct);
            emit_x86_insn("movl %r,%s", registor, buffer.data);
            free_register(registor);
        }
    }

    base->unlock();
    free_register(base);
    if (index) {
        index->unlock();
        free_register(index);
    }
}

void insn_load(insn_t *insn)
{
    FETCH_INSN2;
    assert(source->is_indirect());
    insn_indirect_move(OPCODE_LOAD, source, target);
}

void insn_store(insn_t *insn)
{
    FETCH_INSN2;
    assert(target->is_indirect());
    insn_indirect_move(OPCODE_STORE, target, source);
}

void insn_jump(insn_t *insn)
{
    operand_t *source = insn->get_operand(0);
    emit_x86_insn("jmp %o", source);
}

// OPCODE_COMPARE source0 source1 target
// cmpl source1,source0
// cmpl imm/mem/reg,reg
// cmpl imm/reg,mem
void insn_compare(insn_t *insn, char *x86_opcode)
{
    FETCH_INSN3;

    if (source0->is_immed() || (source0->in_memory() && source1->in_memory())) {
        registor_t *registor = load_operand_to_target(source0);
        emit_x86_insn("cmpl %o,%r", source1, registor);
        free_register(registor);
    } else 
        emit_x86_insn("cmpl %o,%o", source1, source0);
    emit_x86_insn("%s %o", x86_opcode, target);
}

void insn_jump_equal(insn_t *insn)
{
    insn_compare(insn, "je");
}

void insn_jump_not_equal(insn_t *insn)
{
    insn_compare(insn, "jne");
}

void insn_jump_little(insn_t *insn)
{
    insn_compare(insn, "jl");
}

void insn_jump_great(insn_t *insn)
{
    insn_compare(insn, "jg");
}

void insn_jump_little_equal(insn_t *insn)
{
    insn_compare(insn, "jle");
}

void insn_jump_great_equal(insn_t *insn)
{
    insn_compare(insn, "jge");
}

void insn_extend(insn_t *insn)
{
}

void insn_shrink(insn_t *insn)
{
}

void insn_move(insn_t *insn)
{
    FETCH_INSN2;
    if (source->in_memory() && target->in_memory()) {
        registor_t *registor = grab_register();
        emit_x86_insn("movl %o,%r", source, registor);
        emit_x86_insn("movl %r,%o", registor, target);
        free_register(registor);
    } else
        emit_x86_insn("movl %o,%o", source, target);
}

void insn_copy(insn_t *insn)
{
}

void insn_push(insn_t *insn)
{
    operand_t *source = insn->get_operand(0);
    emit_x86_insn("pushl %o", source);
}

void insn_pop(insn_t *insn)
{
    FETCH_INSN1;
    emit_x86_insn("addl %o,%%esp", source);
}

// call function, 
// call function, result
void insn_call(insn_t *insn)
{
    operand_t *source = insn->get_operand(0);
    operand_t *target = NULL;
    if (insn->operand_count == 2) 
        target = insn->get_operand(1);

    caller_save_register(target);
    emit_x86_insn("call %o", source);
    if (target)
        target->write_register(register_eax);
    caller_restore_register(target);
}

void insn_return(insn_t *insn)
{
    if (insn->operand_count == 1) {
        operand_t *source = insn->get_operand(0);
        if (!source->in_register(register_eax))
            emit_x86_insn("movl %o,%%eax", source);
    }
    emit_x86_insn("jmp .L%d", return_label);
}

void output_epilogue(char *name, int max_stack_depth)
{
    output("\t.global %s\n", name);
    output("%s:\n", name);
    output("\tpushl %%ebp\n");
    output("\tmovl %%esp,%%ebp\n");
    if (max_stack_depth > 0)
        output("\tsubl $%d,%%esp\n", max_stack_depth);
}

void output_prologue()
{
    output("\tmovl %%ebp,%%esp\n");
    output("\tpopl %%ebp\n");
    output("\tret\n");
}
