extern int printf(char *format, ...);

// ERROR
// int array[] = {0, {1, 2}, 3};

// OK
// int array[][3] = {0, 1, 2};

// OK
// int array[][3] = {{0, 1, 2}, {3, 4, 5}};

// OK
// int array[][3] = {0, 1, 2, {3, 4, 5}};
//

int array[][3] = {0, 1, 2, {3, 4, }, 6, 7, 8};
// ERROR
// int array[][3] = {0, 1, {3, 4, }, 6, 7, 8};

int main()
{
    int i, j;
    for (i = 0; i < 3; i++) { 
        for (j = 0; j < 3; j++)
            printf("[%d]  ", array[i][j]);
        printf("\n");
    }
    return 0;
}
