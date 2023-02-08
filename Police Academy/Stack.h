// This program was made by Michael Mospan (z5419614) on 5/08/2022
// This file is an implementation of the Stack ADT Interface

#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>

#include "Agent.h"

typedef struct stackNode *StackNode;

// Struct for the nodes stored in the stack
struct stackNode {
    int cityID;
    int cityLength;
    StackNode next;
};

/**
 * Recursively frees the given stack
 */
void StackFree(StackNode stack);

/**
 * Pushes the given city onto the stack
 */
void StackPush(StackNode *stack, int cityID, int cityLength);

/**
 * Pops a city from the stack
 */
struct stackNode StackPop(StackNode *stack);

#endif