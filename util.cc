#include "root.h"
#include "util.h"

string_buffer_t::string_buffer_t()
{
    data[0] = 0;
    end = data;
}

void string_buffer_t::append(char *string)
{
    strcpy(end, string);
    end += strlen(string);
    *end = 0;
}

void string_buffer_t::append(char chr)
{
    *end++ = chr;
    *end = 0;
}

void string_buffer_t::printf(char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    int length = vsprintf(end, format, ap);
    end += length;
    *end = 0;
    va_end(ap);
}

int split_line(char *line, const char *seperator, char *fields[])
{
    int fields_count = 0;
    char *word, *word_save;
    word = strtok_r(line, seperator, &word_save);        
    while (word) {
        fields[fields_count++] = word;
        word = strtok_r(NULL, seperator, &word_save);
    }
    return fields_count;
}

char *get_static_buffer()
{
    const int BUFFER_COUNT = 8;
    const int BUFFER_SIZE = 128;
    static char buffer_array[BUFFER_COUNT * BUFFER_SIZE];
    static char *buffer = buffer_array;

    if ((buffer += BUFFER_SIZE) == buffer_array + sizeof(buffer_array))
        buffer = buffer_array;
    return buffer;
}

