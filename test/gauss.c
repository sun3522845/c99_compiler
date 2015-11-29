int printf(char *format, ...);

int main(int argc)
{
    int i, sum;

    for (i = 1; i < 101; i++)
        sum = sum + i;
    printf("sum = %d\n", sum);
}
