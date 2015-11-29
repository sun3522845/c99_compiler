#ifndef _LIST_H
#define _LIST_H

#define host_of(clazz, member, pointer)                                      \
    ((clazz*) ((ulong)pointer - offset_of(clazz, member)))

#define __(item)  __list_node_##item

#define for_each(i, list)                                                    \
    for (list_node_t *__(i) = (list).dummy.next;                             \
         __(i) == &(list).dummy ?  0 : (i = (typeof(i))__(i)->item, 1);      \
         __(i) = __(i)->next)

#define for_each_reverse(i, list)                                            \
    for (list_node_t *__(i) = (list).dummy.previous;                         \
         __(i) == &(list).dummy ?  0 : (i = (typeof(i))__(i)->item, 1);      \
         __(i) = __(i)->previous)

struct list_node_t {
    typedef void *item_t;
    list_node_t *next, *previous;
    void *item;

    list_node_t()
    {
        next = previous = NULL;
        item = NULL;
    }

    list_node_t(void *item)
    {
        next = previous = NULL;
        this->item = item;
    }

    void unlink()
    {
        next->previous = previous;
        previous->next = next;
        next = previous = NULL;
    }
};

struct list_t {
    list_node_t dummy;
    typedef void *item_t;

    static void link(list_node_t *left, list_node_t *right)
    {
        left->next = right;
        right->previous = left;
    }

    static void link(list_node_t *left, list_node_t *middle, list_node_t *right)
    {
        link(left, middle);
        link(middle, right);
    }

    static void *destroy(list_node_t *list_node);

    list_t()
    {
        dummy.next = dummy.previous = &dummy;
    }

    list_t(void *item)
    {
        push_back(item);
    }

    ~list_t()
    {
        clear();
    }

    void clear();

    int is_empty()
    {
        return dummy.next == &dummy;
    }

    list_node_t *begin()
    {
        return dummy.next;
    }

    list_node_t *end()
    {
        return &dummy;
    }

    list_node_t *reverse_begin()
    {
        return dummy.previous;
    }

    list_node_t *revsere_end()
    {
        return &dummy;
    }

    void *front()
    {
        return is_empty() ? NULL : dummy.next->item;
    }

    void *back()
    {
        return is_empty() ? NULL : dummy.previous->item;
    }

    void push_front(void *item);
    void *pop_front();
    void push_back(void *item);
    void *pop_back();
    int count();
    void to_array(void *array[]);
};

#endif
