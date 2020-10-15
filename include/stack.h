

#define DEFAULT_STACK_SIZE 128

typedef struct Stack{
    void *valueArray;
    int valueSize;
    int arrayUsedSize;
    int arrayAllocSize;
    void *top;
} Stack;

