#ifndef DATASTRUCTURE
#define DATASTRUCTURE

typedef enum eType
{
	INT,
	FLOAT,
	VOID,
	ARRAY,
	FUNCTION,
	ERROR_TYPE,
	NULL_TYPE
}eType;

static const char* TYPE_STRING[] =
{
	"INT",
	"FLOAT",
	"VOID",
	"ARRAY",
	"FUNCTION",
	"ERROR_TYPE",
	"NULL_TYPE"
};

typedef enum eRole
{
	VARIABLE,
	FUNC_PRE_DEFINITION,
	FUNC_FULL_DEFINITION

}eRole;

static const char* ROLE_STRING[] =
{
	"Variable",
	"FuncPreDefinition",
	"FuncFullDefinition"
};

typedef struct FuncParam
{
	char* paramName;
	eType paramType;
	int* arrayDimensions; // for array
	int arrayDimensionsCounter;
}FuncParam;

typedef struct Data
{
	char* name;
	eType type;
	eRole role;
	int* arrayDimensions; // for array
	int arrayDimensionsCounter;
	struct FuncParam* listOfParamsTypes; // for function
	int paramsCounter;
} Data;

#endif
