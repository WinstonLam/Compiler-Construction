#include "linkedlist.h"

#include <stdio.h>
#include <stdlib.h>
#include "dbug.h"


linkedlist *PushIfExistElseCreate(linkedlist *list, char *string) {
    if(list)
    {
        return Push(list, string);
    } else {
        return New(list, string);
    }
}

// Helper function to return new linked list linkedlist from the heap
linkedlist *New(linkedlist *next, char *string)
{
    // allocate a new linkedlist in a heap and set its string
    struct linkedlist* linkedlist = (struct linkedlist*)malloc(sizeof(struct linkedlist));
    linkedlist->string = string;
    linkedlist->next = next;

    return linkedlist;
}

linkedlist *Push(linkedlist *head, char *string)
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

    linkedlist *newList = New(NULL, string);
    current->next = newList;

    return head;
}

void FreeLinkedlist(linkedlist *list)
{
    // DBUG_PRINT("GBC", ("%s", linkedlist));
    linkedlist *updated;

    while(list != NULL)
    {
        updated = list->next;
        free(list->string);
        free(list);

        list = updated;
    }
}