#ifndef _STACK_H
#define _STACK_H

struct stack_t {
    enum { MAX_COUNT = 32 };
    int array[MAX_COUNT];
    int count;

    stack_t()
    {
        count = 0;
    }

    int get_top()
    {
        assert(count > 0);
        return array[count - 1];
    }

    void push(int item)
    {
        assert(count < MAX_COUNT);
        array[count++] = item;
    }

    int pop()
    {
        assert(count > 0);
        return array[--count];
    }
};

#endif
