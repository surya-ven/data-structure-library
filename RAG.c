/*
Author : Surya Venkatesh
Purpose: This file is a custom resource allocation graph library.
*/

#include "RAG.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hashtable.h"

/**** PUBLIC ****/

/*
 * Function: RAG_create
 * --------------------
 *  Creates a new RAG.
 * 
 *  cmp: Function pointer to compare two keys.
 *  hash: Function pointer to hash a key.
 * 
 *  returns: Pointer to the new RAG.
 */
RAG_t* RAG_create(compare_t cmp, hash_t hash) {
    assert(cmp);
    assert(hash);
    
    RAG_t* rag = malloc(sizeof(RAG_t));
    assert(rag);

    // Initialise hashtable for storage of both processes and resources
    rag->ht = ht_create(INITIAL_TABLE_SIZE, cmp, hash);
    assert(rag->ht);

    // Initialise doubly linked lists for adjacency lists
    rag->proc_key_list = DLL_create();
    rag->res_key_list = DLL_create();

    rag->n_resources = 0;
    rag->n_processes = 0;

    return rag;
}

/*
 * Function: RAG_insert
 * --------------------
 *  Inserts a process or resource node into the RAG, updates the node with new 
 *  values if it already exists, given the value it is updating is currently 
 *  NULL.
 * 
 *  rag: Pointer to the RAG.
 *  key: Key to insert.
 *  data: Value to insert.
 *  next: Pointer to the next node.
 * 
 *  returns: status_t indicating insertion update status.
 */
status_t RAG_insert(RAG_t* rag, ckey_t* key, void* data, RAG_node_t* next) {
    assert(rag);
    assert(key);

    RAG_node_t* node = NULL;
    ht_node_t* ht_node = NULL;

    // Create new node if it doesn't exist
    if (!(ht_node = ht_get_node(rag->ht, key))) {
        node = _RAG_create_node(key, data, next);
        ht_unique_insert(rag->ht, key, node);
        _RAG_DLL_insert(rag, key);

        RAG_increment(rag, node->key->type);
        return ADDED_NODE;
    } else {
        // Update NULL node attributes if it exists
        node = ht_node->value;
        if ((next && !(node->next)) && (data && !(node->data))) {
            node->next = next;
            node->data = data;
            return UPDATED_NEXT_DATA;
        } else if (next && !(node->next)) {
            node->next = next;
            return UPDATED_NEXT;
        } else if (data && !(node->data)) {
            node->data = data;
            return UPDATED_DATA;
        }
    }
    
    return NO_UPDATE;
}

/*
 * Function: RAG_hard_insert
 * --------------------
 *  Inserts a process or resource node into the RAG, if the new node already 
 *  exists, overwrites the attributes of the node with the new values.
 * 
 *  rag: Pointer to the RAG.
 *  key: Key to insert.
 *  data: Value to insert.
 *  next: Pointer to the next node.
 * 
 *  returns: status_t indicating insertion update status.
 */
status_t RAG_hard_insert(RAG_t* rag, ckey_t* key, void* data, RAG_node_t* next) {
    assert(rag);
    assert(key);
    
    RAG_node_t* node = NULL;
    ht_node_t* ht_node = NULL;

    // Create new node if it doesn't exist
    if (!(ht_node = ht_get_node(rag->ht, key))) {
        node = _RAG_create_node(key, data, next);
        ht_unique_insert(rag->ht, key, node);
        _RAG_DLL_insert(rag, key);
        RAG_increment(rag, node->key->type);
        return ADDED_NODE;
    } else {
        // Update and overwrite node attributes if it exists
        node = ht_node->value;
        if (next && data) {
            node->next = next;
            node->data = data;
            return UPDATED_NEXT_DATA;
        } else if (next) {
            node->next = next;
            return UPDATED_NEXT;
        } else if (data) {
            node->data = data;
            return UPDATED_DATA;
        }
    }
    
    return NO_UPDATE;
}

/*
 * Function: RAG_convert_to_undirected
 * --------------------
 *  Converts the RAG to an undirected graph.
 * 
 *  rag: Pointer to the RAG.
 * 
 *  returns: Nothing.
 */
void RAG_convert_to_undirected(RAG_t* rag) {
    assert(rag);
    ckey_t* key = NULL;
    RAG_node_t* RAG_node = NULL;

    // Iterate processes and assign prev pointer of next node to current node
    for (DLL_node_t* node = rag->proc_key_list->head; node; node = node->next) {
        key = (ckey_t*)node->data;
        RAG_node = ht_search(rag->ht, key);
        if (RAG_node->next) {
            RAG_node->next->_prev = RAG_node;
        }
    }

    // Iterate resources and assign prev pointer of next node to current node
    for (DLL_node_t* node = rag->res_key_list->head; node; node = node->next) {
        key = (ckey_t*)node->data;
        RAG_node = ht_search(rag->ht, key);
        if (RAG_node->next) {
            RAG_node->next->_prev = RAG_node;
        }
    }
}

/*
 * Function: RAG_increment
 * --------------------
 *  Increments the number of processes or resources in the RAG.
 * 
 *  rag: Pointer to the RAG.
 * 
 *  returns: Nothing.
 */
void RAG_increment(RAG_t* rag, type_t type) {
    assert(rag);
    if (type == PROCESS_T) {
        rag->n_processes++;
    } else {
        rag->n_resources++;
    }
}

/*
 * Function: RAG_decrement
 * --------------------
 *  Decrements the number of processes or resources in the RAG.
 * 
 *  rag: Pointer to the RAG.
 * 
 *  returns: Nothing.
 */
void RAG_decrement(RAG_t* rag, type_t type) {
    assert(rag);
    if (type == PROCESS_T && rag->n_processes > 0) {
        rag->n_processes--;
    } else if (type == RESOURCE_T && rag->n_resources > 0) {
        rag->n_resources--;
    }
}

/*
 * Function: RAG_clean
 * --------------------
 *  Frees all the memory allocated for the RAG.
 * 
 *  rag: Pointer to the RAG.
 *  free_keys: Function pointer to the function that frees the keys.
 *  free_values: Function pointer to the function that frees the values.
 * 
 *  returns: Nothing.
 */
void RAG_clean(RAG_t* rag, free_ht_t free_keys, free_ht_t free_values) {
    assert(rag);
    // Clean RAG
    DLL_clean(rag->proc_key_list, NULL);
    DLL_clean(rag->res_key_list, NULL);
    ht_clean(rag->ht, free_keys, free_values);
    free(rag);
}

/*
 * Function: RAG_free_node_key
 * --------------------
 *  Frees the key of a node in the resource allocation graph.
 * 
 *  _key: Pointer to the key of the node.
 * 
 *  returns: Nothing.
 */
void RAG_free_node_key(void* _key) {
    ckey_t* key = (ckey_t*)_key;
    free(key->id);
    free(key);
}

/*
 * Function: RAG_free_node_value
 * --------------------
 *  Frees the value of a node in the resource allocation graph.
 * 
 *  data: Pointer to the value of the node.
 * 
 *  returns: Nothing.
 */
void RAG_free_node_value(void* data) {
    RAG_node_t* node = (RAG_node_t*)data;
    if (node->data) {
        free(node->data);
    }
    free(node);
}

/**** PRIVATE ****/
/*
 * Function: _RAG_create_node
 * --------------------
 *  Creates a new RAG node.
 * 
 *  key: Key to insert.
 *  data: Value to insert.
 *  next: Pointer to the next RAG node.
 * 
 *  returns: Pointer to the new RAG node.
 */
RAG_node_t* _RAG_create_node(void* key, void* data, RAG_node_t* next) {
    RAG_node_t* node = malloc(sizeof(RAG_node_t));
    assert(node);
    node->key = key;
    node->data = data;
    node->next = next;
    node->_prev = NULL;
    return node;
}

/*
 * Function: _RAG_DLL_insert
 * --------------------
 *  Inserts a key into the DLL.
 * 
 *  rag: Pointer to the RAG.
 *  key: Key to insert.
 * 
 *  returns: Nothing.
 */
void _RAG_DLL_insert(RAG_t* rag, ckey_t* key) {
    if (key->type == PROCESS_T) {
        DLL_insert_tail(rag->proc_key_list, key);
    } else {
        DLL_insert_tail(rag->res_key_list, key);
    }
}