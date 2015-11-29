#include "root.h"
#include "list.h"
#include "intern.h"
#include <ctype.h>
#include "util.h"

static list_t string_table[26 * 26];

static int convert(char c)
{
    int result = tolower(c) - 'a';
    return (result < 0 || result > 26) ? 0 : result;
}

static int hash_function(char *string)
{
    assert(string);
    int length = strlen(string);

    int head = 0;
    if (length > 0) 
        head = convert(string[0]);

    int tail = 0;
    if (length > 1) 
        tail = convert(string[length - 1]);

    return head * 26 + tail;
}

char *intern(char *target)
{
    list_t *string_list = string_table + hash_function(target);

    char *string;
    for_each (string, *string_list) {
        if (strcmp(string, target) == 0)
            return string;
    }

    string = strdup(target);
    string_list->push_front(string);
    return string;
}
