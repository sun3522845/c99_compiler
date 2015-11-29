// OK
// struct person;
// typedef struct person forward;

struct person {
    char name;
    int age;
}; 

// OK
// int person;

// ERROR
// person p;

// OK
// struct person p;

// OK
// struct person person;

// ERROR
// typedef struct person person;
// int person;

// ERROR
// typedef struct person person;
// person person;

// typedef struct person person;
// void function()
// {
//     person person; // OK
//     (person *)0;   // ERROR
// }
