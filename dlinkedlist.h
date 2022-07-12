#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "hashtable.h"

typedef void (* free_dll_t)(void*);
typedef void (* free_dll_ht_t)(void*);

typedef struct DLL_node DLL_node_t;

struct DLL_node {
	void* data;
    DLL_node_t* prev;
	DLL_node_t *next;
};

typedef struct DLL {
	DLL_node_t *head;
	DLL_node_t *tail;
} DLL_t;

typedef struct DLL_HT {
    DLL_t *list;
    hashtable_t* ht;
} DLL_HT_t;

/*
 * Function: DLL_create
 * --------------------
 *  Creates a new doubly linked list.
 * 
 *  No arguments.
 * 
 *  returns: Pointer to the new doubly linked list.
 */
DLL_t* DLL_create(void);

/*
 * Function: DLL_insert_head
 * --------------------
 *  Inserts a new node at the head of the list.
 * 
 *  dll: Pointer to the doubly linked list.
 *  data: Value to be inserted.
 * 
 *  returns: Nothing.
 */
void DLL_insert_head(DLL_t* dll, void* data);

/*
 * Function: DLL_insert_tail
 * --------------------
 *  Inserts a new node at the tail of the list.
 * 
 *  dll: Pointer to the doubly linked list.
 *  data: Value to be inserted.
 * 
 *  returns: Nothing.
 */
void DLL_insert_tail(DLL_t* dll, void* data);

/*
 * Function: DLL_pop
 * --------------------
 *  Removes the head node from the list.
 * 
 *  dll: Pointer to the doubly linked list.
 * 
 *  returns: Pointer to the data of the removed node.
 */
void* DLL_pop(DLL_t* dll);

/*
 * Function: DLL_dequeue
 * --------------------
 *  Removes the tail node from the list.
 * 
 *  dll: Pointer to the doubly linked list.
 * 
 *  returns: Pointer to the data of the removed node.
 */
void* DLL_dequeue(DLL_t* dll);

/*
 * Function: DLL_clean
 * --------------------
 *  Removes all nodes from the list.
 * 
 *  dll: Pointer to the doubly linked list.
 *  free_data: Function pointer to the function that frees the data.
 * 
 *  returns: Nothing.
 */
void DLL_clean(DLL_t* dll, free_dll_t free_data);

/* HASHTABLE  DLL */
/*
 * Function: DLL_HT_create
 * --------------------
 *  Creates a new doubly linked list with a hashtable embedded.
 * 
 *  cmp: Function pointer to the function that compares two data values.
 *  hash: Function pointer to the function that hashes a data value.
 * 
 *  returns: Pointer to the new doubly linked list hashtable.
 */
DLL_HT_t* DLL_HT_create(compare_t cmp, hash_t hash);

/*
 * Function: DLL_HT_insert
 * --------------------
 *  Inserts a new node into the doubly linked list hashtable.
 * 
 *  key: Key of the node to be inserted.
 *  data: Value of the node to be inserted.
 * 
 *  returns: Nothing.
 */
void DLL_HT_insert(DLL_HT_t* dll_ht, void* key, void* data);

/*
 * Function: DLL_HT_remove
 * --------------------
 *  Removes a node from the doubly linked list hashtable.
 * 
 *  dll_ht: Pointer to the doubly linked list hashtable.
 *  key: Key of the node to be removed.
 *  free_data: Function pointer to the function that frees the data.
 * 
 *  returns: Nothing.
 */
void DLL_HT_remove(DLL_HT_t* dll_ht, void* key, free_dll_ht_t free_data);

/*
 * Function: DLL_HT_clean
 * --------------------
 *  Removes all nodes from the doubly linked list hashtable.
 * 
 *  dll_ht: Pointer to the doubly linked list hashtable.
 *  free_key: Function pointer to the function that frees the key.
 *  free_data: Function pointer to the function that frees the data.
 * 
 *  returns: Nothing.
 */
void DLL_HT_clean(DLL_HT_t* dll_ht, free_dll_ht_t free_key, 
                free_dll_ht_t free_data);

/* UTILS */
/*
 * Function: DLL_is_empty
 * --------------------
 *  Checks if the doubly linked list is empty.
 * 
 *  dll_ht: Pointer to the doubly linked list hashtable.
 * 
 *  returns: True if the list is empty, false otherwise.
 */
bool DLL_is_empty(DLL_t* dll);

#endif