int a, b, c, d, e, f, g, x;
void spill()
{
    x = a++ + (b++ + (c++ + (d++ + (e++ + (f++ + g++)))));
}

void spill_short()
{
    x = a++ + (b++ + (c++ + d++));
}

