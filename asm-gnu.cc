#include "root.h"
#include "insn.h"
#include "asm-gnu.h"
#include "util.h"
#include "intern.h"
#include "ast.h"
#include "resource.h"

// %%: %
// %o: operand
// %r: register
// %d: integer
// %s: string
void emit_x86_insn(char *format, ...)
{
    string_buffer_t string_buffer;
    string_buffer.append('\t');

    va_list ap;
    va_start(ap, format);
    while (*format) {
        if (*format != '%') {
            string_buffer.append(*format);
            format++;
            continue;
        }

        format++;
        operand_t *operand;
        registor_t *registor;
        switch (*format) {
            case '%':
                string_buffer.append('%');
                break;
            case 'o':
                operand = va_arg(ap, operand_t*);
                string_buffer.append(operand->x86_address());
                break;
            case 'r':
                registor = va_arg(ap, registor_t*);
                string_buffer.append(registor->name);
                break;
            case 'd':
                char integer[8];
                sprintf(integer, "%d", va_arg(ap, int));
                string_buffer.append(integer);
                break;
            case 's':
                string_buffer.append(va_arg(ap, char*));
                break;
            default:
                assert(0);
        }
        format++;
    }
    va_end(ap);

    string_buffer.append('\n');
    char *insn = strdup(string_buffer.data);
    current_function->machine_insn_list.push_back(insn);
}

void define_label(operand_t *operand)
{
    string_buffer_t string_buffer;
    string_buffer.printf(".L%d:\n", operand->value);
    char *insn = strdup(string_buffer.data);
    current_function->machine_insn_list.push_back(insn);
}
