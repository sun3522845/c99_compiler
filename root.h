#ifndef _ROOT_H
#define _ROOT_H

#include <sys/types.h>
#include <assert.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#define offset_of offsetof

#define trace(...)                                              \
    do {                                                        \
        fprintf(stdout, "%s:%d:", __FILE__, __LINE__);          \
        fprintf(stdout, __VA_ARGS__);                           \
    } while (0)

extern int trace_flag;

template<class type> extern type min(type a, type b) { return a < b ? a : b; }
template<class type> extern type max(type a, type b) { return a > b ? a : b; }
#endif
