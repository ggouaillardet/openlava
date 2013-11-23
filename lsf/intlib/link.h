/*
 * Elementary single linked list in C.
 * D. Knuth Art of Computer Programming Volume 1. 2.2
 *
 */
#ifndef __LINK__
#define __LINK__

/* Each linked list is made of a head whose ptr is always
 * NULL, a list of following links starting from next and
 * a number num indicating how many elements are in the
 * list.
 */
typedef struct link {
    int           num;
    void          *ptr;
    struct link   *next;
} link_t;

#define LINK_NUM_ENTRIES(L) ((L)->num)

typedef struct linkiter {
    link_t   *pos;
} linkiter_t;

link_t   *ecalloc(void);
void     efree(link_t *);
link_t   *link_int(void);
void     link_fini(link_t *);
int      link_add(link_t *,void *);
void     *link_rm(link_t *, void *);
void     *link_peek(link_t *, void *val);
int      link_push(link_t *, void *);
int      link_enqueue(link_t *, void *);
void     *link_dequeue(link_t *);
int      link_priority_enqueue(link_t *,
							   void *,
							   void *,
							   int (*cmp)(const void *,
										  const void *,
										  const void *));
void     *link_pop(link_t *);
void     *link_visit(link_t *);
void     link_traverse_init(const link_t *,
		      linkiter_t *);
void     *link_traverse(linkiter_t *);

#endif /* __LINK__ */
