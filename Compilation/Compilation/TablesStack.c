#include "TablesStack.h"

void pushToStack(StackOfSymbolTables* stack, SymbolTable* table)
{
	NodeOfStack *tempNode = (NodeOfStack*)malloc(sizeof(NodeOfStack));
	tempNode->symbolTable = table;
	tempNode->nextNode = stack->head;
	stack->head = tempNode;
}

SymbolTable* popFromStack(StackOfSymbolTables* stack)
{
	NodeOfStack *tempNode = stack->head;
	stack->head = stack->head->nextNode;
	return tempNode->symbolTable;
}

int isStackEmpty(StackOfSymbolTables* stack)
{
	if (stack->head == NULL)
		return 1;
	else
		return 0;
}

SymbolTable* top(StackOfSymbolTables* stack)
{
	return stack->head->symbolTable;
}

StackOfSymbolTables* createStack()
{
	StackOfSymbolTables* stackToReturn = (StackOfSymbolTables*)malloc(sizeof(StackOfSymbolTables));
	stackToReturn->head = NULL;
	return stackToReturn;
}

void pushAllItemFromFirstStackToSecondStack(StackOfSymbolTables* firstStack, StackOfSymbolTables* secondStack)
{
	while (!isStackEmpty(firstStack))
	{
		pushToStack(secondStack, popFromStack(firstStack));
	}
}