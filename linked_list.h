#ifndef LINKED_LIST_H_INCLUDED
#define LINKED_LIST_H_INCLUDED
#include "memory.h"

typedef struct st_list_node
{
    struct st_list_node* next;
    void* value;
}NodeList;


typedef struct st_list
{
    size_t size;
    short destroy_values;
    NodeList* first, *last;
}List;

typedef struct st_list_iterator
{
    List list;
    NodeList* current_node;
    void* current_value;
}ListIterator;

void initLinkedList(List* list, int destroy_values)
{
    list->first = NULL;
    list->last = NULL;
    list->size = 0;

    list->destroy_values = destroy_values;
}

NodeList* newNodeList(void* value, NodeList* next)
{
    NodeList* new_node = madaAlloc(new_node, sizeof(NodeList), 0);

    new_node->value = value;
    new_node->next = next;

    return new_node;
}

void destroyNodeList(NodeList* node, int destroy_value)
{
    if(node)
    {
        if(destroy_value)
        {
            free(node->value);
        }

        free(node);
    }
}

void destroyLinkedList(List* list)
{
    NodeList* act = list->first, *del=NULL;

    while(act!= NULL)
    {
        del = act;
        act = act->next;

        destroyNodeList(del, list->destroy_values);
    }
}

void appendLinkedList(List* list, void* value)
{

    NodeList* new_node = newNodeList(value, NULL);

    if(!list->size)
    {

        list->first = new_node;
        list->last = new_node;
    }
    else
    {
        list->last->next = new_node;
        list->last = list->last->next;
    }

    list->size++;
}

void* dequeueList(List* list)
{

    if(list->size)
    {
        NodeList* return_node = list->first;
        void* value = return_node->value;

        list->first = list->first->next;

        destroyNodeList(return_node, list->destroy_values);

        list->size--;

        return value;
    }

    return NULL;
}

void initListIterator(ListIterator* it, List list)
{
    it->list = list;
    it->current_node = list.first;
    it->current_value = NULL;

}

void* iterateList(ListIterator* it)
{
    if(it->current_node)
    {

        it->current_value = it->current_node->value;
        it->current_node = it->current_node->next;

        return it->current_value;

    }

    return NULL;
}


#endif // LINKED_LIST_H_INCLUDED
