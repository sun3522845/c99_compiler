int printf(char *format, ...);

int fact(int number)
{
    if (number == 0)
        return 1;
    return number * fact(number - 1);
}

int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

int main()
{
    int value;
    int i;
    value = fact(6);
    printf("max = %d\n", value);
    return 0;
}
