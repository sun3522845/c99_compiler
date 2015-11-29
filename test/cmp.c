#include <stdio.h>
void signed_jmp()
{
    int a, b, c;
    if (a < b)
        c = 123;
    else
        c = 456;
}

void unsigned_jmp()
{
    unsigned int a, b, c;
    if (a < b)
        c = 123;
    else
        c = 456;
}

// signed compare with unsigned, signed is treat as unsigned  
void confuse()
{
    int a;
    unsigned int b;
    int c;
    if (a < b)
        c = 123;
    else
        c = 456;
}

int main()
{
    int a = -1;
    unsigned int b = 1;

    if (a < b)
        printf("%d < %u\n", a, b);
    else
        printf("%d > %u\n", a, b);
    return 0;
}
