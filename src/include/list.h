//
// Created by bison on 10/6/18.
//

#ifndef GAME_LIST_H
#define GAME_LIST_H

#include "defs.h"

#define LIST_PUSH(head, item)           List_Push(&(head), (item))
#define LIST_POP(head, type)     (type) List_Pop(&(head))
#define LIST_REMOVE(head, item)         List_Remove(&(head), (item))
#define LIST_DESTROY(head)              List_Destroy(&(head))
#define LIST_HAS(head, item)            List_Has((head), (item))

typedef struct ListNode ListNode;

struct ListNode {
    void*       data;
    ListNode*   next;
};

void    List_Push(ListNode** head, void *item);
void    List_Remove(ListNode** head, void *item);
void*   List_Pop(ListNode** head);
void    List_Destroy(ListNode** head);
bool    List_Has(ListNode** head, void *item);

#endif //GAME_LIST_H
