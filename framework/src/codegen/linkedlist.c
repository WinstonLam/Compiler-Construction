#include "linkedlist.h"

#include <stdio.h>
#include <stdlib.h>
#include "dbug.h"


linkedlist *PushIfExistElseCreate(linkedlist *list, char *data) {
    if(list)
    {
        return Push(list, data);
    } else {
        return New(list, data);
    }
}

// Helper function to return new linked list linkedlist from the heap
linkedlist *New(linkedlist *next, char *data)
{
    // allocate a new linkedlist in a heap and set its data
    struct linkedlist* linkedlist = (struct linkedlist*)malloc(sizeof(struct linkedlist));
    linkedlist->data = data;
    linkedlist->next = next;

    DBUG_PRINT("GBC", ("%s", linkedlist));

    return linkedlist;
}

linkedlist *Push(linkedlist *head, char *data)
{
    if (head == NULL)
    {
        return NULL;
    }

    linkedlist *current = head;
    while (current->next != NULL)
    {
        current = current->next;
    }

    linkedlist *newList = New(NULL, data);
    current->next = newList;

    return head;
}

void Free(linkedlist *list)
{
    linkedlist *updated;

    while(list != NULL)
    {
        updated = list->next;
        free(list->data);
        free(list);

        list = updated;
    }
}
