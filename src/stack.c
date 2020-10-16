#include "stack.h"

#include <stdlib.h>
Stack *initStack();
/*stackSize取0为默认大小*/
Stack *initStack(int typeSize, int stackSize, assignFunction assign) {
    Stack *stack = malloc(sizeof(Stack));
    if (stackSize == 0) {
        stackSize = DEFAULT_STACK_SIZE;
    }
    stack->arrayAllocSize = stackSize;
    stack->arrayUsedSize = 0;
    stack->valueTypeSize = typeSize;
    stack->valueArray = malloc(stack->valueTypeSize * stack->arrayAllocSize);
    stack->assign = assign;
    return stack;
}

void freeStack(Stack *stack) {
    if (stack->arrayAllocSize != 0) {
        free(stack->valueArray);
    }
    free(stack);
}

int stackPush(Stack *stack, void *value) {
    if (stack->arrayAllocSize == stack->arrayUsedSize) {
        stack->arrayAllocSize *= 2;
        void *tmp = realloc(stack->valueArray, stack->valueTypeSize * stack->arrayAllocSize);
        if(tmp == NULL){
            return -1;
        }
        stack->valueArray = tmp;
    }
    stack->assign(&(((unsigned char *)(stack->valueArray))[stack->arrayUsedSize * stack->valueTypeSize]), value);
    stack->arrayUsedSize++;
    return 0;
}

int stackPop(Stack *stack) {
    if (stack->arrayUsedSize > 0) {
        stack->arrayUsedSize--;
        return 0;
    } else {
        return -1;
    }
}

void *stackGetTop(Stack *stack) {
    if (stack->arrayUsedSize > 0) {
        return &(((unsigned char *)(stack->valueArray))[(stack->arrayUsedSize - 1) * stack->valueTypeSize]);
    } else {
        return NULL;
    }
}