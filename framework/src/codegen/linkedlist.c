#include "linkedlist.h"

#include <stdio.h>
#include "str.h"
#include <stdlib.h>
#include "dbug.h"
#include "ctinfo.h"



linkedlist *PushIfExistElseCreate(linkedlist *list, char *string, char *substring, int duplicates){
    if(list)
    {
        if (duplicates == 0) {
            return Push(list, string, substring);
        } else {
            return Pushnodup(list, string, substring);
        }
    } else {
        return New(list, string, substring);
    }
}

// Helper function to return new linked list linkedlist from the heap
linkedlist *New(linkedlist *next, char *string, char *substring)
{
    // allocate a new linkedlist in a heap and set its string
    struct linkedlist* linkedlist = (struct linkedlist*)malloc(sizeof(struct linkedlist));
    linkedlist->string = string;
    linkedlist->substring = substring;
    linkedlist->next = next;

    return linkedlist;
}

linkedlist *Push(linkedlist *head, char *string ,char *substring)
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

    linkedlist *newList = New(NULL, string, substring);
    current->next = newList;

    return head;
}
// this function adds to linked list if the string is not already in the list
linkedlist *Pushnodup (linkedlist *head, char *string ,char *substring)
{
    if (head == NULL)
    {
        return NULL;
    }

    linkedlist *current = head;
    if (STReq(current->string, string))
        {
            current->substring = substring;
            return head;
        }
    while (current->next != NULL)
    {
        if (STReq(current->string, string))
        {
            current->substring = substring;
            return head;
        }
        current = current->next;
    }

    linkedlist *newList = New(NULL, string, substring);
    current->next = newList;

    return head;
}

linkedlist *Find(linkedlist *head, char *string)
{
    linkedlist *current = head;
    while (current != NULL)
    {
        if (STReq(current->string, string))
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void FreeLinkedlist(linkedlist *list)
{
    // DBUG_PRINT("GBC", ("%s", linkedlist));
    linkedlist *updated;

    while(list != NULL)
    {
        updated = list->next;
        free(list->string);
        free(list->substring);
        free(list);

        list = updated;
    }
}