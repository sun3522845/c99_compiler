#ifndef _UTIL_H
#define _UTIL_H

#include "list.h"
struct string_buffer_t {
    char data[256];
    char *end;

    string_buffer_t();
    void append(char *string);
    void append(char chr);
    void printf(char *format, ...);
};

extern int split_line(char *line, const char *seperater, char *fields[]);
extern char *get_static_buffer();

// size must be power of two 
extern inline int round_up(int offset, int size)
{
    return (offset + size - 1) & ~(size - 1);
}

#endif
