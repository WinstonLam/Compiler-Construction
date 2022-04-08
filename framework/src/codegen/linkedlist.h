#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

// string structure to store a linked list linkedlist
typedef struct linkedlist
{
    char *string;
    struct linkedlist* next;
} linkedlist;

extern linkedlist *PushIfExistElseCreate(linkedlist *list, char *string);
extern linkedlist *New(linkedlist *next, char *string);
extern linkedlist *Push(linkedlist *head, char *string);
extern void FreeLinkedlist(linkedlist *list);

#endif