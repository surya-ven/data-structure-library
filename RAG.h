#ifndef RAG_H
#define RAG_H

#include "hashtable.h"
#include "dlinkedlist.h"

#define NOT_SAME_TYPE -2

typedef void (* free_RAG_t)(void*);

typedef enum type {
    PROCESS_T,
    RESOURCE_T
} type_t;

typedef enum status {
    ADDED_NODE,
    UPDATED_NEXT,
    UPDATED_DATA,
    UPDATED_NEXT_DATA,
    NO_UPDATE
} status_t;

typedef struct ckey {
    type_t type;
    void* id;
} ckey_t;

typedef struct RAG_node RAG_node_t;

struct RAG_node {
    ckey_t* key;
    void* data;
    // Prev node for when user converts RAG to undirected graph
    RAG_node_t* _prev;
    RAG_node_t* next;
};

typedef struct RAG {
    hashtable_t* ht;
    DLL_t* proc_key_list;
    DLL_t* res_key_list;
    size_t n_processes;
    size_t n_resources;
} RAG_t;

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
RAG_t* RAG_create(compare_t cmp, hash_t hash);

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
status_t RAG_insert(RAG_t* rag, ckey_t* key, void* data, RAG_node_t* next);

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
status_t RAG_hard_insert(RAG_t* rag, ckey_t* key, void* data, RAG_node_t* next);

/*
 * Function: RAG_convert_to_undirected
 * --------------------
 *  Converts the RAG to an undirected graph.
 * 
 *  rag: Pointer to the RAG.
 * 
 *  returns: Nothing.
 */
void RAG_convert_to_undirected(RAG_t* rag);

/*
 * Function: RAG_increment
 * --------------------
 *  Increments the number of processes or resources in the RAG.
 * 
 *  rag: Pointer to the RAG.
 * 
 *  returns: Nothing.
 */
void RAG_increment(RAG_t* rag, type_t type);

/*
 * Function: RAG_decrement
 * --------------------
 *  Decrements the number of processes or resources in the RAG.
 * 
 *  rag: Pointer to the RAG.
 * 
 *  returns: Nothing.
 */
void RAG_decrement(RAG_t* rag, type_t type);

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
void RAG_clean(RAG_t* rag, free_ht_t free_keys, free_ht_t free_values);

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
RAG_node_t* _RAG_create_node(void* key, void* data, RAG_node_t* next);

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
void _RAG_DLL_insert(RAG_t* rag, ckey_t* key);

/*
 * Function: RAG_free_node_key
 * --------------------
 *  Frees the key of a node in the resource allocation graph.
 * 
 *  _key: Pointer to the key of the node.
 * 
 *  returns: Nothing.
 */
void RAG_free_node_key(void* _key);

/*
 * Function: RAG_free_node_value
 * --------------------
 *  Frees the value of a node in the resource allocation graph.
 * 
 *  data: Pointer to the value of the node.
 * 
 *  returns: Nothing.
 */
void RAG_free_node_value(void* data);

#endif