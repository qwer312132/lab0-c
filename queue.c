#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "harness.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *q = (struct list_head *) malloc(sizeof(struct list_head));
    if (q) {
        INIT_LIST_HEAD(q);
        return q;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *li = l->next;
    while (li != l) {
        struct list_head *temp = li;
        li = li->next;
        element_t *e = container_of(temp, element_t, list);
        q_release_element(e);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = (element_t *) malloc(sizeof(element_t));
    if (!e)
        return false;
    e->value = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (!e->value) {
        free(e);
        return false;
    }
    strncpy(e->value, s, strlen(s));
    list_add(&e->list, head);
    e->value[strlen(s)] = '\0';
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = (element_t *) malloc(sizeof(element_t));
    if (!e)
        return false;
    e->value = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (!e->value) {
        free(e);
        return false;
    }
    strncpy(e->value, s, strlen(s));
    list_add_tail(&e->list, head);
    e->value[strlen(s)] = '\0';
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
#define min(a, b) (((a) < (b)) ? (a) : (b))
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head && !list_empty(head) && sp) {
        struct list_head *h = head->next;
        element_t *e = container_of(h, element_t, list);
        int L = min(strlen(e->value), bufsize - 1);
        strncpy(sp, e->value, L);
        list_del(h);
        sp[L] = '\0';
        return e;
    }
    return NULL;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head && !list_empty(head) && sp) {
        struct list_head *t = head->prev;
        element_t *e = container_of(t, element_t, list);
        int L = min(strlen(e->value), bufsize - 1);
        strncpy(sp, e->value, L);
        list_del(t);
        sp[L] = '\0';
        return e;
    }
    return NULL;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return NULL if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head)
        return NULL;
    struct list_head *temp1 = head->next;
    struct list_head *temp2 = head->prev;
    while (temp1 != temp2 && temp1->next != temp2) {
        temp1 = temp1->next;
        temp2 = temp2->prev;
    }
    list_del(temp1);
    q_release_element(container_of(temp1, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return NULL;
    struct list_head *temphead = head->next;
    while (temphead != head) {
        bool flag = 0;
        element_t *tempheadElement = container_of(temphead, element_t, list);
        struct list_head *temp = temphead->next;
        while (temp != head) {
            element_t *tempElement = container_of(temp, element_t, list);
            if (strcmp(tempElement->value, tempheadElement->value) == 0) {
                flag = 1;
                list_del(temp);
                temp = temp->next;
                q_release_element(tempElement);
            } else {
                break;
            }
        }
        temphead = temphead->next;
        if (flag) {
            list_del(temphead->prev);
            q_release_element(tempheadElement);
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    else {
        struct list_head *temp1, *temp2, *temp3;
        temp1 = head->next;
        temp2 = temp1->next;
        temp3 = head;
        while (1) {
            temp1->next = temp2->next;
            temp1->prev = temp2;
            temp2->next = temp1;
            temp2->prev = temp3;
            temp3->next = temp2;
            temp1->next->prev = temp1;
            if (temp1->next != head && temp1->next->next != head) {
                temp3 = temp1;
                temp1 = temp1->next;
                temp2 = temp1->next;
            } else {
                break;
            }
        }
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void swap_two_node(struct list_head **node1, struct list_head **node2)
{
    struct list_head *temp;
    temp = *node1;
    *node1 = *node2;
    *node2 = temp;
}
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *temp = head;
    swap_two_node(&temp->next, &temp->prev);
    list_for_each (temp, head) {
        swap_two_node(&temp->next, &temp->prev);
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
struct list_head *merge(struct list_head *l, struct list_head *r)
{
    struct list_head *head = NULL;
    struct list_head **indirect = &head;
    while (l && r) {
        element_t *elementl = container_of(l, element_t, list);
        element_t *elementr = container_of(r, element_t, list);
        if (strcmp(elementl->value, elementr->value) < 0) {
            *indirect = l;
            l = l->next;

        } else {
            *indirect = r;
            r = r->next;
        }
        indirect = &(*indirect)->next;
    }
    *indirect = (struct list_head *) ((uintptr_t) l | (uintptr_t) r);
    return head;
}
struct list_head *mergesort(struct list_head *l, struct list_head *r)
{
    struct list_head *tortoise = l;
    struct list_head *hare = l;
    if (l == r) {
        l->next = NULL;
        return l;
    } else if (l->next == r) {
        element_t *left = container_of(l, element_t, list);
        element_t *right = container_of(r, element_t, list);
        if (strcmp(left->value, right->value) > 0) {
            l->next = NULL;
            r->next = l;
            return r;
        } else {
            r->next = NULL;
            return l;
        }
    }
    while (hare->next != NULL && hare->next->next != NULL && hare->next != r &&
           hare->next->next != r) {
        hare = hare->next->next;
        tortoise = tortoise->next;
    }
    struct list_head *list1, *list2;
    struct list_head *temp = tortoise->next;
    list1 = mergesort(l, tortoise);
    list2 = mergesort(temp, r);
    return merge(list1, list2);
}
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    head->prev->next = NULL;
    head->next = mergesort(head->next, head->prev);
    struct list_head *node = head;
    while (node->next) {
        node->next->prev = node;
        node = node->next;
    }
    node->next = head;
    head->prev = node;
}