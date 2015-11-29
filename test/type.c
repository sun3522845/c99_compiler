void (*array_of_function[3])(int a, int b);
int *array_of_pointer[3];
int (*pointer_to_array)[3];
void signal(int (*handler)(int signo));

//void (array_of_function[3])(int a, int b);
//int (void (*function)(int a, int b))[3];
//
struct person {
    char name[10];
    int age;
    struct unix {
        char author;
        int version;
    } linux;
} tom;

