#ifndef Stack
#define Stack

#include <stdio.h>
#include "SymbolTable.h"

typedef struct NodeOfStack
{
	struct SymbolTable* symbolTable;
	struct NodeOfStack* nextNode;
}NodeOfStack;

typedef struct StackOfSymbolTables
{
	struct NodeOfStack* head;
}StackOfSymbolTables;


StackOfSymbolTables* createStack();
void pushToStack(StackOfSymbolTables* stack, SymbolTable* table);
SymbolTable* popFromStack(StackOfSymbolTables* stack);
void pushAllItemFromFirstStackToSecondStack(StackOfSymbolTables* firstStack, StackOfSymbolTables* secondStack);
SymbolTable* top(StackOfSymbolTables* stack);
int isStackEmpty(StackOfSymbolTables* stack);

#endif
