#include "root.h"
#include "ast.h"
#include "type.h"
#include "dump.h"

static int level;

void indent()
{
    for (int i = 0; i < level; i++)
        printf("  ");
}

void indent(char *value)
{
    indent();
    printf("%s\n", value);
}

void indent(int value)
{
    indent();
    printf("%d\n", value);
}

void push_tag(const char *tag)
{
    indent();
    printf("<%s>\n", tag);
    level++;
}

void pop_tag(const char *tag)
{
    level--;
    indent();
    printf("</%s>\n", tag);
}

void push_dump_pop(const char *tag, ast_t *ast)
{
    push_tag(tag);
    ast->dump();
    pop_tag(tag);
}

void push_dump_pop(const char *tag, type_t *type)
{
    push_tag(tag);
    type->dump();
    pop_tag(tag);
}

void push_dump_pop(const char *tag, int value)
{
    indent();
    printf("%s = %d\n", tag, value);
}

void push_dump_pop(const char *tag, char *value)
{
    indent();
    printf("%s = %s\n", tag, value);
}
