struct person {
    char name[8];
    int age;
};

struct person return_struct()
{
    struct person p;
    return p;
}

void main()
{
    struct person p;
    
    p = return_struct();
}
