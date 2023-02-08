// This program was made by Michael Mospan (z5419614) on 5/08/2022
// This file is an implementation of the Stack ADT

#include <stdio.h>
#include <stdlib.h>

#include "Stack.h"
#include "Agent.h"

// Frees the given stack
void StackFree(StackNode stack) {
    if (stack == NULL) {
        return;
    }
    
    // If the next item on the stack isn't null
    // call itself recursively to free
    if (stack->next != NULL) {
        StackFree(stack->next);
    }

    free(stack);
}

// Pushes a city onto the stack
void StackPush(StackNode *stack, int cityID, int cityLength) {
    // Initialises the node values
    StackNode node = malloc(sizeof(struct stackNode));
    node->next = NULL;
    node->cityID = cityID;
    node->cityLength = cityLength;

    // If the stack isn't null, shift the stack to the end of
    // the new node
    if (*stack != NULL) {
        node->next = *stack;
    }

    // Start the stack at the node
    *stack = node;
}

// Pops a city from the stack
struct stackNode StackPop(StackNode *stack) {
    StackNode freeNode = *stack;
    *stack = freeNode->next;

    // Dereferences the freeNode before freeing
    struct stackNode byValue = *freeNode;
    free(freeNode);

    return byValue;
}