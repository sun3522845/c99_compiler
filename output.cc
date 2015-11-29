#include "root.h"
#include "output.h"

FILE *asm_file = stdout;

void output_open(char *source_file_name)
{
}

void output_close()
{
    fclose(asm_file);
}

void output(char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(asm_file, format, ap);
    va_end(ap);
}
