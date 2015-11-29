#include "root.h"
#include "ast.h"
#include "compile.h"
#include "main.h"

int trace_flag;
extern FILE *yyin;
extern int yydebug;
extern int yyparse();

void usage()
{
    puts("c99 [options] file\n");
    puts("   --help\n");
    puts("   --dump-ast\n");
    puts("   --dump-symbol\n");
    puts("   --dump-insn\n");
}

int option_dump_ast;
int option_dump_symbol;
int option_dump_insn;
int option_const_fold;
int option_strength_reduce;
int option_eliminate_temporary;
int option_eliminate_common_express;
int option_eliminate_jump;
int option_register_variable;
int option_switch_table;

#define is_option(option) strcmp(argv[i], option) == 0

int main(int argc, char *argv[])
{
    int i;
    for (i = 1; i < argc; i++) {
        if (is_option("--help")) {
            usage();
            return 0;
        }
        if (is_option("--dump-ast")) {
            option_dump_ast = 1;
            continue;
        }
        if (is_option("--dump-symbol")) {
            option_dump_symbol = 1;
            continue;
        }
        if (is_option("--dump-insn")) {
            option_dump_insn = 1;
            continue;
        }
        if (is_option("-O")) {
            option_const_fold = 1;
            option_strength_reduce = 1;
            option_eliminate_common_express = 1;
            option_eliminate_temporary = 1;
            option_eliminate_jump = 1;
            option_register_variable = 1;
            option_switch_table = 1;
            continue;
        }
        break;
    }

    if (i == argc - 1) {
        yyin = fopen(argv[i], "r");
        if (!yyin) {
            fprintf(stderr, "c99: no input files\n");
            exit(0);
        }
    }

    yydebug = 0;
    yyparse();
    if (option_dump_ast) {
        program->dump();
        return 0;
    }
    compile_program();
    output_program();
    return 0;
}
