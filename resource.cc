#include "root.h"
#include "resource.h"
#include "insn.h"
#include "intern.h"
#include "asm-gnu.h"
#include "list.h"
#include "output.h"

list_t temporary_list;

registor_t register_array[REGISTER_COUNT] = {
    {"%eax", REGISTER_EAX, CALLER_SAVED},
    {"%ecx", REGISTER_ECX, CALLER_SAVED},
    {"%edx", REGISTER_EDX, CALLER_SAVED},
    {"%ebx", REGISTER_EBX, CALLEE_SAVED},
    {"%esi", REGISTER_ESI, CALLEE_SAVED},
    {"%edi", REGISTER_EDI, CALLEE_SAVED},
};

registor_t *register_eax = register_array + REGISTER_EAX;
registor_t *register_ecx = register_array + REGISTER_ECX;
registor_t *register_edx = register_array + REGISTER_EDX;

#define MAX_TRANSIENT_COUNT 32
transient_t transient_array[MAX_TRANSIENT_COUNT];
int transient_count;

void reset_temporary_list()
{
     temporary_list.clear();
}

operand_t *allocate_temporary()
{
    operand_t *operand = new operand_t();
    int index = temporary_list.count();
    operand->set_temporary(index);
    temporary_list.push_back(operand);
    return operand;
}

void free_temporary(operand_t *operand)
{
    if (!operand->is_temporary())
        return;
    if (operand->in_register())
        free_register(operand->registor);
    else if (operand->in_transient())
        free_transient(operand->transient);
    operand->address = ADDRESS_DEFAULT;
}

void reset_register_array()
{
    for (int index = 0; index < REGISTER_COUNT; index++) {
        registor_t *registor = register_array + index;
        registor->name = intern(registor->name);
        registor->in_use = 0;
        assert(registor->is_locked == 0);
    }
}

registor_t *allocate_register(int who_saved)
{
    for (int index = 0; index < REGISTER_COUNT; index++) {
        registor_t *registor = register_array + index;
        if (registor->in_use == 0 && registor->who_saved == who_saved) {
            registor->in_use = 1;
            return registor;
        }
    }
    return NULL;
}

void free_register(registor_t *registor)
{
    int index = registor->index;
    assert(index < REGISTER_COUNT);
    assert(registor->in_use > 0);
    registor->in_use--;
}

void reset_transient_array()
{
    transient_count = 0;
}

transient_t *allocate_transient()
{
    int index;
    for (index = 0; index < transient_count; index++)
        if (transient_array[index].in_use == 0)
            break;
    if (index == transient_count) {
        transient_count++;
        assert(transient_count < MAX_TRANSIENT_COUNT);
    }
    transient_t *transient = transient_array + index;
    transient->in_use = 1;
    transient->index = index;
    return transient;
}

void free_transient(transient_t *transient)
{
    int index = transient->index;
    assert(index < transient_count);
    transient->in_use = 0;
}

// Add new reference to EAX
void caller_save_register(operand_t *target)
{
    for (int i = 0; i < REGISTER_COUNT; i++) {
        registor_t *registor = register_array + i;
        if (registor == register_eax && target) {
            grab_register(register_eax);
            continue;
        }
        if (registor->who_saved == CALLER_SAVED && registor->in_use)
            emit_x86_insn("pushl %s", registor->name);
    }
}

void caller_restore_register(operand_t *target)
{
    // in reverse order
    for (int i = REGISTER_COUNT - 1; i >= 0; i--) {
        registor_t *registor = register_array + i;
        if (registor == register_eax && target)
            continue;
        if (registor->who_saved == CALLER_SAVED && registor->in_use)
            emit_x86_insn("popl %s", registor->name);
    }
}

void callee_save_register()
{
    for (int i = 0; i < REGISTER_COUNT; i++) {
        registor_t *registor = register_array + i;
        if (registor->who_saved == CALLEE_SAVED && registor->in_use)
            emit_x86_insn("pushl %s", registor->name);
    }
}

void callee_restore_register()
{
    // in reverse order
    for (int i = REGISTER_COUNT - 1; i >= 0; i--) {
        registor_t *registor = register_array + i;
        if (registor->who_saved == CALLEE_SAVED && registor->in_use)
            emit_x86_insn("popl %s", registor->name);
    }
}

registor_t *spill_register(operand_t *grabbed)
{
    registor_t *registor = grabbed->registor;
    assert(registor->in_use == 1 && registor->is_locked == 0);
    transient_t *transient = allocate_transient();
    grabbed->write_transient(transient);
    emit_x86_insn("movl %r,%o", registor, grabbed);
    return registor;
}

// Return new reference
registor_t *grab_register()
{
    registor_t *registor = allocate_register(CALLER_SAVED);
    if (registor)
        return registor;

    operand_t *temporary;
    for_each (temporary, temporary_list) {
        if (!temporary->in_register())
            continue;
        registor = temporary->registor;
        if (registor->in_use == 1 && registor->is_locked == 0) 
            return spill_register(temporary);
    }
    assert(0);
}

// Return new reference
void grab_register(registor_t *registor)
{
    if (registor->in_use == 0) {
        registor->in_use = 1;
        return;
    }

    operand_t *temporary;
    for_each (temporary, temporary_list) {
        if (temporary->in_register(registor)) {
            spill_register(temporary);
            return;
        }
    }
    assert(0);
}

// Return new reference, returned register can't be modified
registor_t *load_operand(operand_t *operand)
{
    registor_t *registor;
    if (operand->in_register()) {
        registor = operand->registor;
        registor->in_use++;
        return registor;
    }
    registor = grab_register();
    emit_x86_insn("movl %o,%r", operand, registor);
    return registor;
}

// Return new reference, returned register may be modified
registor_t *load_operand_to_target(operand_t *source)
{
    registor_t *registor;
    if (source->in_recycle_register()) {
        registor = source->registor;
        registor->in_use++;
        return registor;
    }
    registor = grab_register();
    emit_x86_insn("movl %o,%r", source, registor);
    return registor;
}

// Consume reference
void store_operand(registor_t *registor, operand_t *operand)
{
    if (operand->is_temporary()) {
        assert(operand->address == ADDRESS_DEFAULT);
        operand->write_register(registor);
    } else {
        emit_x86_insn("movl %r,%o", registor, operand); 
        free_register(registor);
    }
}
