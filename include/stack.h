

#define DEFAULT_STACK_SIZE 128
typedef void (*assignFunction)(void * assignedValue, void * fromValue);
typedef struct Stack {
    void *valueArray;
    int valueTypeSize;
    int arrayUsedSize;
    int arrayAllocSize;

    assignFunction assign;/*void (*assign)(void *assignedValue, void *fromValue);*/
} Stack;
Stack *initStack(int typeSize, int stackSize, assignFunction assign);
void freeStack(Stack *stack);
int stackPush(Stack *stack, void *value);
int stackPop(Stack *stack);
void *stackGetTop(Stack *stack);