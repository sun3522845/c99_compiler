#ifndef _OPTIMIZE_H
#define _OPTIMIZE_H

struct operand_t;
extern operand_t *find_common_express(int opcode, ...);
extern void optimize();

#endif
