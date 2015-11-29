#include <stdio.h>

int array[2][3] = {0, 1, 2, 3, 4, 5};

int *pointer;

int main()
{
    int *pointer = array[1];
    printf("%d\n", pointer[1]);
    return 0;
}
