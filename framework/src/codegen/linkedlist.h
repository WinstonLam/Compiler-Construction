#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

// string structure to store a linked list linkedlist
typedef struct linkedlist
{
    char *string;
    char *substring;
    struct linkedlist* next;
} linkedlist;

extern linkedlist *PushIfExistElseCreate(linkedlist *list, char *string, char *substring, int duplicates);
extern linkedlist *New(linkedlist *next, char *string, char *substring);
extern linkedlist *Push(linkedlist *head, char *string, char *substring);
extern linkedlist *Find(linkedlist *head, char *string);
extern linkedlist *Pushnodup(linkedlist *head, char *string, char *substring);
extern void FreeLinkedlist(linkedlist *list);

#endif