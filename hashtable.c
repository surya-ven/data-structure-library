/*
Author : Surya Venkatesh
Purpose: This file is a custom hashtable library with efficient growing. 
*/

#include "hashtable.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

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
hashtable_t* ht_create(size_t size, compare_t compare, hash_t hash) {
    assert(compare);
    assert(hash);

    hashtable_t* ht = malloc(sizeof(hashtable_t));
    assert(ht);

    // Initialise hashtable
    ht->table = malloc(sizeof(ht_node_t*) * size);
    assert(ht->table);
    _initialise_table(ht->table, size);

    // Initialise hashtable parameters
    ht->n_values = 0;
    ht->size = size;
    ht->compare = compare;
    ht->hash = hash;

    return ht;
}

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
void ht_insert(hashtable_t* ht, void* key, void* value) {
    assert(ht);
    assert(key);
    size_t index = 0;
    ht_node_t* node = NULL;

    // Get index of key
    index = ht_get_index(ht, key);

    // Check if key already exists
    if ((node = ht_get_node(ht, key))) {
        node->value = value;
        return;
    }

    // Create new node
    node = malloc(sizeof(ht_node_t));
    assert(node);
    node->key = key;
    node->value = value;
    node->next = ht->table[index];
    ht->table[index] = node;

    ht->n_values++;

    // Check if hashtable needs to be resized
    if (_needs_resize(ht)) {
        _resize_ht(ht);
    }
}

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
void* ht_search(hashtable_t* ht, void* key) {
    assert(ht);
    assert(key);
    ht_node_t* node = NULL;

    // Get node if key exists
    node = ht_get_node(ht, key);

    // Key found
    if (node) {
        return node->value;
    }
    // Key not found
    return NULL;
}

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
size_t ht_get_index(hashtable_t* ht, void* key) {
    assert(ht);
    assert(key);
    size_t hash = 0, index = 0;
    
    // Use hash to determine index
    hash = ht->hash(key);
    index = hash % ht->size;

    return index;
}

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
ht_node_t* ht_get_node(hashtable_t* ht, void* key) {
    assert(ht);
    assert(key);
    size_t index = 0;

    index = ht_get_index(ht, key);

    // Traverse through bucket list and check if key exists, return it if found
    for (ht_node_t* node = ht->table[index]; node; node = node->next) {
        if (ht->compare(node->key, key) == 0) {
            return node;
        }
    }

    return NULL;
}

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
void* ht_get_key(hashtable_t* ht, void* key) {
    assert(ht);
    assert(key);
    ht_node_t* node = NULL;

    // Get node if key exists
    if ((node = ht_get_node(ht, key))) {
        return node->key;
    }
    // Key not found
    return NULL;
}

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
bool ht_contains(hashtable_t* ht, void* key) {
    assert(ht);
    assert(key);

    // Check if key exists
    if (ht_get_node(ht, key)) {
        return true;
    }
    // Key not found
    return false;
}

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
bool ht_unique_insert(hashtable_t* ht, void* key, void* value) {
    assert(ht);
    assert(key);

    // Check if key exists
    if (ht_contains(ht, key)) {
        return false;
    }

    // Insert key if it doesn't exist
    ht_insert(ht, key, value);
    return true;
}

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
                free_ht_t free_key, free_ht_t free_value) {
    assert(ht);
    assert(key);
    ht_node_t* node = NULL, * next_node = NULL;
    void* old_key, * old_value;

    // Get node if key exists
    if ((node = ht_get_node(ht, key))) {
        next_node = node->next;
        old_key = node->key;
        old_value = node->value;

        // Remove node from bucket list
        if (node->next) {
            node->key = next_node->key;
            node->value = next_node->value;
            node->next = next_node->next;
        } else {
            node->key = NULL;
            node->value = NULL;
            node->next = NULL;
        }

        // Free key if needed
        if (free_key && old_key) {
            free_key(old_key);
        }
        
        // Free value if needed
        if (free_value && old_value) {
            free_value(old_value);
        }

        if (next_node) {
            free(next_node);
        }
        
        ht->n_values--;
    }

    // Note: Table is not shrinked
}

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
void ht_reset(hashtable_t* ht, free_ht_t free_key, free_ht_t free_value) {
    assert(ht);
    ht_node_t* node = NULL, * next = NULL;
    
    // Free all nodes and reset them to NULL
    for (size_t i = 0; i < ht->size; i++) {
        node = ht->table[i];

        while (node) {
            next = node->next;

            // Free key if needed
            if (free_key) {
                free_key(node->key);
            }

            // Free value if needed
            if (free_value) {
                free_value(node->value);
            }

            free(node);
            node = next;
        }

        ht->table[i] = NULL;
    }

    ht->n_values = 0;
}

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
void ht_clean(hashtable_t* ht, free_ht_t free_key, free_ht_t free_value) {
    assert(ht);
    ht_node_t* node = NULL, * next = NULL;
    
    // Free all nodes and destroy them
    for (size_t i = 0; i < ht->size; i++) {
        node = ht->table[i];
        while (node) {
            next = node->next;

            // Free key if needed
            if (free_key) {
                free_key(node->key);
            }

            // Free value if needed
            if (free_value) {
                free_value(node->value);
            }

            free(node);
            node = next;
        }
    }

    free(ht->table);
    free(ht);
}

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
size_t ht_insert_count(hashtable_t* ht, void* key) {
    ht_node_t* node = ht_get_node(ht, key);

    // Key exists, update count
    if (node) {
        *(size_t*)(node->value) = *(size_t*)(node->value) + 1;
        return *(size_t*)(node->value);
    } 
    // Key doesn't exist, insert it
    else {
        size_t* start_value = malloc(sizeof(size_t));
        assert(start_value);
        *start_value = 1;
        ht_insert(ht, key, start_value);
        return *start_value;
    }
}

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
size_t ht_get_count(hashtable_t* ht, void* key) {
    void* count = NULL;
    if ((count = ht_search(ht, key))) {
        return *(size_t*)count;
    } else {
        return 0;
    }
}

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
bool _needs_resize(hashtable_t* ht) {
    // Check if table surpasses load factor
    if (ht->n_values >= ht->size * MAX_LOAD_FACTOR) {
        return true;
    }
    return false;
}

/*
 * Function: _resize_ht
 * --------------------
 *  Resizes ht.
 * 
 *  ht: Pointer to the hashtable.
 * 
 *  returns: Nothing.
 */
void _resize_ht(hashtable_t* ht) {
    size_t new_size = ht->size * GROWTH_FACTOR;

    // Allocate new table
    ht_node_t** new_table = malloc(sizeof(ht_node_t*) * new_size);
    assert(new_table);
    _initialise_table(new_table, new_size);

    // Efficient copy and rehash all nodes
    _copy_ht(new_table, ht, new_size);
    ht->size = new_size;

    // Free old table
    free(ht->table);
    ht->table = new_table;
}

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
void _copy_ht(ht_node_t** new_table, hashtable_t* src_ht, size_t new_size) {
    ht_node_t* next_node = NULL;

    // Transfer all existing nodes into new table without duplicating data
    for (size_t i = 0; i < src_ht->size; i++) {
        for (ht_node_t* node = src_ht->table[i]; node; node = next_node) {
            next_node = node->next;
            _ht_copy_insert(new_size, src_ht->hash, new_table, node);
        }
    }
}

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
                ht_node_t* node) {
    assert(new_table && node);
    size_t index = 0, hash_result = 0;

    // Get index of new node
    hash_result = hash(node->key);
    index = hash_result % new_size;

    // Insert node into new table
    node->next = new_table[index];
    new_table[index] = node;
}

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
void _initialise_table(ht_node_t** table, size_t size) {
    for (size_t i = 0; i < size; i++) {
        table[i] = NULL;
    }
}