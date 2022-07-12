/*
Author : Surya Venkatesh
Purpose: This file is a custom doubly linked list library. It also contains 
         the ability to create a hashtable embeded within the DLL for avg O(1)
         removal if the user wants to delete using a key. Also allows for all
         benefits of ht with the user being able to use hash table functions
         directly on the embedded hashtable.
*/

#include "dlinkedlist.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "hashtable.h"

/* STANDARD DOUBLY LINKED LIST */

/*
 * Function: DLL_create
 * --------------------
 *  Creates a new doubly linked list.
 * 
 *  No arguments.
 * 
 *  returns: Pointer to the new doubly linked list.
 */
DLL_t* DLL_create(void) {
    DLL_t* dll = malloc(sizeof(DLL_t));
    assert(dll);

    dll->head = NULL;
    dll->tail = NULL;

    return dll;
}

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
void DLL_insert_head(DLL_t* dll, void* data) {
    assert(dll);

    // Create new node
    DLL_node_t* node = malloc(sizeof(DLL_node_t));
    assert(node);
    node->data = data;
    node->next = dll->head;
    node->prev = NULL;

    // Insert at head
    if (dll->head) {
        dll->head->prev = node;
    }

    if (!dll->tail) {
        dll->tail = node;
    }

    dll->head = node;
}

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
void DLL_insert_tail(DLL_t* dll, void* data) {
    assert(dll);

    // Create new node
    DLL_node_t* node = malloc(sizeof(DLL_node_t));
    assert(node);
    node->data = data;
    node->next = NULL;
    node->prev = dll->tail;

    // Insert at tail
    if (dll->tail) {
        dll->tail->next = node;
    }

    if (!dll->head) {
        dll->head = node;
    }

    dll->tail = node;
}

/*
 * Function: DLL_pop
 * --------------------
 *  Removes the head node from the list.
 * 
 *  dll: Pointer to the doubly linked list.
 * 
 *  returns: Pointer to the data of the removed node.
 */
void* DLL_pop(DLL_t* dll) {
    assert(dll);
    DLL_node_t* node = NULL;

    if (!dll->head) {
        return NULL;
    }

    // Remove head
    node = dll->head;
    dll->head = node->next;

    if (dll->head) {
        dll->head->prev = NULL;
    }

    if (node == dll->tail) {
        dll->tail = NULL;
    }

    void* data = node->data;
    free(node);

    return data;
}

/*
 * Function: DLL_dequeue
 * --------------------
 *  Removes the tail node from the list.
 * 
 *  dll: Pointer to the doubly linked list.
 * 
 *  returns: Pointer to the data of the removed node.
 */
void* DLL_dequeue(DLL_t* dll) {
    assert(dll);
    DLL_node_t* node = NULL;

    if (!dll->tail) {
        return NULL;
    }

    // Remove tail
    node = dll->tail;
    dll->tail = node->prev;

    if (dll->tail) {
        dll->tail->next = NULL;
    }

    if (node == dll->head) {
        dll->head = NULL;
    }

    void* data = node->data;
    free(node);

    return data;
}

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
void DLL_clean(DLL_t* dll, free_dll_t free_data) {
    assert(dll);

    // Remove all nodes
    while (dll->head) {
        void* data = DLL_pop(dll);
        if (free_data) {
            free_data(data);
        }
    }

    free(dll);
}

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
DLL_HT_t* DLL_HT_create(compare_t cmp, hash_t hash) {
    assert(cmp);
    assert(hash);

    DLL_HT_t* dll_ht = NULL;
    DLL_t* dll = NULL;
    hashtable_t* ht = NULL;

    dll_ht = malloc(sizeof(DLL_HT_t));
    assert(dll_ht);

    dll = DLL_create();

    ht = ht_create(INITIAL_TABLE_SIZE, cmp, hash);

    // Assign to combined data structure
    dll_ht->list = dll;
    dll_ht->ht = ht;

    return dll_ht;
}

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
void DLL_HT_insert(DLL_HT_t* dll_ht, void* key, void* data) {
    assert(dll_ht);
    assert(key);

    DLL_node_t* node = NULL;

    if (ht_contains(dll_ht->ht, key)) {
        return;
    }

    // Create new node from tail insertion
    DLL_insert_tail(dll_ht->list, data);
    node = dll_ht->list->tail;

    // Insert into hashtable
    ht_unique_insert(dll_ht->ht, key, node);
}

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
void DLL_HT_remove(DLL_HT_t* dll_ht, void* key, free_dll_ht_t free_data) {
    assert(dll_ht);
    assert(key);

    DLL_node_t* node = NULL;

    node = ht_search(dll_ht->ht, key);

    // Disconnect prev
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        dll_ht->list->head = node->next;
    }

    // Disconnect next
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        dll_ht->list->tail = node->prev;
    }

    // Remove from hashtable
    ht_remove(dll_ht->ht, key, NULL, NULL);

    // Free data if needed
    if (free_data) {
        free_data(node->data);
    }

    free(node);
}

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
                free_dll_ht_t free_data) {
    assert(dll_ht);

    DLL_clean(dll_ht->list, free_data);
    ht_clean(dll_ht->ht, free_key, NULL);
    free(dll_ht);
}


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
bool DLL_is_empty(DLL_t* dll) {
    assert(dll);

    if (!dll->head && !dll->tail) {
        return true;
    }

    return false;
}
