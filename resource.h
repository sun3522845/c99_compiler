#ifndef _RESOURCE_H
#define _RESOURCE_H

struct registor_t;
struct operand_t;

extern void reset_temporary_list();
extern operand_t *allocate_temporary();
extern void free_temporary(operand_t *operand);

struct transient_t {
    int index;
    int in_use;
    int offset;
};

extern int transient_count;
extern void reset_transient_array();
extern transient_t *allocate_transient();
extern void free_transient(transient_t *transient);

enum {
    REGISTER_EAX = 0,
    REGISTER_ECX,
    REGISTER_EDX,
    REGISTER_EBX,
    REGISTER_ESI,
    REGISTER_EDI,
    REGISTER_COUNT,
};

enum {
    CALLER_SAVED,
    CALLEE_SAVED,
};

// register_t is already defined in <sys/types.h>
struct registor_t {
    char *name;
    int index;
    int who_saved;
    int in_use;
    int is_locked;

    void lock()
    {
        is_locked = 1;
    }

    void unlock()
    {
        is_locked = 0;
    }
};

extern void caller_save_register(operand_t *target);
extern void caller_restore_register(operand_t *target);
extern void callee_save_register();
extern void callee_restore_register();

extern registor_t *register_eax;
extern registor_t *register_ecx;
extern registor_t *register_edx;
extern void reset_register_array();
extern registor_t *allocate_register(int who_saved);
extern void free_register(registor_t *registor);

extern registor_t *grab_register();
extern void grab_register(registor_t *registor);
extern registor_t *load_operand(operand_t *operand);
extern registor_t *load_operand_to_target(operand_t *operand);
extern void store_operand(registor_t *registor, operand_t *operand);

#endif
