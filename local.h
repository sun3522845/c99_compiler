#ifndef _LOCAL_H
#define _LOCAL_H
#include "list.h"

struct operand_t;
struct insn_t;

extern void reset_local();
extern void access_variable(insn_t *insn);
extern void access_variable(operand_t *operand);
extern void dump_local_usage();
extern int allocate_register_for_local();
extern void load_argument();
extern void enter_loop();
extern void leave_loop();

#endif
