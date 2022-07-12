#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_TABLE_SIZE 49
#define MAX_LOAD_FACTOR 1.0
#define GROWTH_FACTOR 2

typedef int (* compare_t)(const void*, const void*);
typedef size_t (* hash_t)(const void*);
typedef void (* free_ht_t)(void*);

typedef struct ht_node ht_node_t;

struct ht_node {
    void* key;
    void* value;
    ht_node_t* next;
};

typedef struct hashtable {
    size_t size;
    size_t n_values;
    compare_t compare;
    hash_t hash;
    ht_node_t** table;
} hashtable_t;

/**** PUBLIC ****/

/*
 * Function: ht_create
 * --------------------
 *  Creates a new hashtable.
 * 
 *  size: Initial size of the hashtable.
 *  cmp: Function pointer to compare two keys.
 *  hash: Function pointer to hash a key.
 * 
 *  returns: Pointer to the new hashtable.
 */
hashtable_t* ht_create(size_t size, compare_t compare, 
                            hash_t hash);
/*
 * Function: ht_insert
 * --------------------
 *  Inserts key and value into ht, overwites value if key already exists.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Key to insert.
 *  value: Value to insert.
 * 
 *  returns: Nothing.
 */
void ht_insert(hashtable_t* ht, void* key, void* value);

/*
 * Function: ht_search
 * --------------------
 *  Searches for a key in ht.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Key to search for.
 * 
 *  returns: Value associated with key, NULL if key not found.
 */
void* ht_search(hashtable_t* ht, void* key);

/*
 * Function: ht_get_index
 * --------------------
 *  Gets the index of a key in ht.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Key to get index of.
 * 
 *  returns: Index of key.
 */
size_t ht_get_index(hashtable_t* ht, void* key);

/*
 * Function: ht_get_node
 * --------------------
 *  Gets the node of a key in ht.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Key to get node of.
 * 
 *  returns: Node of key.
 */
ht_node_t* ht_get_node(hashtable_t* ht, void* key);

/*
 * Function: ht_get_key
 * --------------------
 *  Gets the key of a node in ht.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Node to get key of.
 * 
 *  returns: Key of node.
 */
void* ht_get_key(hashtable_t* ht, void* key);

/*
 * Function: ht_contains
 * --------------------
 *  Checks if a key is in ht.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Key to check for.
 * 
 *  returns: True if key is in ht, false otherwise.
 */
bool ht_contains(hashtable_t* ht, void* key);

/*
 * Function: ht_unique_insert
 * --------------------
 *  Inserts only if key doesn't exist in ht.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Key to insert.
 *  value: Value to insert.
 * 
 *  returns: True if key was inserted, false otherwise.
 */
bool ht_unique_insert(hashtable_t* ht, void* key, void* value);

/*
 * Function: ht_remove
 * --------------------
 *  Removes a node from ht.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Key to remove.
 *  free_key: Function to free key.
 *  free_value: Function to free value.
 * 
 *  returns: Nothing.
 */
void ht_remove(hashtable_t* ht, void* key, 
                free_ht_t free_key, free_ht_t free_value);

/*
 * Function: ht_reset
 * --------------------
 *  Resets ht.
 * 
 *  ht: Pointer to the hashtable.
 *  free_key: Function to free key.
 *  free_value: Function to free value.
 * 
 *  returns: Nothing.
 */
void ht_reset(hashtable_t* ht, free_ht_t free_key, free_ht_t free_value);

/*
 * Function: ht_clean
 * --------------------
 *  Cleans ht.
 * 
 *  ht: Pointer to the hashtable.
 *  free_key: Function to free key.
 *  free_value: Function to free value.
 * 
 *  returns: Nothing.
 */
void ht_clean(hashtable_t* ht, free_ht_t free_keys, free_ht_t free_values);


/* COUNTER HT */
/*
 * Function: ht_insert_count
 * --------------------
 *  Inserts a key with a count value into ht, if it already exists, 
 *  updates its count.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Key to insert.
 * 
 *  returns: Count.
 */
size_t ht_insert_count(hashtable_t* ht, void* key);


/*
 * Function: ht_get_count
 * --------------------
 *  Gets the count of a key in ht.
 * 
 *  ht: Pointer to the hashtable.
 *  key: Key to get count from.
 * 
 *  returns: Count.
 */
size_t ht_get_count(hashtable_t* ht, void* key);

/**** PRIVATE ****/
/*
 * Function: _needs_resize
 * --------------------
 *  Checks if ht needs to be resized.
 * 
 *  ht: Pointer to the hashtable.
 * 
 *  returns: True if ht needs to be resized, false otherwise.
 */
bool _needs_resize(hashtable_t* ht);

/*
 * Function: _resize_ht
 * --------------------
 *  Resizes ht.
 * 
 *  ht: Pointer to the hashtable.
 * 
 *  returns: Nothing.
 */
void _resize_ht(hashtable_t* ht);

/*
 * Function: _copy_ht
 * --------------------
 *  Efficiently copies all nodes from ht to new_table.
 * 
 *  new_table: Pointer to the new table.
 *  src_ht: Pointer to the source hashtable.
 *  new_size: Size of the new table.
 * 
 *  returns: Nothing.
 */
void _copy_ht(ht_node_t** new_table, hashtable_t* src_ht, size_t new_size);

/*
 * Function: _ht_copy_insert
 * --------------------
 *  Transfers a source node using a reference into a new table.
 * 
 *  new_size: Size of the new table.
 *  hash: Hash function.
 *  new_table: Pointer to the new table.
 *  node: Pointer to the source node.
 * 
 *  returns: Nothing.
 */
void _ht_copy_insert(size_t new_size, hash_t hash, ht_node_t** new_table, 
                ht_node_t* node);

/*
 * Function: _initialise_table
 * --------------------
 *  Initialises a table.
 * 
 *  table: Pointer to the table.
 *  size: Size of the table.
 * 
 *  returns: Nothing.
 */
void _initialise_table(ht_node_t** table, size_t size);

#endif