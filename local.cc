#include "root.h"
#include "local.h"
#include "insn.h"
#include "asm-gnu.h"
#include "resource.h"
#include "main.h"

static list_t operand_list;
static list_t register_operand_list;
static int factor;

void reset_local()
{
    operand_list.clear();
    register_operand_list.clear();
    factor = 1;
}

void access_variable(operand_t *operand)
{
    if (!operand->is_local())
        return;
    if (operand->access_count == 0)
        operand_list.push_back(operand);
    operand->access_count += factor;
}

void access_variable(insn_t *insn)
{
    for (int i = 0; i < insn->operand_count; i++)
        access_variable(insn->get_operand(i));
}

static int compare(const void *a, const void *b)
{
    operand_t **x = (operand_t**)a;
    operand_t **y = (operand_t**)b;
    return (*y)->access_count - (*x)->access_count;
}

int allocate_register_for_local()
{
    if (!option_register_variable)
        return 0;
    int operand_count = operand_list.count();
    operand_t *operand_array[operand_count];
    operand_list.to_array((void**)operand_array);
    qsort(operand_array, operand_count, sizeof(operand_t*), compare);

    int register_count = 0;
    for (int i = 0; i < operand_count; i++) {
        operand_t *operand = operand_array[i];
        if (operand->access_count < 4)
            break;
        registor_t *registor = ::allocate_register(CALLEE_SAVED);
        if (registor) {
            register_count++;
            operand->write_register(registor);
            register_operand_list.push_back(operand);
        }
    }
    return register_count;
}

void load_argument()
{
    operand_t *operand;
    for_each (operand, register_operand_list) {
        if (operand->offset > 0)
            emit_x86_insn("movl %d(%%ebp),%r", 
                          operand->offset, operand->registor);
    }
}

void dump_local_usage()
{
    printf("name\taccess_count\n");
    operand_t *operand;
    for_each (operand, operand_list)
        printf("%s\t%d\n", operand->name, operand->access_count);
}

void enter_loop()
{
    factor *= 4;
}

void leave_loop()
{
    factor /= 4;
}
