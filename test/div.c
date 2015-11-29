int a, b, c, d, e;
void test1()
{
    a = b / c / d / e;
}

void test2()
{
    a = b % c % d % e;
}

void test3()
{
    a = (b + c) / (d + e);
}

void test4()
{
    a = (b + c) % (d + e);
}

void signed_div()
{
    int a, b, c;
    a = b / c;
}

void unsigend_div()
{
    unsigned int a, b, c;
    a = b / c;
}

void signed_mul()
{
    int a, b, c;
    a = b * c;
}

void unsigned_mul()
{
    unsigned int a, b, c;
    a = b * c;
}
