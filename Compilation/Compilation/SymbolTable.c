#include "SymbolTable.h"

#define SIZE 5000

int returnHashCode(int size, int key)
{
	if (key < 0)
	{
		while (key < 0)
		{
			key += size;
		}
	}
	else
	{
		return key % size;
	}
	return key;
}

SymbolTable* createSymbolTable()
{
	SymbolTable* tempTable = (SymbolTable*)malloc(sizeof(SymbolTable));
	tempTable->contentSize = SIZE;
	tempTable->content = (DataItem*)calloc(SIZE, sizeof(DataItem));
	tempTable->Keys = NULL;
	tempTable->contentCounter = 0;
	return tempTable;
}

DataItem* searchInCurrentSymbolTableAndReturnDataItem(SymbolTable* symbolTable, int key)
{
	int hashIndex = returnHashCode((*symbolTable).contentSize, key);

	while ((*symbolTable).content[hashIndex].data != NULL)
	{
		if ((*symbolTable).content[hashIndex].key == key)
		{
			symbolTable->content[hashIndex].isUsed = 1;
			return &(symbolTable->content[hashIndex]);
		}
		++hashIndex;
		hashIndex %= (*symbolTable).contentSize;
	}
	return NULL;
}

void insertToSymbolTable(SymbolTable* symbolTable, DataItem* dataItem)
{

	int hashIndex = returnHashCode((*symbolTable).contentSize, dataItem->key);
	while ((*symbolTable).content[hashIndex].data != NULL)
	{
		++hashIndex;
		hashIndex %= (*symbolTable).contentSize;
	}
	DataItem* currentDataItem = &((*symbolTable).content[hashIndex]);
	currentDataItem->data = (Data*)malloc(sizeof(Data));
	currentDataItem->key = dataItem->key;
	currentDataItem->data->name = dataItem->data->name;
	currentDataItem->data->arrayDimensions = (int*)calloc(20, sizeof(int));
	currentDataItem->data->arrayDimensions = dataItem->data->arrayDimensions;
	currentDataItem->data->arrayDimensionsCounter = dataItem->data->arrayDimensionsCounter;
	currentDataItem->data->listOfParamsTypes = (FuncParam*)calloc(20, sizeof(FuncParam));	
	currentDataItem->data->listOfParamsTypes = dataItem->data->listOfParamsTypes;
	currentDataItem->data->paramsCounter = dataItem->data->paramsCounter;
	currentDataItem->data->type = dataItem->data->type;
	currentDataItem->data->role = dataItem->data->role;
	currentDataItem->isUsed = 0;
	symbolTable->Keys = realloc(symbolTable->Keys, (++symbolTable->contentCounter) * sizeof(int));
	symbolTable->Keys[symbolTable->contentCounter - 1] = currentDataItem->key;
}

int StringToInt(const char* s) {
	int h = 0;
	while (*s) {
		h = 31 * h + (*s++);
	}
	return h;
}

int checkUnusedVariable(SymbolTable* symbolTable)
{
	int i = 0;
	int hashIndex;
	int UnusedVar = 0;
	int counter = 0;

	for (i = 0; i < symbolTable->contentCounter; i++)
	{
		hashIndex = returnHashCode((*symbolTable).contentSize, symbolTable->Keys[i]);
		if((*symbolTable).content[hashIndex].data != NULL)
		{
			counter++;

			if ((*symbolTable).content[hashIndex].isUsed == 0)
			{
				if (UnusedVar == 0 && (*symbolTable).content[hashIndex].data->role == VARIABLE)
				{
					fprintf(semanticOutput, "\n\t unUsed Variable in this scope :");
				}
				else
				{
					fprintf(semanticOutput, ",");
				}
				UnusedVar++; 
				fprintf(semanticOutput, " { %s } ", (*symbolTable).content[hashIndex].data->name);
			}

		}
	}
	return UnusedVar;
}