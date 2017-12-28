
#ifndef DLIST_H
#define DLIST_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct list_node
{
	struct list_node *next;
	struct list_node *prev;
	void *data;
}list_node;

typedef int(*list_compare)(void *, void *);

typedef struct
{
	int count;
	list_node *head;
	list_node *tail;
	list_compare compare;
}dlist;

int dlist_init(dlist * list);
int dlist_count(dlist * list);
int dlist_int_comp(void *a, void *b);
int dlist_set_compare(dlist * list, list_compare compare);
int dlist_add(dlist * list, void * data);
int dlist_insert(dlist *list, void *data);
int dlist_delete(dlist * list, void * data);
int dlist_insert_node(dlist *list, list_node *node);
int dlist_add_node(dlist * list, list_node *node);
int dlist_move_node(dlist * list, list_node * node);
int dlist_delete_node(dlist * list, list_node * node);
int dlist_clean(dlist * list);
int dlist_clean_data(dlist *list);
int dlist_append(dlist * toList, dlist * addList);
int dlist_get_index(dlist * list, void * data);
void *dlist_pop(dlist *list);
void * dlist_get_data(dlist * list, int index);
list_node *dlist_pop_node(dlist * list);
list_node *dlist_move_data(dlist * list, void *data);
list_node * dlist_get_first_node(dlist * list);
list_node *dlist_get_next_node(dlist * list, list_node * base_node);
list_node *dlist_get_prev_node(dlist * list, list_node * base_node);
list_node *dlist_find_node(dlist *list, void *data);
dlist *dlist_create();
int dlist_distory(dlist * list);
int dlist_test();

#ifdef __cplusplus
}
#endif
#endif

