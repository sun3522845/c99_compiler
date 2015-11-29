#include "root.h"
#include "list.h"

void list_t::clear()
{
    list_node_t *this_node = dummy.next;
    list_node_t *next_node;
    while (this_node != &dummy) {
        next_node = this_node->next;
        this_node->unlink();
        delete this_node;
        this_node = next_node;
    }
}

void *list_t::destroy(list_node_t *list_node)
{
    void *item = list_node->item;
    list_node->unlink();
    delete list_node;
    return item;
}

void list_t::push_front(void *item)
{
    list_node_t *left = &dummy;
    list_node_t *right = dummy.next;
    link(left, new list_node_t(item), right); 
}

void *list_t::pop_front()
{
    if (is_empty())
        return NULL;
    return destroy(dummy.next);
}

void list_t::push_back(void *item)
{
    list_node_t *left = dummy.previous;
    list_node_t *right = &dummy;
    link(left, new list_node_t(item), right); 
}

void *list_t::pop_back()
{
    if (is_empty())
        return NULL;
    return destroy(dummy.previous);
}

int list_t::count()
{
    int sum = 0;
    list_node_t *cursor;
    for (cursor = dummy.next; cursor != &dummy; cursor = cursor->next)
        sum++;
    return sum;
}

void list_t::to_array(void *array[])
{
    int i = 0;
    void *item;
    for_each (item, *this) {
        array[i] = item;
        i++;
    }
}
