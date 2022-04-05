#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

// Data structure to store a linked list linkedlist
typedef struct linkedlist
{
    char *data;
    struct linkedlist* next;
} linkedlist;

extern linkedlist *PushIfExistElseCreate(linkedlist *list, char *data);
extern linkedlist *New(linkedlist *next, char *data);
extern linkedlist *Push(linkedlist *head, char *data);
extern void Free(linkedlist *list);

#endif