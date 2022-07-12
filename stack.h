#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdbool.h>

typedef void (* free_stack_t)(void*);
typedef void* (* get_next_t)(void*);

typedef struct stack_node stack_node_t;

struct stack_node {
    void* data;
    stack_node_t* next;
};

typedef struct stack {
    stack_node_t* head;
} stack_t;

/*
 * Function: stack_create
 * --------------------
 *  Creates a new stack.
 * 
 *  No parameters.
 * 
 *  returns: Pointer to the new stack.
 */
stack_t* stack_create(void);

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
stack_t* stack_create_from_array(void** node_array, size_t n_nodes);

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
stack_t* stack_create_from_linkedlist(void* node, get_next_t get_next);

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
void stack_push(stack_t* stack, void* data);

/*
 * Function: stack_peek
 * --------------------
 *  Peeks the top node from the stack.
 * 
 *  stack: Pointer to the stack.
 * 
 *  returns: Pointer to the data of the top node.
 */
void* stack_peek(stack_t* stack);

/*
 * Function: stack_pop
 * --------------------
 *  Pops the top node from the stack.
 * 
 *  stack: Pointer to the stack.
 * 
 *  returns: Pointer to the data of the popped node.
 */
void* stack_pop(stack_t* stack);

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
void stack_pop_free(stack_t* stack, free_stack_t free_data);

/*
 * Function: stack_is_empty
 * --------------------
 *  Checks if the stack is empty.
 * 
 *  stack: Pointer to the stack.
 * 
 *  returns: True if stack is empty, false otherwise.
 */
bool stack_is_empty(stack_t* stack);

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
void stack_clean(stack_t* stack, free_stack_t free_data);

#endif