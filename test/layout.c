#include <stdio.h>

struct {
    char a;             // 0
    struct {             
        char x;         // 4
        int y;          // 8
        char z;         // 12 
    } b; 
} data;                 // sizeof == 16 

#define offset(field) ((unsigned long)&data.field) - ((unsigned long)&data)

int main()
{
    printf("%lu\n", offset(a));
    printf("%lu\n", offset(b.x));
    printf("%lu\n", offset(b.y));
    printf("%lu\n", offset(b.z));
    printf("sizeof=%d\n", sizeof(data));
    return 0;
}
