/*
Author : Surya Venkatesh
Purpose: This file is a custom stack library.
*/

#include "stack.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

/*
 * Function: stack_create
 * --------------------
 *  Creates a new stack.
 * 
 *  No parameters.
 * 
 *  returns: Pointer to the new stack.
 */
stack_t* stack_create(void) {
    stack_t* stack = malloc(sizeof(stack_t));
    assert(stack);
    stack->head = NULL;
    return stack;
}

/*
 * Function: stack_create_from_array
 * --------------------
 *  Creates a new stack from given array.
 * 
 *  node_array: Array to create stack from.
 *  n_nodes: Number of nodes in array.
 * 
 *  returns: Pointer to the new stack.
 */
stack_t* stack_create_from_array(void** node_array, size_t n_nodes) {
    stack_t* stack = malloc(sizeof(stack_t));
    assert(stack);
    stack->head = NULL;

    // Create stack nodes from array
    for (size_t i = 0; i < n_nodes; i++) {
        stack_push(stack, node_array[i]);
    }

    return stack;
}

/*
 * Function: stack_create_from_linkedlist
 * --------------------
 *  Creates a new stack from a given linked list.
 * 
 *  node: Pointer to the head of the linked list.
 *  get_next: Function pointer to get the next node.
 * 
 *  returns: Pointer to the new stack.
 */
stack_t* stack_create_from_linkedlist(void* node, get_next_t get_next) {
    stack_t* stack = malloc(sizeof(stack_t));
    assert(stack);
    stack->head = NULL;

    // Create stack nodes from linked list
    while (node) {
        stack_push(stack, node);
        node = get_next(node);
    }

    return stack;
}

/*
 * Function: stack_push
 * --------------------
 *  Pushes a new node to the stack.
 * 
 *  stack: Pointer to the stack.
 *  data: Data to insert.
 * 
 *  returns: Nothing.
 */
void stack_push(stack_t* stack, void* data) {
    assert(stack);
    stack_node_t* node = malloc(sizeof(stack_node_t));
    assert(node);
    node->data = data;
    node->next = stack->head;
    stack->head = node;
}

/*
 * Function: stack_pop
 * --------------------
 *  Pops the top node from the stack.
 * 
 *  stack: Pointer to the stack.
 * 
 *  returns: Pointer to the data of the popped node.
 */
void* stack_pop(stack_t* stack) {
    assert(stack);
    void* data = NULL;
    stack_node_t* node = stack->head;

    // Store data and remove node
    if (node) {
        data = node->data;
        stack->head = node->next;
        free(node);
    }

    return data;
}

/*
 * Function: stack_peek
 * --------------------
 *  Peeks the top node from the stack.
 * 
 *  stack: Pointer to the stack.
 * 
 *  returns: Pointer to the data of the top node.
 */
void* stack_peek(stack_t* stack) {
    assert(stack);
    if (!(stack->head)) {
        return NULL;
    }
    return stack->head->data;
}

/*
 * Function: stack_pop_free
 * --------------------
 *  Pops the top node from the stack and frees the data if appropriate 
 *  function is given.
 * 
 *  stack: Pointer to the stack.
 *  free_data: Function pointer to free the data.
 * 
 *  returns: Nothing.
 */
void stack_pop_free(stack_t* stack, free_stack_t free_data) {
    assert(stack);
    stack_node_t* node = stack->head;
    stack->head = node->next;

    // Free data if function is given
    if (free_data) {
        free_data(node->data);
    }

    free(node);
}

/*
 * Function: stack_is_empty
 * --------------------
 *  Checks if the stack is empty.
 * 
 *  stack: Pointer to the stack.
 * 
 *  returns: True if stack is empty, false otherwise.
 */
bool stack_is_empty(stack_t* stack) {
    assert(stack);
    return !(stack->head);
}

/*
 * Function: stack_clean
 * --------------------
 *  Cleans the stack.
 * 
 *  stack: Pointer to the stack.
 *  free_data: Function pointer to free the data.
 * 
 *  returns: Nothing.
 */
void stack_clean(stack_t* stack, free_stack_t free_data) {
    assert(stack);
    while (!stack_is_empty(stack)) {
        stack_pop_free(stack, free_data);
    }
    free(stack);
}