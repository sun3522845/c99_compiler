extern int a, b, c, d;

void test1()
{
    if (a - b > 1 && a - b < 3)
        c = 123;
}

void test2()
{
    if (a + b)
        c = a + b;
    else
        d = a + b;
}
