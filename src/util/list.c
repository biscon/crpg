//
// Created by bison on 10/6/18.
//

#include <malloc.h>
#include <assert.h>
#include "list.h"

void List_Push(ListNode** head, void *item)
{
    ListNode* new_node;
    new_node = calloc(1, sizeof(ListNode));
    new_node->data = item;
    new_node->next = *head;
    *head = new_node;
}

void *List_Pop(ListNode **head)
{
    ListNode* next_node = NULL;
    void* retval;
    if (*head == NULL) {
        return NULL;
    }
    next_node = (*head)->next;
    retval = (*head)->data;
    free(*head);
    *head = next_node;
    return retval;
}

void List_Remove(ListNode **head, void *item)
{
    // if item is the first in list
    if((*head)->data == item) {
        List_Pop(head);
        return;
    }
    ListNode* current = *head;
    ListNode* tmp_node = NULL;
    while(current != NULL) {
        // if the next item is the one we're looking for break
        if(current->next != NULL) {
            if (current->next->data == item) {
                break;
            }
        }
        current = current->next;
    }

    // could not find the node
    if(current == NULL)
        return;

    // save pointer to the node we're removing, set prev's next to point at current next
    tmp_node = current->next;
    current->next = tmp_node->next;
    free(tmp_node);
}

void List_Destroy(ListNode **head) {
    ListNode* current = *head;
    if(current == NULL)
        return;
    while(current != NULL) {
        ListNode* next = current->next;
        free(current);
        current = next;
    }
    (*head) = NULL;
}

bool List_Has(ListNode **head, void *item) {
    ListNode* current = *head;
    if(current == NULL)
        return false;
    while(current != NULL) {
        if(current == item)
            return true;
        current = current->next;
    }
    return false;
}


