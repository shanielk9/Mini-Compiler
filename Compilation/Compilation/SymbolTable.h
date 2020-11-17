#ifndef HashTableFile
#define HashTableFile

#include <stdio.h>
#include "Token.h"
#include <stdlib.h>
#include "DataStructure.h"

typedef struct DataItem
{
	struct Data* data;
	int key;
	int isUsed;

}DataItem;

typedef struct SymbolTable
{
	struct DataItem* content;
	int contentSize;
	int* Keys;
	int contentCounter;
}SymbolTable;

int StringToInt(const char* str);
int returnHashCode(int size, int key);
SymbolTable* createSymbolTable();
void insertToSymbolTable(SymbolTable* currentSymbolTable, DataItem* data);
DataItem* searchInCurrentSymbolTableAndReturnDataItem(SymbolTable* currentHashTable, int key);
int checkUnusedVariable(SymbolTable* symbolTable);

#endif 
