#ifndef _DUMP_H
#define _DUMP_H

struct ast_t;
struct type_t;
extern void indent();
extern void indent(char *value);
extern void indent(int value);
extern void push_tag(const char *tag);
extern void pop_tag(const char *tag);
extern void push_dump_pop(const char *tag, ast_t *ast);
extern void push_dump_pop(const char *tag, type_t *type);
extern void push_dump_pop(const char *tag, int value);
extern void push_dump_pop(const char *tag, char *value);

#endif
