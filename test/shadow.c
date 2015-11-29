int main(int argc, int argv)
{
    int i;
    i = 123;
    {
        i = 456;
        int i;
        i = 789;
    }
    {
        i = 456;
        int i;
        i = 789;
    }
    return 0;
}
