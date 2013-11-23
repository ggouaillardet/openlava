/*
 *
 * Elementary single linked list in C.
 */
#include "link.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

/* Euler's cache keeps link_t entries on a list
 * and use them as cache to avoid malloc()/free()
 * operation as much as possible.
 */
#define EULER   1024

static link_t   E;
static link_t   *_euler(void);

static link_t *
_euler(void)
{
    int      n;
    link_t   *e;

    if (LINK_NUM_ENTRIES(&E) > 0)
        return &E;

    for (n = 0; n < EULER; n++) {
        /* populate euler's link
         */
        e = calloc(1, sizeof(link_t));
        if (e == NULL)
            goto aia;

        e->next = E.next;
        E.next = e;
        E.num++;
    }

    return(&E);

aia:

    while (E.next) {
        e = E.next;
        E.next = e->next;
        free(e);
    }

    return NULL;

} /* euler() */

/* calloc() wrapper
 */
link_t *
ecalloc(void)
{
    link_t   *e;

    if (!_euler()) {
        return NULL;
    }

    /* straight outta cache
     */
    e = E.next;
    E.next = e->next;

    e->next = NULL; /* like calloc() */
    e->ptr  = NULL;
    e->num = 0;

    /* yes one less in the E cache
     */
    E.num--;

    return e;

} /* ecalloc() */

/* and free wrapper...
 */
void
efree(link_t *e)
{
    e->next = E.next;
    E.next = e;
    E.num++;

} /* efree() */

/* Make a new link
 */
link_t *
link_init(void)
{
    link_t   *link;

    link = ecalloc();
    if (!link)
        return NULL;

    return link;

} /* initLink() */

/* Insert in front of the head.
 * This is the stack push operation.
 */
int
link_add(link_t *head, void *val)
{
    link_t   *el;

    if (!head)
        return -1;

    el = ecalloc();
    if (!el)
        return -1;

    el->ptr = val;
    el->next = head->next;
    head->next = el;
    head->num++;

    return 0;

} /* inLink() */

void
link_fini(link_t * head)
{
    if (!head)
        return;

    while (link_pop(head))
        head->num--;

    efree(head);

} /* finLink() */

/* Just wrap the inLink() and call it push.
 */
int
link_push(link_t *head, void *val)
{
    int   cc;

    if (!head)
        return -1;

    cc = link_add(head, val);

    return cc;

} /* pushLink() */

/* The stack pop operation.
 */
void *
link_pop(link_t * head)
{
    link_t   *p;
    void     *t;

    if (!head)
        return NULL;

    if (head->next != NULL) {
        p = head->next;
        head->next = p->next;
        t = p->ptr;
        efree(p);
        head->num--;
        return t;
    }

    return NULL;

} /* popLink() */

/* Queue operation. If the link is long
 * this operation is expensive.
 */
int
link_enqueue(link_t *head, void *val)
{
    link_t   *p;
    link_t   *p2;

    if (!head)
        return -1;

    p2 = ecalloc();
    if (!p2)
        return -1;

    p2->ptr = val;

    /* walk till the end... if long this cost you...
     */
    for (p = head; p->next != NULL; p = p->next)
        ;

    p2->next = p->next;
    p->next = p2;
    head->num++;

    return 0;

} /* enqueueLink() */

/* The opposite of popLink(), return
 * the first element in the list,
 * first in first out, if you inserted
 * the elements by pushLink()
 */
void *
link_dequeue(link_t *head)
{
    link_t   *p;
    link_t   *p2;
    void     *t;

    if (!head
        || !head->next)
        return NULL;

    p2 = head;
    p = head->next;

    while (p->next) {
        p2 = p;
        p = p->next;
    }

    p2->next = p->next;
    t = p->ptr;
    efree(p);
    head->num--;

    return t;

} /* dequeueLink() */

/* Insert in the linked list based on some priority defined
 * by the user function int (*cmp). (*cmp) is supposed to
 * behave the compare function of qsort(3).
 */
int
link_priority_enqueue(link_t *head,
					  void *val,
					  void *extra,
					  int (*cmp)(const void *,
								 const void *,
								 const void *))
{
    link_t   *t;
    link_t   *t2;
    link_t   *p;
    int      cc;

    if (! head)
        return -1;

    t = head;
    t2 = head->next;

    while (t2) {

        /* val is the new element
         */
        cc = (*cmp)(val, t2->ptr, extra);
        if (cc <= 0)
            break;

        t = t2;
        t2 = t2->next;
    }

    p = ecalloc();
    if (!p)
        return -1;

    p->next = t2;
    t->next = p;
    p->ptr = val;
    head->num++;

    return 0;

} /* priorityLink() */

/* Return the address of the first element saved in the
 * in the linked list, the top of the stack. Unlike popLink()
 * this routine does not remove the element from the list.
 */
void *
link_visit(link_t *head)
{
    void   *p;

    if (!head || !head->next)
        return NULL;

    p = head->next->ptr;

    return p;

} /* visitLink() */

/* Remove the element val from the link.
 */
void *
link_rm(link_t *head, void *val)
{
    link_t   *p;
    link_t   *t;
    void     *v;

    if (!head)
        return NULL;

    /* Since we have only one link
     * we need to keep track of 2
     * elements: the current and
     * the previous.
     */

    t = head;
    for (p = head->next;
         p != NULL;
         p = p->next) {
        if (p->ptr == val) {
            t->next = p->next;
            v = p->ptr;
            efree(p);
            head->num--;
            return v;
        }
        t = p;
    }

    return p;

} /* rmlink() */

/* Find an element val, return it, but do not
 * remove from the list.
 */
void *
link_peek(link_t *head, void *val)
{
    link_t   *p;

    for (p = head->next;
         p != NULL;
         p = p->next) {
        if (p->ptr == val) {
            return p->ptr;
        }
    }

    return NULL;

} /* peekLink() */

/* We could this interface as
 * traverseInit(link_t *head, link_t **iter),
 * however then iter would need to be dynamically
 * allocated, while in this case it can be
 * an automatic variable defined by the caller.
 *
 * Here we still think that the STL concept of
 * iterator is useful on collections that have a bit
 * of underlying complexity....although how complex
 * is:
 *    for (p = head; p->next != NULL; p = p->next)
 *           dome(p->next);
 *
 */
void
link_traverse_init(const link_t *head, linkiter_t *iter)
{
    if (head == NULL) {
        memset(iter, 0, sizeof(linkiter_t));
        return;
    }

    iter->pos = head->next;

} /* traverseInit() */

void *
link_traverse(linkiter_t *iter)
{
    void   *p;

    if (!iter)
        return NULL;

    if (iter->pos != NULL) {
        p = iter->pos->ptr;
        iter->pos = iter->pos->next;
        return p;
    }

    return NULL;

} /* traverseLink() */

#if 0
/* Dybag && test
 */
static void
link_print(link_t *h)
{
    link_t *p;

    printf("head %p\n", h);

    for (p = h->next; p != NULL; p = p->next) {
        printf("%p\n", p);
    }
}
#include<stdio.h>
#include"link.h"
#include<stdlib.h>

int
main(void)
{
    link_t   *h;
    int      i;
    char     *p;

    h = initLink();

    for (i = 0; i < 1024 * 2; i++) {

        p = malloc(128);
        enqueueLink(h, p);
        printf("%p enqueued\n", p);
    }

    printLink(h);

    while ((p = dequeueLink(h))) {
        printf("%p dequeued\n", p);
        free(p);
    }

    finLink(h);

    return 0;
}
#endif
