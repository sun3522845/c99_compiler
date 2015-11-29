#include "root.h"
#include "optimize.h"
#include "insn.h"
#include "opcode.h"
#include "main.h"
#include "ast.h"
#include "resource.h"
#include "asm-gnu.h"

int is_common_express(insn_t *insn, int opcode, int operand_count, 
                      operand_t *operand_array[])
{
    if (opcode != insn->opcode)
        return 0;
    assert(operand_count == insn->operand_count - 1);
    for (int i = 0; i < operand_count; i++)
        if (!operand_t::is_equal(operand_array[i], insn->operand_array[i]))
            return 0; 
    return 1;
}

static int may_eliminate(int opcode)
{
    return opcode == OPCODE_ADDRESS ||
           opcode == OPCODE_MOVE ||
           opcode >= OPCODE_OR && opcode <= OPCODE_COMPLEMENT;
}

int may_modify(insn_t *insn, int operand_count, operand_t *operand_array[])
{
    if (!may_eliminate(insn->opcode))
        return 0;
    operand_t *target = insn->get_last_operand();
    for (int i = 0; i < operand_count; i++)
        if (operand_t::is_equal(operand_array[i], target))
            return 1;
    return 0;
}

operand_t *find_common_express(int opcode, ...)
{
    if (!option_eliminate_common_express || !may_eliminate(opcode))
        return NULL;

    operand_t *operand_array[4];
    int operand_count = 0;
    va_list ap;
    va_start(ap, opcode);
    operand_t *operand;
    while (operand = va_arg(ap, operand_t*)) {
        assert(operand_count < 4);
        operand_array[operand_count++] = operand;
    }
    va_end(ap);

    list_t *block = current_function->current_block;
    if (block == NULL)
        return NULL;
    insn_t *insn;
    for_each_reverse (insn, *block) {
        if (is_common_express(insn, opcode, operand_count, operand_array))
            return insn->get_last_operand();
        if (may_modify(insn, operand_count, operand_array))
            return NULL;
    }
    return NULL;
}

static int merge(insn_t *this_insn, insn_t *next_insn)
{
    if (next_insn->opcode != OPCODE_MOVE ||
        !may_eliminate(this_insn->opcode))
        return 0;

    operand_t *this_target = this_insn->get_last_operand();
    operand_t *next_source = next_insn->get_operand(0);
    operand_t *next_target = next_insn->get_operand(1);

    if (this_target->is_temporary() && 
        this_target->life_count == 2 &&
        operand_t::is_equal(this_target, next_source)) {
        this_insn->change_last_operand(next_target);
        return 1;
    }
    return 0;
}

void eliminate_temporary(list_t *block)
{
    list_node_t *this_node = block->dummy.next;
    list_node_t *end_node = &block->dummy;
    if (this_node == end_node)
        return;

    list_node_t *next_node = this_node;
    while (next_node != end_node) {
        insn_t *this_insn = (insn_t*) this_node->item;
        insn_t *next_insn = (insn_t*) next_node->item;
        if (merge(this_insn, next_insn)) {
            next_node->unlink();
            delete next_node;
            next_node = this_node->next;
        } else {
            this_node = next_node;
            next_node = next_node->next;
        }
    }
}

void strength_reduce(list_t *block)
{
}

void optimize()
{
    list_t *block;
    list_t *block_list = &current_function->block_list;

    if (option_eliminate_temporary) {
        for_each (block, *block_list)
            eliminate_temporary(block);
    }

    if (option_strength_reduce) {
        for_each (block, *block_list)
            strength_reduce(block);
    }
}

