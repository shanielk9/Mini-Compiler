#include <stdlib.h>
#include "Parser.h"
#define ARRAY_DIMENSIONS_SIZE 20
#define FUNC_PARAMS_SIZE 20

Token* currentToken;
StackOfSymbolTables* tablesStack;
DataItem* tempDataItem;
FuncParam* tempFunctionParam;
Token *currentCommandLine;
int sizeOfTokensInCommandLine = 0;
int funcPreDefinitionCounter = 0;
int semanticErrorsCounter = 0;
int doublePrint = 0;
int returnStmtCounter = 0;

void printSemanticErrors(Token* token, eSemanticErrors errorType)
{
	semanticErrorsCounter++;
	switch (errorType)
	{
	case UNDEFINED:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: Undefined - this ID doesn't declared.", token->lineNumber);
		break;
	case REDECLERATION_VAR:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: Redecleration - this variable has been declare before.", token->lineNumber);
		break;
	case REDECLERATION_FUNC:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: Redecleration - this function has been declare before.", token->lineNumber);
		break;
	case REDEFINITION:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: Redifintion - this function has been implemented before.", token->lineNumber);
		break;
	case TYPE_MISMATCH:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: Type Mismatch - mismatch between the left side type and right side type.", token->lineNumber);
		break;
	case INVALID_ARRAY_INDEX_TYPE:
		break;
	case REDECLERATION_PARAMS:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: Redecleration - this param has been declare before on the func declaration.", token->lineNumber);
		break;
	case FUNC_FULL_DEF_MISMATCH:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: The full defintion of the function doesn't match the pre definition. function's signature has change.", token->lineNumber);
		break;
	case FUNC_ASSINGMENT_ERROR:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: Invalid function assignment.", token->lineNumber);
		break;
	case GENERIC_MSG:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: Incorrect use of variable. ", token->lineNumber);
		break;
	case FUNCTION_ID:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. The ID of this function dosen't exist, therfore can't has a return type.", token->lineNumber);
		break;
	case INCORRECT_RETURN_TYPE:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. The return type from function doesn't match. ", token->lineNumber);
		break;
	case ERROR_TYPE_ASSIGNMENT:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Cannot Asssing to error type.", token->lineNumber);
		break;
	case INCORRECT_ARRAY_ASSIGNMENT:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Incorrect assignment to array.", token->lineNumber);
		break;
	case UNIMPLEMENT_PRE_DEFINITION_FUNC:
		fprintf(semanticOutput, "\n< Semantic Error >. There is function that has only pre definition and not full definition.");
		break;
	case RETURN_STMT_ERROR:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Missing at least one currect return statement in function.", token->lineNumber);
		break;
	default:
		fprintf(semanticOutput, "\n< Semantic Error in line: %d >. Description: Unknown error.", token->lineNumber);
		break;
	}
}

void parse_PROG()
{
	tablesStack = createStack();
	SymbolTable* symbolTableToPushToStack = createSymbolTable();
	pushToStack(tablesStack, symbolTableToPushToStack);
	semanticErrorsCounter = 0;
	funcPreDefinitionCounter = 0;

	fprintf(syntacticOutput, "Rule (PROG -> GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS) \n");
	parse_GLOBAL_VARS();
	parse_FUNC_PREDEFS();
	parse_FUNC_FULL_DEFS();

	if (funcPreDefinitionCounter > 0)
	{
		printSemanticErrors(currentToken, UNIMPLEMENT_PRE_DEFINITION_FUNC);
	}

	int UnUsedVar = checkUnusedVariable(symbolTableToPushToStack);
	fprintf(semanticOutput, "\n\nFound: %d semantic errors in file.\n", semanticErrorsCounter);
	popFromStack(tablesStack);

}

void parse_GLOBAL_VARS()
{
	fprintf(syntacticOutput, "Rule (GLOBAL_VARS -> VAR_DEC GLOBAL_VARS_tag) \n");
	parse_VAR_DEC();
	parse_GLOBAL_VARS_tag();
}

void parse_GLOBAL_VARS_tag()
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case KEYWORD_FLOAT_token:
	case KEYWORD_INT_token:
	{
		currentToken = next_token();
		if (currentToken->kind == ID_token)
		{
			Token * current_token_temp = next_token();
			switch (current_token_temp->kind)
			{
			case SEPERATION_SIGN_SEMI_COLON_token:
			case SEPERATION_SIGN_LEFT_BRACKETS_token:
			{
				back_token();
				back_token();
				back_token();
				fprintf(syntacticOutput, "Rule (GLOBAL_VARS_tag -> VAR_DEC GLOBAL_VARS_tag) \n");
				parse_VAR_DEC();
				parse_GLOBAL_VARS_tag();

			}break;

			case SEPERATION_SIGN_LEFT_PARENTHESES_token:
			{
				back_token();
				back_token();
				back_token();

			}break;

			default:
				break;
			}

		}
		else
		{
			fprintf(syntacticOutput, "Rule (GLOBAL_VARS_tag -> EPSILON ) \n");
			back_token();
		}
	}break;

	case KEYWORD_VOID_token:
	{
		fprintf(syntacticOutput, "Rule (GLOBAL_VARS_tag -> EPSILON ) \n");
		back_token();
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'KEYWORD_INT_token' , 'KEYWORD_FLOAT_token' , 'KEYWORD_VOID_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != KEYWORD_INT_token &&
			currentToken->kind != KEYWORD_FLOAT_token &&
			currentToken->kind != KEYWORD_VOID_token)
		{
			currentToken = next_token();
		}
		back_token();
	}
	}
}

void parse_VAR_DEC()
{
	tempDataItem = calloc(1, sizeof(DataItem));
	tempDataItem->data = (Data*)calloc(1, sizeof(Data));
	tempDataItem->data->role = VARIABLE;
	fprintf(syntacticOutput, "Rule (VAR_DEC -> TYPE id VAR_DEC_tag) \n");
	parse_TYPE("VAR_DEC");
	match(ID_token);
	currentToken = getCurrentToken();
	tempDataItem->data->name = currentToken->lexeme;
	tempDataItem->key = StringToInt(tempDataItem->data->name);
	parse_VAR_DEC_tag();
}

void parse_VAR_DEC_tag()
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case SEPERATION_SIGN_SEMI_COLON_token:
	{
		fprintf(syntacticOutput, "Rule (VAR_DEC_tag -> ;) \n");

		if (tempDataItem != NULL)
		{
			insertToSymbolTableIfIdIsValid(0);
		}
	}break;

	case SEPERATION_SIGN_LEFT_BRACKETS_token:
	{
		fprintf(syntacticOutput, "Rule (VAR_DEC_tag -> [ DIM_SIZES ] ;) \n");
		tempDataItem->data->arrayDimensions = (int*)calloc(ARRAY_DIMENSIONS_SIZE, sizeof(int));
		tempDataItem->data->arrayDimensionsCounter = 0;
		parse_DIM_SIZES();
		match(SEPERATION_SIGN_RIGHT_BRACKETS_token);
		match(SEPERATION_SIGN_SEMI_COLON_token);
		if (tempDataItem != NULL)
		{
			insertToSymbolTableIfIdIsValid(0);
		}
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_SEMI_COLON_token' , 'SEPERATION_SIGN_LEFT_BRACKETS_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != ID_token &&
			currentToken->kind != SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token &&
			currentToken->kind != KEYWORD_IF_token &&
			currentToken->kind != KEYWORD_RETURN_token &&
			currentToken->kind != KEYWORD_INT_token &&
			currentToken->kind != KEYWORD_FLOAT_token &&
			currentToken->kind != KEYWORD_VOID_token)
		{
			currentToken = next_token();
		}
		back_token();
	}
	}
}

void parse_TYPE(char* callFunc)
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case KEYWORD_FLOAT_token:
	{
		if (tempDataItem->data->role == VARIABLE)
			tempDataItem->data->type = FLOAT;
		else
		{
			if (callFunc == "RETURNED_TYPE")
				tempDataItem->data->type = FLOAT;
			else
				tempFunctionParam->paramType = FLOAT;
		}

		fprintf(syntacticOutput, "Rule (TYPE -> float) \n");
	}break;

	case KEYWORD_INT_token:
	{
		if (tempDataItem->data->role == VARIABLE)
			tempDataItem->data->type = INT;
		else
		{
			if (callFunc == "RETURNED_TYPE")
				tempDataItem->data->type = INT;
			else
				tempFunctionParam->paramType = INT;
		}

		fprintf(syntacticOutput, "Rule (TYPE -> int) \n");
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'KEYWORD_INT_token' , 'KEYWORD_FLOAT_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != ID_token)
		{
			currentToken = next_token();
		}
		back_token();
		return ERROR_TYPE;
	}
	}
}

void parse_DIM_SIZES()
{
	fprintf(syntacticOutput, "Rule (DIM_SIZES ->  int_num DIM_SIZES_tag) \n");
	match(INT_NUM_token);
	currentToken = getCurrentToken();
	if (tempDataItem->data->role == VARIABLE)
	{
		tempDataItem->data->arrayDimensions[tempDataItem->data->arrayDimensionsCounter] = atoi(currentToken->lexeme);
		tempDataItem->data->arrayDimensionsCounter++;
	}
	else
	{
		tempFunctionParam->arrayDimensions[tempFunctionParam->arrayDimensionsCounter] = atoi(currentToken->lexeme);
		tempFunctionParam->arrayDimensionsCounter++;

	}
	parse_DIM_SIZES_tag();
}

void parse_DIM_SIZES_tag()
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case SEPERATION_SIGN_COMMA_token:
	{
		fprintf(syntacticOutput, "Rule (DIM_SIZES_tag -> , int_num DIM_SIZES_tag) \n");
		match(INT_NUM_token);
		currentToken = getCurrentToken();
		if (tempDataItem->data->role == VARIABLE)
		{
			tempDataItem->data->arrayDimensions[tempDataItem->data->arrayDimensionsCounter] = atoi(currentToken->lexeme);
			tempDataItem->data->arrayDimensionsCounter++;
		}
		else
		{
			tempFunctionParam->arrayDimensions[tempFunctionParam->arrayDimensionsCounter] = atoi(currentToken->lexeme);
			tempFunctionParam->arrayDimensionsCounter++;

		}
		parse_DIM_SIZES_tag();
	}break;

	case SEPERATION_SIGN_RIGHT_BRACKETS_token:
	{
		fprintf(syntacticOutput, "Rule (DIM_SIZES_tag -> EPSILON) \n");
		back_token();
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_COMMA_token' , 'SEPERATION_SIGN_RIGHT_BRACKETS_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_BRACKETS_token)
		{
			currentToken = next_token();
		}
		back_token();
	}
	}
}

void parse_FUNC_PREDEFS()
{
	fprintf(syntacticOutput, "Rule (FUNC_PREDEFS -> FUNC_PROTOTYPE; FUNC_PREDEFS_tag) \n");
	tempDataItem->data->arrayDimensions = (int*)calloc(ARRAY_DIMENSIONS_SIZE, sizeof(int));
	tempDataItem->data->arrayDimensionsCounter = 0;
	tempDataItem->data->role = FUNC_PRE_DEFINITION;
	parse_FUNC_PROTOTYPE();
	match(SEPERATION_SIGN_SEMI_COLON_token);
	if (tempDataItem != NULL)
	{
		insertToSymbolTableIfIdIsValid(1);
	}
	parse_FUNC_PREDEFS_tag();
}

void parse_FUNC_PREDEFS_tag()
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case KEYWORD_INT_token:
	case KEYWORD_FLOAT_token:
	case KEYWORD_VOID_token:
	{
		currentToken = next_token();
		if (currentToken->kind == ID_token)
		{
			back_token();
			back_token();
			parse_FUNC_PROTOTYPE();
			tempDataItem->data->role = FUNC_PRE_DEFINITION;
			Token * current_token_temp = next_token();
			switch (current_token_temp->kind)
			{

			case SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token:
			{
				while (current_token_temp->kind != SEPERATION_SIGN_SEMI_COLON_token)
					current_token_temp = back_token();
				fprintf(syntacticOutput, "Rule (FUNC_PREDEFS_tag -> EPSILON ) \n");

			}break;

			case SEPERATION_SIGN_SEMI_COLON_token:
			{
				fprintf(syntacticOutput, "Rule (FUNC_PREDEFS_tag -> FUNC_PROTOTYPE; FUNC_PREDEFS_tag) \n");
				tempDataItem->data->role = FUNC_PRE_DEFINITION;
				if (tempDataItem != NULL)
				{
					insertToSymbolTableIfIdIsValid(1);
				}
				parse_FUNC_PREDEFS_tag();
			}break;

			default:
				break;
			}
		}
		else
		{
			fprintf(syntacticOutput, "Rule (FUNC_PREDEFS_tag -> EPSILON ) \n");
			back_token();
		}
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'KEYWORD_INT_token' , 'KEYWORD_FLOAT_token' , 'KEYWORD_VOID_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != KEYWORD_VOID_token &&
			currentToken->kind != KEYWORD_FLOAT_token &&
			currentToken->kind != KEYWORD_INT_token)
		{
			currentToken = next_token();
		}
		back_token();
	}
	}
}

Token* parse_FUNC_PROTOTYPE()
{
	Token* returnIDToken;
	fprintf(syntacticOutput, "Rule (FUNC_PROTOTYPE -> RETURNED_TYPE id (PARAMS)) \n");
	parse_RETURNED_TYPE();
	match(ID_token);
	currentToken = getCurrentToken();
	returnIDToken = getCurrentToken();
	tempDataItem->data->name = currentToken->lexeme;
	tempDataItem->key = StringToInt(tempDataItem->data->name);
	match(SEPERATION_SIGN_LEFT_PARENTHESES_token);
	tempFunctionParam = calloc(1, sizeof(FuncParam));
	tempFunctionParam->arrayDimensions = (int*)calloc(ARRAY_DIMENSIONS_SIZE, sizeof(int));
	tempFunctionParam->arrayDimensionsCounter = 0;
	parse_PARAMS();
	match(SEPERATION_SIGN_RIGHT_PARENTHESES_token);
	return returnIDToken;
}

void parse_FUNC_FULL_DEFS()
{
	fprintf(syntacticOutput, "Rule (FUNC_FULL_DEFS -> FUNC_WITH_BODY FUNC_FULL_DEFS_tag) \n");
	tempDataItem->data->arrayDimensions = (int*)calloc(ARRAY_DIMENSIONS_SIZE, sizeof(int));
	tempDataItem->data->arrayDimensionsCounter = 0;
	tempDataItem->data->role = FUNC_FULL_DEFINITION;
	parse_FUNC_WITH_BODY();
	parse_FUNC_FULL_DEFS_tag();
}

void parse_FUNC_FULL_DEFS_tag()
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case KEYWORD_FLOAT_token:
	case KEYWORD_INT_token:
	case KEYWORD_VOID_token:
	{
		fprintf(syntacticOutput, "Rule (FUNC_FULL_DEFS_tag -> FUNC_WITH_BODY FUNC_FULL_DEFS_tag) \n");
		back_token();
		parse_FUNC_WITH_BODY();
		parse_FUNC_FULL_DEFS_tag();
	}break;

	case EOF_token:
	{
		fprintf(syntacticOutput, "Rule (FUNC_FULL_DEFS_tag -> EPSILON ) \n");
		back_token();
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'EOF_token','KEYWORD_INT_token' , 'KEYWORD_FLOAT_token' , 'KEYWORD_VOID_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token)
		{
			currentToken = next_token();
		}
		back_token();

	}
	}
}

void parse_FUNC_WITH_BODY()
{
	fprintf(syntacticOutput, "Rule (FUNC_WITH_BODY -> FUNC_PROTOTYPE COMP_STMT) \n");
	tempDataItem->data->role = FUNC_FULL_DEFINITION;
	Token* idToken;
	returnStmtCounter = 0;
	idToken = parse_FUNC_PROTOTYPE();
	checkPreDefAndFullDefMatch();
	parse_COMP_STMT(1, idToken);
	DataItem* di = searchInAllSymbolTablesAndReturnDataItem(StringToInt(idToken->lexeme));
	if (di != NULL)
	{
		if (di->data->type != VOID)
		{
			if (returnStmtCounter == 0)
			{
				printSemanticErrors(getCurrentToken(), RETURN_STMT_ERROR);
			}
		}
	}
}

void parse_RETURNED_TYPE()
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case KEYWORD_VOID_token:
	{
		fprintf(syntacticOutput, "Rule (RETURNED_TYPE -> void ) \n");
		tempDataItem->data->type = VOID;
	}break;

	case KEYWORD_INT_token:
	case KEYWORD_FLOAT_token:
	{
		fprintf(syntacticOutput, "Rule (RETURNED_TYPE -> TYPE ) \n");
		back_token();
		parse_TYPE("RETURNED_TYPE");
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'KEYWORD_VOID_token' , 'KEYWORD_FLOAT_token' , 'KEYWORD_INT_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != ID_token)
		{
			currentToken = next_token();
		}
		back_token();
	}
	}
}

void parse_PARAMS()
{
	currentToken = next_token();
	tempDataItem->data->listOfParamsTypes = (FuncParam*)calloc(FUNC_PARAMS_SIZE, sizeof(FuncParam));
	tempDataItem->data->paramsCounter = 0;


	switch (currentToken->kind)
	{
	case KEYWORD_FLOAT_token:
	case KEYWORD_INT_token:
	{
		fprintf(syntacticOutput, "Rule (PARAMS -> PARAMS_LIST) \n");
		tempFunctionParam = calloc(1, sizeof(FuncParam));
		tempFunctionParam->arrayDimensions = (int*)calloc(ARRAY_DIMENSIONS_SIZE, sizeof(int));
		tempFunctionParam->arrayDimensionsCounter = 0;
		back_token();
		parse_PARAMS_LIST();
	}break;

	case SEPERATION_SIGN_RIGHT_PARENTHESES_token:
	{
		fprintf(syntacticOutput, "Rule (PARAMS -> EPSILON) \n");
		back_token();
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_RIGHT_PARENTHESES_token' , 'KEYWORD_INT_token' , 'KEYWORD_FLOAT_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
		{
			currentToken = next_token();
		}
		back_token();
	}
	}
}

void parse_PARAMS_LIST()
{
	fprintf(syntacticOutput, "Rule (PARAMS_LIST -> PARAM PARAMS_LIST_tag) \n");
	parse_PARAM();
	parse_PARAMS_LIST_tag();
}

void parse_PARAMS_LIST_tag()
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case SEPERATION_SIGN_COMMA_token:
	{
		fprintf(syntacticOutput, "Rule (PARAMS_LIST_tag ->, PARAM PARAMS_LIST_tag) \n");
		tempDataItem->data->listOfParamsTypes[tempDataItem->data->paramsCounter] = *tempFunctionParam;
		tempDataItem->data->paramsCounter++;
		parse_PARAM();
		parse_PARAMS_LIST_tag();
	}break;

	case SEPERATION_SIGN_RIGHT_PARENTHESES_token:
	{
		fprintf(syntacticOutput, "Rule (PARAMS_LIST_tag -> EPSILON) \n");
		tempDataItem->data->listOfParamsTypes[tempDataItem->data->paramsCounter] = *tempFunctionParam;
		tempDataItem->data->paramsCounter++;
		back_token();
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_RIGHT_PARENTHESES_token','SEPERATION_SIGN_COMMA_token' , at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
		{
			currentToken = next_token();
		}

		back_token();
	}
	}
}

void parse_PARAM()
{
	fprintf(syntacticOutput, "Rule (PARAM -> TYPE id PARAM_tag) \n");
	tempFunctionParam = calloc(1, sizeof(FuncParam));
	tempFunctionParam->arrayDimensions = (int*)calloc(ARRAY_DIMENSIONS_SIZE, sizeof(int));
	tempFunctionParam->arrayDimensionsCounter = 0;
	parse_TYPE("PARAM");
	match(ID_token);
	currentToken = getCurrentToken();
	tempFunctionParam->paramName = currentToken->lexeme;
	parse_PARAM_tag();
}

void parse_PARAM_tag()
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case SEPERATION_SIGN_LEFT_BRACKETS_token:
	{
		fprintf(syntacticOutput, "Rule (PARAM_tag -> [DIM_SIZES] ) \n");
		parse_DIM_SIZES();
		match(SEPERATION_SIGN_RIGHT_BRACKETS_token);
	}break;

	case SEPERATION_SIGN_COMMA_token:
	case SEPERATION_SIGN_RIGHT_PARENTHESES_token:
	{
		fprintf(syntacticOutput, "Rule (PARAM_tag -> EPSILON ) \n");
		back_token();
	}break;
	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_LEFT_BRACKETS_token' , 'SEPERATION_SIGN_COMMA_token' , 'SEPERATION_SIGN_RIGHT_PARENTHESES_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_COMMA_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
		{
			currentToken = next_token();
		}

		back_token();
	}
	}
}

void parse_COMP_STMT(int IsFunc, Token* idToken)
{
	fprintf(syntacticOutput, "Rule ( COMP_STMT -> {VAR_DEC_LIST STMT_LIST} ) \n");
	match(SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token);
	SymbolTable* symbolTableToPushToStack = createSymbolTable();
	pushToStack(tablesStack, symbolTableToPushToStack);
	if (IsFunc)
	{
		insertFuncParamsToSymbolTable();
	}
	parse_VAR_DEC_LIST();
	parse_STMT_LIST(idToken);
	match(SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token);
	int UnUsedVar = checkUnusedVariable(symbolTableToPushToStack);
	popFromStack(tablesStack);
}

void parse_VAR_DEC_LIST()
{
	currentToken = next_token();
	switch (currentToken->kind)
	{
	case KEYWORD_INT_token:
	case KEYWORD_FLOAT_token:
	{
		fprintf(syntacticOutput, "Rule (VAR_DEC_LIST -> VAR_DEC VAR_DEC_LIST) \n");
		back_token();
		parse_VAR_DEC();
		parse_VAR_DEC_LIST();
	}break;


	case ID_token:
	case SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token:
	case KEYWORD_IF_token:
	case KEYWORD_RETURN_token:
	{
		fprintf(syntacticOutput, "Rule (VAR_DEC_LIST -> EPSILON ) \n");
		back_token();
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'KEYWORD_INT_token' , 'KEYWORD_FLOAT_token' , 'ID_token' , 'SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token', 'KEYWORD_IF_token', 'KEYWORD_RETURN_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != ID_token &&
			currentToken->kind != SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token &&
			currentToken->kind != KEYWORD_IF_token &&
			currentToken->kind != KEYWORD_RETURN_token)
		{
			currentToken = next_token();
		}

		back_token();
	}
	}

}

void parse_STMT_LIST(Token* idToken)
{
	fprintf(syntacticOutput, "Rule (STMT_LIST -> STMT STMT_LIST_tag) \n");
	parse_STMT(idToken);
	parse_STMT_LIST_tag(idToken);
}

void parse_STMT_LIST_tag(Token* idToken)
{
	currentToken = next_token();

	switch (currentToken->kind)
	{

	case SEPERATION_SIGN_SEMI_COLON_token:
	{
		fprintf(syntacticOutput, "Rule (STMT_LIST_tag -> ; STMT STMT_LIST_tag) \n");
		parse_STMT(idToken);
		parse_STMT_LIST_tag(idToken);
	}break;

	case SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token:
	{
		fprintf(syntacticOutput, "Rule (STMT_LIST_tag -> EPSILON) \n");
		back_token();
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_SEMI_COLON_token' , 'SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);
		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token)
		{
			currentToken = next_token();
		}

		back_token();
	}
	}
}

void parse_STMT(Token* idToken)
{
	currentToken = next_token();
	DataItem* currentID = calloc(1, sizeof(DataItem));
	switch (currentToken->kind)
	{
	case SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token:
	{
		fprintf(syntacticOutput, "Rule (STMT -> COMP_STMT) \n");
		back_token();
		parse_COMP_STMT(0, idToken);

	}break;

	case KEYWORD_IF_token:
	{
		fprintf(syntacticOutput, "Rule (STMT -> if (CONDITION) STMT) \n");
		match(SEPERATION_SIGN_LEFT_PARENTHESES_token);
		parse_CONDITION();
		match(SEPERATION_SIGN_RIGHT_PARENTHESES_token);
		parse_STMT(idToken);
	}break;

	case KEYWORD_RETURN_token:
	{
		fprintf(syntacticOutput, "Rule (STMT -> RETURN_STMT) \n");
		back_token();
		parse_RETURN_STMT(idToken);
	}break;

	case ID_token:
	{
		fprintf(syntacticOutput, "Rule (STMT -> id STMT_tag) \n");
		currentCommandLine = NULL;
		sizeOfTokensInCommandLine = 0;
		assignCurrentTokenToCommands(getCurrentToken());
		currentToken = getCurrentToken();
		currentID = searchInAllSymbolTablesAndReturnDataItem(StringToInt(currentToken->lexeme));
		/*if (currentID == NULL)
		{
			printSemanticErrors(currentToken, UNDEFINED);
		}*/
		parse_STMT_tag(currentID);

	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token' , 'KEYWORD_IF_token' , 'KEYWORD_RETURN_token' , 'ID_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_SEMI_COLON_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token)
		{
			currentToken = next_token();
		}
		back_token();
	}
	}
}

void parse_STMT_tag(DataItem* idDataItem)
{
	currentToken = next_token();
	switch (currentToken->kind)
	{
	case SEPERATION_SIGN_LEFT_BRACKETS_token:
	case ASSIGNMENT_OP_token:
	{
		eType firstType;
		eType secType;
		fprintf(syntacticOutput, "Rule (STMT_tag -> VAR_tag = EXPR) \n");
		back_token();
		firstType = parse_VAR_tag();
		match(ASSIGNMENT_OP_token);
		assignCurrentTokenToCommands(getCurrentToken());
		secType = parse_EXPR(0);
		checkAssignment();
		checkFinalAssignmentAndPrintError(firstType, secType, idDataItem); /// for the sec option of checking;
	}break;

	case SEPERATION_SIGN_LEFT_PARENTHESES_token:
	{
		fprintf(syntacticOutput, "Rule (STMT_tag -> (ARGS)) \n");
		assignCurrentTokenToCommands(getCurrentToken());
		parse_ARGS();
		match(SEPERATION_SIGN_RIGHT_PARENTHESES_token);
		assignCurrentTokenToCommands(getCurrentToken());
		checkFunctionCall();
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_LEFT_BRACKETS_token' , 'ASSIGNMENT_OP_token' , 'SEPERATION_SIGN_LEFT_PARENTHESES_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);

		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_SEMI_COLON_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token)
		{
			currentToken = next_token();
		}

		back_token();
	}
	}
}

void parse_IF_STMT(Token* idToken)
{
	fprintf(syntacticOutput, "Rule (IF_STMT -> if (CONDITION) STMT) \n");
	match(KEYWORD_IF_token);
	match(SEPERATION_SIGN_LEFT_PARENTHESES_token);
	parse_CONDITION();
	match(SEPERATION_SIGN_RIGHT_PARENTHESES_token);
	parse_STMT(idToken);
}

void parse_CALL()
{
	fprintf(syntacticOutput, "Rule (CALL -> id (ARGS)) \n");
	match(ID_token);
	match(SEPERATION_SIGN_LEFT_PARENTHESES_token);
	parse_ARGS();
	match(SEPERATION_SIGN_RIGHT_PARENTHESES_token);
}

eType parse_ARGS()
{
	Token * current_token = next_token();
	eType typeToReturn;

	switch (current_token->kind)
	{
	case FLOAT_NUM_token:
	case INT_NUM_token:
	case SEPERATION_SIGN_LEFT_PARENTHESES_token:
	case ID_token:
	{
		fprintf(syntacticOutput, "Rule (ARGS -> ARG_LIST) \n");
		back_token();
		typeToReturn = parse_ARG_LIST();
		return typeToReturn;
	}break;

	case SEPERATION_SIGN_RIGHT_PARENTHESES_token:
	{
		fprintf(syntacticOutput, "Rule (ARGS -> EPSILON) \n");
		back_token();
		return NULL_TYPE;
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'FLOAT_NUM_token' , 'INT_NUM_token','ID_token' , 'SEPERATION_SIGN_RIGHT_PARENTHESES_token' , 'SEPERATION_SIGN_LEFT_PARENTHESES_token' at line %d ", current_token->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[current_token->kind], current_token->lexeme);
		while (current_token->kind != EOF_token && current_token->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
		{
			current_token = next_token();
		}

		back_token();
	}
	}
}

eType parse_ARG_LIST()
{
	eType firstType;
	eType secType;
	fprintf(syntacticOutput, "Rule (ARG_LIST -> EXPR ARG_LIST_tag) \n");
	firstType = parse_EXPR(0);
	secType = parse_ARG_LIST_tag();
	return FUNCTION;
}

eType parse_ARG_LIST_tag()
{
	Token * current_token = next_token();

	switch (current_token->kind)
	{
	case SEPERATION_SIGN_COMMA_token:
	{
		eType firstType;
		eType secType;
		assignCurrentTokenToCommands(getCurrentToken());
		fprintf(syntacticOutput, "Rule (ARG_LIST_tag -> , EXPR ARG_LIST_tag) \n");
		firstType = parse_EXPR(0);
		secType = parse_ARG_LIST_tag();
		return checkListArrayIsInt(firstType, secType);

	}break;
	case SEPERATION_SIGN_RIGHT_PARENTHESES_token:
	{
		fprintf(syntacticOutput, "Rule (ARG_LIST_tag -> EPSILON) \n");
		back_token();
		return NULL_TYPE;
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_COMMA_token', 'SEPERATION_SIGN_RIGHT_PARENTHESES_token' at line %d ", current_token->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[current_token->kind], current_token->lexeme);
		while (current_token->kind != EOF_token && current_token->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
		{
			current_token = next_token();
		}

		back_token();
	}
	}
}

void parse_RETURN_STMT(Token* idToken)
{
	fprintf(syntacticOutput, "Rule (RETURN_STMT -> return RETURN_STMT_tag) \n");
	match(KEYWORD_RETURN_token);
	parse_RETURN_STMT_tag(idToken);
}

void parse_RETURN_STMT_tag(Token* idToken)
{
	Token * current_token = next_token();

	switch (current_token->kind)
	{
	case FLOAT_NUM_token:
	case INT_NUM_token:
	case SEPERATION_SIGN_LEFT_PARENTHESES_token:
	case ID_token:
	{
		fprintf(syntacticOutput, "Rule (RETURN_STMT_tag  -> EXPR) \n");
		back_token();
		eType returnType;
		returnType = parse_EXPR(0);
		checkIfReturnTypeMatch(returnType, idToken);
	}break;

	case SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token:
	case SEPERATION_SIGN_SEMI_COLON_token:
	{
		fprintf(syntacticOutput, "Rule (RETURN_STMT_tag -> EPSILON) \n");
		back_token();
		checkIfReturnTypeMatch(VOID, idToken);
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'FLOAT_NUM_token' , 'INT_NUM_token' , 'ID_token' , 'SEPERATION_LEFT_RIGHT_PARENTHESES_token' , 'SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token' , 'SEPERATION_SIGN_SEMI_COLON_token' at line %d ", current_token->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[current_token->kind], current_token->lexeme);
		while (current_token->kind != EOF_token
			&& current_token->kind != SEPERATION_SIGN_SEMI_COLON_token
			&& current_token->kind != SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token)
		{
			current_token = next_token();
		}

		back_token();
	}
	}
}

void parse_VAR()
{
	fprintf(syntacticOutput, "Rule (VAR -> id VAR_tag) \n");
	match(ID_token);
	parse_VAR_tag();
}

eType parse_VAR_tag()
{
	Token * current_token = next_token();
	eType typeToReturn;

	switch (current_token->kind)
	{
	case SEPERATION_SIGN_LEFT_BRACKETS_token:
	{
		assignCurrentTokenToCommands(getCurrentToken());
		fprintf(syntacticOutput, "Rule (VAR_tag -> [EXPR_LIST]) \n");
		typeToReturn = parse_EXPR_LIST();
		match(SEPERATION_SIGN_RIGHT_BRACKETS_token);
		assignCurrentTokenToCommands(getCurrentToken());
		return typeToReturn;
	}break;

	case REL_OP_EQUAL_token:
	case REL_OP_ABOVE_EQUAL_token:
	case REL_OP_BELOW_EQUAL_token:
	case REL_OP_ABOVE_token:
	case REL_OP_NOT_EQUAL_token:
	case REL_OP_BELOW_token:
	case AR_OP_PlUS_token:
	case SEPERATION_SIGN_COMMA_token:
	case SEPERATION_SIGN_SEMI_COLON_token:
	case SEPERATION_SIGN_RIGHT_PARENTHESES_token:
	case ASSIGNMENT_OP_token:
	case SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token:
	case SEPERATION_SIGN_RIGHT_BRACKETS_token:
	case AR_OP_MUL_token:
	{
		fprintf(syntacticOutput, "Rule (VAR_tag -> EPSILON) \n");
		back_token();
		return NULL_TYPE;
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'ASSIGNMENT_OP_token' , 'SEPERATION_SIGN_LEFT_PARENTHESES_token' , 'SEPERATION_SIGN_LEFT_BRACKETS_token' , 'AR_OP_MUL_token' , 'AR_OP_PlUS_token' , 'SEPERATION_SIGN_RIGHT_BRACKETS_token' , 'SEPERATION_SIGN_COMMA_token' , 'SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token' , 'SEPERATION_SIGN_RIGHT_PARENTHESES_token', 'SEPERATION_SIGN_SEMI_COLON_token' , 'REL_OP_ABOVE_EQUAL_token', 'REL_OP_ABOVE_token', 'REL_OP_BELOW_EQUAL_token', 'REL_OP_EQUAL_token', 'REL_OP_NOT_EQUAL_token' , 'REL_OP_BELOW_token' at line %d", current_token->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[current_token->kind], current_token->lexeme);
		while (current_token->kind != EOF_token && current_token->kind != REL_OP_ABOVE_EQUAL_token && current_token->kind != REL_OP_ABOVE_token
			&& current_token->kind != REL_OP_BELOW_EQUAL_token && current_token->kind != REL_OP_BELOW_token
			&& current_token->kind != REL_OP_EQUAL_token && current_token->kind != REL_OP_NOT_EQUAL_token
			&& current_token->kind != AR_OP_MUL_token && current_token->kind != AR_OP_PlUS_token
			&& current_token->kind != SEPERATION_SIGN_COMMA_token && current_token->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token
			&& current_token->kind != SEPERATION_SIGN_RIGHT_BRACKETS_token && current_token->kind != SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token
			&& current_token->kind != SEPERATION_SIGN_SEMI_COLON_token && current_token->kind != REL_OP_ABOVE_EQUAL_token && current_token->kind != ASSIGNMENT_OP_token)
		{
			current_token = next_token();
		}

		back_token();
	}
	}
}

eType parse_EXPR_LIST()
{
	eType firstType;
	eType secType;
	fprintf(syntacticOutput, "Rule (EXPR_LIST -> EXPR EXPR_LIST_tag) \n");
	firstType = parse_EXPR(0);
	secType = parse_EXPR_LIST_tag();
	return checkListArrayIsInt(firstType, secType);
}

eType parse_EXPR_LIST_tag()
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case SEPERATION_SIGN_COMMA_token:
	{
		assignCurrentTokenToCommands(getCurrentToken());
		eType firstType;
		eType secType;
		fprintf(syntacticOutput, "Rule (EXPR_LIST_tag -> , EXPR EXPR_LIST_tag) \n");
		firstType = parse_EXPR(0);
		secType = parse_EXPR_LIST_tag();
		return checkListArrayIsInt(firstType, secType);
	}break;

	case SEPERATION_SIGN_RIGHT_BRACKETS_token:
	{
		fprintf(syntacticOutput, "Rule (EXPR_LIST_tag -> EPSILON) \n");
		back_token();
		return NULL_TYPE;
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_RIGHT_BRACKETS_token','SEPERATION_SIGN_COMMA_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);
		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_BRACKETS_token)
		{
			currentToken = next_token();
		}

		back_token();
	}
	}
}

void parse_CONDITION()
{
	currentToken = next_token();

	fprintf(syntacticOutput, "Rule (CONDITION -> EXPR rel_op EXPR) \n");
	back_token();
	parse_EXPR(1);

	currentToken = next_token();
	switch (currentToken->kind)
	{
	case REL_OP_ABOVE_EQUAL_token:
	case REL_OP_ABOVE_token:
	case REL_OP_BELOW_EQUAL_token:
	case REL_OP_BELOW_token:
	case REL_OP_EQUAL_token:
	case REL_OP_NOT_EQUAL_token:

	{
		parse_EXPR(1);
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'REL_OP_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);
	}break;
	}
}

eType parse_EXPR(int isCondition)
{
	eType firstType;
	eType secType;
	fprintf(syntacticOutput, "Rule (EXPR -> TERM EXPR_tag) \n");
	firstType = parse_TERM(isCondition);
	secType = parse_EXPR_tag(isCondition);
	return checkMulOrAddTypes(firstType, secType);
}

eType parse_EXPR_tag(int isCondition)
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case AR_OP_PlUS_token:
	{
		eType firstType;
		eType secType;
		fprintf(syntacticOutput, "Rule (EXPR_tag -> + TERM EXPR_tag) \n");
		firstType = parse_TERM(isCondition);
		secType = parse_EXPR_tag(isCondition);
		return checkMulOrAddTypes(firstType, secType);
	}break;

	case SEPERATION_SIGN_RIGHT_BRACKETS_token:
	case SEPERATION_SIGN_COMMA_token:
	case SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token:
	case SEPERATION_SIGN_RIGHT_PARENTHESES_token:
	case SEPERATION_SIGN_SEMI_COLON_token:
	case REL_OP_ABOVE_EQUAL_token:
	case REL_OP_ABOVE_token:
	case REL_OP_BELOW_EQUAL_token:
	case REL_OP_EQUAL_token:
	case REL_OP_NOT_EQUAL_token:
	case REL_OP_BELOW_token:
	{
		fprintf(syntacticOutput, "Rule (EXPR_tag -> EPSILON) \n");
		back_token();
		return NULL_TYPE;
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'AR_OP_PlUS_token' , 'SEPERATION_SIGN_RIGHT_BRACKETS_token' , 'SEPERATION_SIGN_COMMA_token' , 'SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token' , 'SEPERATION_SIGN_RIGHT_PARENTHESES_token', 'SEPERATION_SIGN_SEMI_COLON_token' , 'REL_OP_ABOVE_EQUAL_token', 'REL_OP_ABOVE_token', 'REL_OP_BELOW_EQUAL_token', 'REL_OP_EQUAL_token', 'REL_OP_NOT_EQUAL_token' , 'REL_OP_BELOW_token' at line %d", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);
		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_BRACKETS_token &&
			currentToken->kind != SEPERATION_SIGN_COMMA_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token &&
			currentToken->kind != SEPERATION_SIGN_SEMI_COLON_token &&
			currentToken->kind != REL_OP_ABOVE_EQUAL_token &&
			currentToken->kind != REL_OP_ABOVE_token &&
			currentToken->kind != REL_OP_BELOW_EQUAL_token &&
			currentToken->kind != REL_OP_EQUAL_token &&
			currentToken->kind != REL_OP_NOT_EQUAL_token &&
			currentToken->kind != REL_OP_BELOW_token)
		{
			currentToken = next_token();
		}

		back_token();
	}

	}
}

eType parse_TERM(int isCondition)
{
	eType firstType;
	eType SecType;
	fprintf(syntacticOutput, "Rule (TERM -> FACTOR TERM_tag) \n");
	firstType = parse_FACTOR(isCondition);
	SecType = parse_TERM_tag(isCondition);
	return checkMulOrAddTypes(firstType, SecType);
}

eType parse_TERM_tag(int isCondition)
{
	currentToken = next_token();

	switch (currentToken->kind)
	{
	case AR_OP_MUL_token:
	{
		eType firstType;
		eType secType;
		fprintf(syntacticOutput, "Rule (TERM_tag -> * FACTOR TERM_tag) \n");
		firstType = parse_FACTOR(isCondition);
		secType = parse_TERM_tag(isCondition);
		return checkMulOrAddTypes(firstType, secType);
	}break;

	case SEPERATION_SIGN_RIGHT_BRACKETS_token:
	case SEPERATION_SIGN_COMMA_token:
	case SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token:
	case SEPERATION_SIGN_RIGHT_PARENTHESES_token:
	case SEPERATION_SIGN_SEMI_COLON_token:
	case REL_OP_ABOVE_EQUAL_token:
	case REL_OP_ABOVE_token:
	case REL_OP_BELOW_EQUAL_token:
	case REL_OP_EQUAL_token:
	case REL_OP_NOT_EQUAL_token:
	case REL_OP_BELOW_token:
	case AR_OP_PlUS_token:
	{
		fprintf(syntacticOutput, "Rule (TERM_tag -> EPSILON) \n");
		back_token();
		return NULL_TYPE;
	}break;


	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'AR_OP_MUL_token' , 'AR_OP_PlUS_token' , 'SEPERATION_SIGN_RIGHT_BRACKETS_token' , 'SEPERATION_SIGN_COMMA_token' , 'SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token' , 'SEPERATION_SIGN_RIGHT_PARENTHESES_token', 'SEPERATION_SIGN_SEMI_COLON_token' , 'REL_OP_ABOVE_EQUAL_token', 'REL_OP_ABOVE_token', 'REL_OP_BELOW_EQUAL_token', 'REL_OP_EQUAL_token', 'REL_OP_NOT_EQUAL_token' , 'REL_OP_BELOW_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);
		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_BRACKETS_token &&
			currentToken->kind != SEPERATION_SIGN_COMMA_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token &&
			currentToken->kind != SEPERATION_SIGN_SEMI_COLON_token &&
			currentToken->kind != REL_OP_ABOVE_EQUAL_token &&
			currentToken->kind != REL_OP_ABOVE_token &&
			currentToken->kind != REL_OP_BELOW_EQUAL_token &&
			currentToken->kind != REL_OP_EQUAL_token &&
			currentToken->kind != REL_OP_NOT_EQUAL_token &&
			currentToken->kind != REL_OP_BELOW_token &&
			currentToken->kind != AR_OP_PlUS_token)
		{
			currentToken = next_token();
		}

		back_token();
	}
	}
}

eType parse_FACTOR(int isCondition)
{
	currentToken = next_token();
	eType typeToReturn;


	switch (currentToken->kind)
	{
	case INT_NUM_token:
	{
		fprintf(syntacticOutput, "Rule (FACTOR -> int_num) \n");
		assignCurrentTokenToCommands(getCurrentToken());
		return INT;
	}break;

	case FLOAT_NUM_token:
	{
		fprintf(syntacticOutput, "Rule (FACTOR -> float_num) \n");
		assignCurrentTokenToCommands(getCurrentToken());
		return FLOAT;
	}break;

	case SEPERATION_SIGN_LEFT_PARENTHESES_token:
	{
		fprintf(syntacticOutput, "Rule (FACTOR -> (EXPR)) \n");
		typeToReturn = parse_EXPR(0);
		match(SEPERATION_SIGN_RIGHT_PARENTHESES_token);
		return typeToReturn;
	}break;

	case ID_token:
	{
		fprintf(syntacticOutput, "Rule (FACTOR -> id FACTOR_tag) \n");
		DataItem* di = searchInAllSymbolTablesAndReturnDataItem(StringToInt(currentToken->lexeme));
		
		if (di == NULL)
		{
			if (isCondition == 1)
			{
				printSemanticErrors(getCurrentToken(), UNDEFINED);
			}
			assignCurrentTokenToCommands(getCurrentToken());
			return ERROR_TYPE;
		}
		else
		{
			assignCurrentTokenToCommands(getCurrentToken());
		}
		typeToReturn = parse_FACTOR_tag();
		if (typeToReturn != ERROR_TYPE)
		{
			return di->data->type;
		}
		else
		{
			return ERROR_TYPE;
		}
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_LEFT_PARENTHESES_token' , 'INT_NUM_token' , 'FLOAT_NUM_token', 'ID_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);
		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_BRACKETS_token &&
			currentToken->kind != SEPERATION_SIGN_COMMA_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token &&
			currentToken->kind != SEPERATION_SIGN_SEMI_COLON_token &&
			currentToken->kind != REL_OP_ABOVE_EQUAL_token &&
			currentToken->kind != REL_OP_ABOVE_token &&
			currentToken->kind != REL_OP_BELOW_EQUAL_token &&
			currentToken->kind != REL_OP_EQUAL_token &&
			currentToken->kind != REL_OP_NOT_EQUAL_token &&
			currentToken->kind != REL_OP_BELOW_token &&
			currentToken->kind != AR_OP_PlUS_token &&
			currentToken->kind != AR_OP_MUL_token)
		{
			currentToken = next_token();
		}

		back_token();
	}
	}

}

eType parse_FACTOR_tag()
{
	currentToken = next_token();
	eType typeToReturn;

	switch (currentToken->kind)
	{
	case SEPERATION_SIGN_LEFT_PARENTHESES_token:
	{
		fprintf(syntacticOutput, "Rule (FACTOR_tag -> (ARGS)) \n");
		assignCurrentTokenToCommands(getCurrentToken());
		typeToReturn = parse_ARGS();
		match(SEPERATION_SIGN_RIGHT_PARENTHESES_token);
		assignCurrentTokenToCommands(getCurrentToken());
		return typeToReturn;
	}break;

	case SEPERATION_SIGN_LEFT_BRACKETS_token:
	case SEPERATION_SIGN_RIGHT_BRACKETS_token:
	case SEPERATION_SIGN_COMMA_token:
	case SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token:
	case SEPERATION_SIGN_RIGHT_PARENTHESES_token:
	case SEPERATION_SIGN_SEMI_COLON_token:
	case REL_OP_ABOVE_EQUAL_token:
	case REL_OP_ABOVE_token:
	case REL_OP_BELOW_EQUAL_token:
	case REL_OP_EQUAL_token:
	case REL_OP_NOT_EQUAL_token:
	case REL_OP_BELOW_token:
	case AR_OP_PlUS_token:
	case AR_OP_MUL_token:
	{
		fprintf(syntacticOutput, "Rule (FACTOR_tag -> VAR_tag) \n");
		back_token();
		typeToReturn = parse_VAR_tag();
		return typeToReturn;
	}break;

	default:
	{
		fprintf(syntacticOutput, "Expected: one of tokens 'SEPERATION_SIGN_LEFT_PARENTHESES_token' , 'SEPERATION_SIGN_LEFT_BRACKETS_token' , 'AR_OP_MUL_token' , 'AR_OP_PlUS_token' , 'SEPERATION_SIGN_RIGHT_BRACKETS_token' , 'SEPERATION_SIGN_COMMA_token' , 'SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token' , 'SEPERATION_SIGN_RIGHT_PARENTHESES_token', 'SEPERATION_SIGN_SEMI_COLON_token' , 'REL_OP_ABOVE_EQUAL_token', 'REL_OP_ABOVE_token', 'REL_OP_BELOW_EQUAL_token', 'REL_OP_EQUAL_token', 'REL_OP_NOT_EQUAL_token' , 'REL_OP_BELOW_token' at line %d ", currentToken->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s' , lexeme “ %s ” \n", ENUM_strings[currentToken->kind], currentToken->lexeme);
		while (currentToken->kind != EOF_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_BRACKETS_token &&
			currentToken->kind != SEPERATION_SIGN_COMMA_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token &&
			currentToken->kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token &&
			currentToken->kind != SEPERATION_SIGN_SEMI_COLON_token &&
			currentToken->kind != REL_OP_ABOVE_EQUAL_token &&
			currentToken->kind != REL_OP_ABOVE_token &&
			currentToken->kind != REL_OP_BELOW_EQUAL_token &&
			currentToken->kind != REL_OP_EQUAL_token &&
			currentToken->kind != REL_OP_NOT_EQUAL_token &&
			currentToken->kind != REL_OP_BELOW_token &&
			currentToken->kind != AR_OP_PlUS_token &&
			currentToken->kind != AR_OP_MUL_token)
		{
			currentToken = next_token();
		}
		back_token();
	}
	}
}

void insertToSymbolTableIfIdIsValid(int IsPreDef)
{
	char* currentTypeOfVariable = NULL;
	if (tempDataItem->data->role == VARIABLE)
	{
		insertVariableType();

	}
	else
	{
		insertFunctionType(IsPreDef); 
	
	}
}

int insertVariableType()
{

	DataItem* dataItemThatReturnFromTheSymboltable;
	SymbolTable* tempSymbolTable = top(tablesStack);
	DataItem* currentDataItem = searchInCurrentSymbolTableAndReturnDataItem(tempSymbolTable, tempDataItem->key);
	if (currentDataItem != NULL)
	{
		printSemanticErrors(currentToken, REDECLERATION_VAR);
		return;
	}
	
	else
	{
		insertToSymbolTable(top(tablesStack), tempDataItem);
		
	}
	return;
}



int checkPreDefAndFullDefMatch() {
	int isMatch = 1;
	int counter = 0;
	SymbolTable* tempSymbolTable = top(tablesStack);
	DataItem* currentDataItem = searchInCurrentSymbolTableAndReturnDataItem(tempSymbolTable, tempDataItem->key);

	if (currentDataItem == NULL)
	{
		insertFunctionType(0);
	}
	else if (currentDataItem->data->role == FUNC_FULL_DEFINITION)
	{
		printSemanticErrors(currentToken, REDEFINITION);
		return;
	}
	else
	{
		if (currentDataItem->data->type != tempDataItem->data->type)
		{
			isMatch = 0;
			currentDataItem->data->type = tempDataItem->data->type;
		}

		if (currentDataItem->data->paramsCounter > tempDataItem->data->paramsCounter)
		{
			counter = currentDataItem->data->paramsCounter;
		}
		else
		{
			counter = tempDataItem->data->paramsCounter;
		}

		if (currentDataItem->data->paramsCounter != tempDataItem->data->paramsCounter)
		{
			isMatch = 0;
			currentDataItem->data->paramsCounter = tempDataItem->data->paramsCounter;
		}


		for (int i = 0; i < counter; i++)
		{
			if (currentDataItem->data->listOfParamsTypes[i].paramType != tempDataItem->data->listOfParamsTypes[i].paramType)
			{
				isMatch = 0;
				currentDataItem->data->listOfParamsTypes[i].paramType = tempDataItem->data->listOfParamsTypes[i].paramType;
			}
			if (currentDataItem->data->listOfParamsTypes[i].paramName != tempDataItem->data->listOfParamsTypes[i].paramName)
			{
				currentDataItem->data->listOfParamsTypes[i].paramName = tempDataItem->data->listOfParamsTypes[i].paramName;
			}

			int maxArrayDimSize;
			if (currentDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter > tempDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter)
			{
				maxArrayDimSize = currentDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter;
				if (tempDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter == 0)
				{
					tempDataItem->data->listOfParamsTypes[i].arrayDimensions = (int*)calloc(20, sizeof(int));
				}
			}
			else
			{
				maxArrayDimSize = tempDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter;
				if (currentDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter == 0)
				{
					currentDataItem->data->listOfParamsTypes[i].arrayDimensions = (int*)calloc(20, sizeof(int));
				}
			}

			if (currentDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter != tempDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter)
			{
				isMatch = 0;
				currentDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter = tempDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter;
			}

			for (int j = 0; j < maxArrayDimSize; j++)
			{
			
				if (currentDataItem->data->listOfParamsTypes[i].arrayDimensions[j] != tempDataItem->data->listOfParamsTypes[i].arrayDimensions[j])
				{
					isMatch = 0;
					currentDataItem->data->listOfParamsTypes[i].arrayDimensions[j] = tempDataItem->data->listOfParamsTypes[i].arrayDimensions[j];
				}
			}
		}


		if (isMatch == 0)
		{
			printSemanticErrors(currentToken, FUNC_FULL_DEF_MISMATCH);
		}

		currentDataItem->data->role = FUNC_FULL_DEFINITION;
		funcPreDefinitionCounter--;

		DataItem* checkDataItem = searchInCurrentSymbolTableAndReturnDataItem(tempSymbolTable, currentDataItem->key);

	}
}

void insertFuncParamsToSymbolTable()
{
	SymbolTable* tempSymbolTable = top(tablesStack);
	DataItem* d = calloc(1, sizeof(DataItem));

	for (int i = 0; i < tempDataItem->data->paramsCounter; i++)
	{
		d->data = (Data*)malloc(sizeof(Data));
		d->data->role = VARIABLE;
		d->data->name = tempDataItem->data->listOfParamsTypes[i].paramName;
		d->key = StringToInt(tempDataItem->data->listOfParamsTypes[i].paramName);
		d->data->arrayDimensions = (int*)calloc(20, sizeof(int));
		d->data->arrayDimensions = tempDataItem->data->listOfParamsTypes[i].arrayDimensions;
		d->data->arrayDimensionsCounter = tempDataItem->data->listOfParamsTypes[i].arrayDimensionsCounter;
		d->data->listOfParamsTypes = (FuncParam*)calloc(20, sizeof(FuncParam));
		d->data->paramsCounter = 0;
		d->data->type = tempDataItem->data->listOfParamsTypes[i].paramType;

		DataItem* currentDataItem = searchInCurrentSymbolTableAndReturnDataItem(tempSymbolTable, d->key);
		if (currentDataItem != NULL)
		{
			printSemanticErrors(currentToken, REDECLERATION_PARAMS);
		}
		else
		{
			insertToSymbolTable(top(tablesStack), d);
			
		}
	}
}

int insertFunctionType(int isPreDef)
{
	
	SymbolTable* tempSymbolTable = top(tablesStack);
	DataItem* currentDataItem = searchInCurrentSymbolTableAndReturnDataItem(tempSymbolTable, tempDataItem->key);
	if (currentDataItem != NULL && (currentDataItem->data->role == FUNC_PRE_DEFINITION || currentDataItem->data->role == VARIABLE))
	{
		printSemanticErrors(currentToken, REDECLERATION_FUNC);
		for (int i = 0; i < tempDataItem->data->paramsCounter; i++)
		{
			for (int j = i+1; j < tempDataItem->data->paramsCounter; j++)
			{
				if (i != j)
				{
					if (strcmp(tempDataItem->data->listOfParamsTypes[i].paramName, tempDataItem->data->listOfParamsTypes[j].paramName) == 0)
					{
						printSemanticErrors(currentToken, REDECLERATION_PARAMS);
					}
				}
			}
		}
		return;
	}
	else
	{
		int isParamNameExist = 0;
		for (int i = 0; i < tempDataItem->data->paramsCounter; i++)
		{
			for (int j = i+1; j < tempDataItem->data->paramsCounter; j++)
			{
				if (i != j)
				{
					if (strcmp(tempDataItem->data->listOfParamsTypes[i].paramName, tempDataItem->data->listOfParamsTypes[j].paramName) == 0)
					{
						printSemanticErrors(currentToken, REDECLERATION_PARAMS);
						isParamNameExist = 1;
					}
				}
			}
		}

		if (!isParamNameExist)
		{
			insertToSymbolTable(top(tablesStack), tempDataItem);
			if (isPreDef == 1)
			{
				funcPreDefinitionCounter++;
			}
			
		}

	}
	return;
}

void assignCurrentTokenToCommands(Token* CurrToken)
{
	currentCommandLine = realloc(currentCommandLine, (++sizeOfTokensInCommandLine) * sizeof(Token));
	if (CurrToken != NULL)
	{
		currentCommandLine[sizeOfTokensInCommandLine - 1].lexeme = CurrToken->lexeme;
		currentCommandLine[sizeOfTokensInCommandLine - 1].kind = CurrToken->kind;
		currentCommandLine[sizeOfTokensInCommandLine - 1].lineNumber = CurrToken->lineNumber;
	}
}

void checkAssignment()
{
	eType leftSide = ERROR_TYPE;
	DataItem* leftDataItem = searchInAllSymbolTablesAndReturnDataItem(StringToInt(currentCommandLine[0].lexeme));
	int i = 0;
	if (leftDataItem == NULL)
	{
		printSemanticErrors(currentToken, UNDEFINED);
	}
	else
	{
		if (leftDataItem->data->role != VARIABLE)
		{
			printSemanticErrors(currentToken, FUNC_ASSINGMENT_ERROR);
			return;
		}


		if (currentCommandLine[1].kind == SEPERATION_SIGN_LEFT_BRACKETS_token)
		{
			i++;
			int currentArrDimensionCounter = 1;
			while (currentCommandLine[i].kind != SEPERATION_SIGN_RIGHT_BRACKETS_token) //check if array dims not out of bounds
			{
				i++;
				if (currentArrDimensionCounter > leftDataItem->data->arrayDimensionsCounter)
				{
					if (leftDataItem->data->arrayDimensionsCounter != 0)
					{
						printSemanticErrors(currentToken, GENERIC_MSG);
						fprintf(semanticOutput, "The number of dimensions is too high.");
						return;
					}
					else
					{
						printSemanticErrors(currentToken, GENERIC_MSG);
						fprintf(semanticOutput, "Does not declare as array.");
						return;
					}
				}

				while (currentCommandLine[i].kind != SEPERATION_SIGN_COMMA_token && currentCommandLine[i].kind != SEPERATION_SIGN_RIGHT_BRACKETS_token)
				{
					if (currentCommandLine[i].kind == FLOAT_NUM_token)
					{
						printSemanticErrors(currentToken, GENERIC_MSG);
						fprintf(semanticOutput, "The array dimension can't be float.");
						return;
					}
					if (currentCommandLine[i].kind == INT_NUM_token
						&& (currentCommandLine[i + 1].kind == SEPERATION_SIGN_COMMA_token || currentCommandLine[i + 1].kind == SEPERATION_SIGN_RIGHT_BRACKETS_token)
						&& (currentCommandLine[i - 1].kind == SEPERATION_SIGN_COMMA_token || currentCommandLine[i - 1].kind == SEPERATION_SIGN_LEFT_BRACKETS_token))
					{
						if (atoi(currentCommandLine[i].lexeme) > (leftDataItem->data->arrayDimensions[currentArrDimensionCounter - 1]) - 1)
						{
							printSemanticErrors(currentToken, GENERIC_MSG);
							fprintf(semanticOutput, "Array assignment out of bounds.");
							return;
						}
					}
					if (currentCommandLine[i].kind == ID_token)
					{
						DataItem* currentIdItem = searchInAllSymbolTablesAndReturnDataItem(StringToInt(currentCommandLine[i].lexeme));
						if (currentIdItem == NULL)
						{
							printSemanticErrors(getCurrentToken(), UNDEFINED);
							fprintf(semanticOutput, "The lexeme: %s is undefined", currentCommandLine[i].lexeme);
							return;
						}
						if (currentIdItem->data->type != INT)
						{
							printSemanticErrors(currentToken, GENERIC_MSG);
							fprintf(semanticOutput, "lexeme: %s as array dimentions must be INT", currentCommandLine[i].lexeme);
							return;
						}
					}
					i++;
				}
				currentArrDimensionCounter++;
			}
			if (currentArrDimensionCounter - 1 < leftDataItem->data->arrayDimensionsCounter)
			{
				printSemanticErrors(currentToken, GENERIC_MSG);
				fprintf(semanticOutput, "The number of dimensions is too low.");
				return;
			}
		}
		if (currentCommandLine[1].kind == ASSIGNMENT_OP_token && leftDataItem->data->arrayDimensionsCounter > 0)
		{
			printSemanticErrors(currentToken, INCORRECT_ARRAY_ASSIGNMENT);
			return;
		}

		leftSide = leftDataItem->data->type;
	}

	i++; // move to "="
	i++; // move to next lexema;

	eType rightSide = ERROR_TYPE;
	DataItem* rightDataItem;
	for (i; i < sizeOfTokensInCommandLine; i++)
	{
		
		if (currentCommandLine[i].kind == ID_token)
		{
			rightDataItem = searchInAllSymbolTablesAndReturnDataItem(StringToInt(currentCommandLine[i].lexeme));
			if (rightDataItem == NULL)
			{
				printSemanticErrors(currentToken, UNDEFINED);
				return;
			}
			if (rightDataItem->data->type == FLOAT)
			{
				rightSide = FLOAT;
			}

			// check array assignment
			if (rightDataItem->data->arrayDimensionsCounter > 0)
			{
				if (currentCommandLine[i + 1].kind != SEPERATION_SIGN_LEFT_BRACKETS_token)
				{
					printSemanticErrors(currentToken, INCORRECT_ARRAY_ASSIGNMENT);
					return;
				}
				else
				{
					i++;
					int currentArrDimensionCounter = 1;
					while (currentCommandLine[i].kind != SEPERATION_SIGN_RIGHT_BRACKETS_token) //check if array dims not out of bounds
					{
						i++;
						if (currentArrDimensionCounter > rightDataItem->data->arrayDimensionsCounter)
						{
							printSemanticErrors(currentToken, GENERIC_MSG);
							fprintf(semanticOutput, "The number of dimensions is to high.");
							return;
						}

						while (currentCommandLine[i].kind != SEPERATION_SIGN_COMMA_token && currentCommandLine[i].kind != SEPERATION_SIGN_RIGHT_BRACKETS_token)
						{
							if (currentCommandLine[i].kind == FLOAT_NUM_token)
							{
								printSemanticErrors(currentToken, GENERIC_MSG);
								fprintf(semanticOutput, "The array dimension can't be float.");
								return;
							}
							if (currentCommandLine[i].kind == INT_NUM_token
								&& (currentCommandLine[i + 1].kind == SEPERATION_SIGN_COMMA_token || currentCommandLine[i + 1].kind == SEPERATION_SIGN_RIGHT_BRACKETS_token)
								&& (currentCommandLine[i - 1].kind == SEPERATION_SIGN_COMMA_token || currentCommandLine[i - 1].kind == SEPERATION_SIGN_LEFT_BRACKETS_token))
							{
								if (atoi(currentCommandLine[i].lexeme) > (rightDataItem->data->arrayDimensions[currentArrDimensionCounter - 1]) - 1)
								{
									printSemanticErrors(currentToken, GENERIC_MSG);
									fprintf(semanticOutput, "Array assignment out of bounds.");
									return;
								}
							}
							if (currentCommandLine[i].kind == ID_token)
							{
								DataItem* currentIdItem = searchInAllSymbolTablesAndReturnDataItem(StringToInt(currentCommandLine[i].lexeme));
								if (currentIdItem == NULL)
								{
									printSemanticErrors(getCurrentToken(), UNDEFINED);
									fprintf(semanticOutput, "The lexeme: %s is undefined.", currentCommandLine[i].lexeme);
									return;
								}
								if (currentIdItem->data->type != INT)
								{
									printSemanticErrors(currentToken, GENERIC_MSG);
									fprintf(semanticOutput, "lexeme: %s as array dimentions must be int.", currentCommandLine[i].lexeme);
									return;
								}
							}
							i++;
						}
						currentArrDimensionCounter++;
					}

					if (currentArrDimensionCounter - 1 < rightDataItem->data->arrayDimensionsCounter)
					{
						printSemanticErrors(currentToken, GENERIC_MSG);
						fprintf(semanticOutput, "The number of dimensions is too low.");
						return;
					}
				}
			}

			// check function assignment
			if (rightDataItem->data->role != VARIABLE)
			{
				if (rightDataItem->data->paramsCounter == 0)
				{
					if (currentCommandLine[i + 1].kind != SEPERATION_SIGN_LEFT_PARENTHESES_token || currentCommandLine[i + 2].kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
					{
						printSemanticErrors(currentToken, FUNC_ASSINGMENT_ERROR);
						return;
					}
					else
					{
						i = i + 2;
					}
				}
				else
				{
					if (currentCommandLine[i + 1].kind != SEPERATION_SIGN_LEFT_PARENTHESES_token)
					{
						printSemanticErrors(currentToken, FUNC_ASSINGMENT_ERROR);
						return;
					}
					else
					{
						i++;
						int currentArgsCounter = 1;
						while (currentCommandLine[i].kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
						{
							i++;
							if (currentArgsCounter > rightDataItem->data->paramsCounter)
							{
								printSemanticErrors(currentToken, GENERIC_MSG);
								fprintf(semanticOutput, "There are too many paramas in function call.");
								return;
							}

							while (currentCommandLine[i].kind != SEPERATION_SIGN_COMMA_token && currentCommandLine[i].kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
							{
								if (currentCommandLine[i].kind == FLOAT_NUM_token)
								{
									if (rightDataItem->data->listOfParamsTypes[currentArgsCounter - 1].paramType != FLOAT_NUM_token)
									{
										printSemanticErrors(currentToken, GENERIC_MSG);
										fprintf(semanticOutput, "The type of parameter in function call doesn't match to the type in the definition.");
										return;
									}
								}

								if (currentCommandLine[i].kind == ID_token)
								{
									DataItem* currentIdItem = searchInAllSymbolTablesAndReturnDataItem(StringToInt(currentCommandLine[i].lexeme));
									if (currentIdItem == NULL)
									{
										printSemanticErrors(getCurrentToken(), UNDEFINED);
										fprintf(semanticOutput, "The lexeme: %s is undefined.", currentCommandLine[i].lexeme);
										return;
									}
									if (currentIdItem->data->type == FLOAT)
									{
										if (rightDataItem->data->listOfParamsTypes[currentArgsCounter - 1].paramType != FLOAT_NUM_token)
										{
											printSemanticErrors(currentToken, GENERIC_MSG);
											fprintf(semanticOutput, "The type of parameter in function call doesn't match to the type in the definition.");
											return;
										}
									}
								}
								i++;
							}
							currentArgsCounter++;
						}
						if (currentArgsCounter - 1 < rightDataItem->data->paramsCounter)
						{
							printSemanticErrors(currentToken, GENERIC_MSG);
							fprintf(semanticOutput, "There are not enough paramas in function call.");
							return;
						}
					}
				}
			}

		}
		else
		{
			if (currentCommandLine[i].kind == FLOAT_NUM_token)
				rightSide = FLOAT;
		}
	}
	if (rightSide == ERROR_TYPE)
		rightSide = INT;

	if (leftSide == FLOAT)
		return;
	else
	{
		if (rightSide == INT)
			return;
		else
		{
			if (doublePrint == 0)
			{
				printSemanticErrors(currentToken, TYPE_MISMATCH);
				doublePrint = 1;
			}
			else
			{
				doublePrint = 0;
			}
		}
	}
}

DataItem* searchInAllSymbolTablesAndReturnDataItem(int key)
{
	NodeOfStack* currNode = tablesStack->head;
	SymbolTable* tempSymbolTableToSearch = tablesStack->head->symbolTable;
	DataItem* result;
	while (currNode != NULL)
	{
		result = searchInCurrentSymbolTableAndReturnDataItem(tempSymbolTableToSearch, key);
		if (result == NULL)
		{
			currNode = currNode->nextNode;
			if (currNode != NULL)
			{
				tempSymbolTableToSearch = currNode->symbolTable;
			}
		}
		else
		{
			return result;
		}
	}
	return NULL;
}

eType checkMulOrAddTypes(eType first, eType sec)
{
	if (sec == NULL_TYPE)
	{
		return first;
	}
	if (first == ERROR_TYPE || sec == ERROR_TYPE)
	{
		return ERROR_TYPE;
	}
	if (first == FLOAT || sec == FLOAT)
	{
		return FLOAT;
	}
	if (first == INT && sec == INT)
	{
		return INT;
	}
	return ERROR_TYPE;
}

eType checkListArrayIsInt(eType first, eType sec)
{
	if (sec == NULL_TYPE)
	{
		return first;
	}
	if (first == INT && sec == INT)
	{
		return INT;
	}
	return ERROR_TYPE;

}

void checkFinalAssignmentAndPrintError(eType leftType, eType rightType, DataItem* idDataItem)
{
	if (idDataItem == NULL)
	{
		return;
	}
	else {
		if (leftType == ERROR_TYPE || rightType == ERROR_TYPE)
		{
			fprintf(semanticOutput, " Cannot assign to error type");
		}
		else if (idDataItem->data->type == INT && rightType == FLOAT)
		{
			if (doublePrint == 0)
			{
				printSemanticErrors(currentToken, TYPE_MISMATCH);
				doublePrint = 1;
			}
			else
			{
				doublePrint = 0;
			}
		}
		else
		{
			
		}
		
	}

}

void checkIfReturnTypeMatch(eType type, Token* idToken)
{
	DataItem* di;
	di = searchInAllSymbolTablesAndReturnDataItem(StringToInt(idToken->lexeme));
	if (di == NULL)
	{
		printSemanticErrors(currentToken, UNDEFINED);
		printSemanticErrors(currentToken, FUNCTION_ID);
		return;
	}
	if (type == di->data->type)
	{	
		returnStmtCounter++;		
	}
	else
	{
		printSemanticErrors(currentToken, INCORRECT_RETURN_TYPE);
		fprintf(semanticOutput, "return type: %s, return type from func: %s.", TYPE_STRING[di->data->type], TYPE_STRING[type]);
	}
}

void checkFunctionCall()
{
	DataItem* idDataItem = searchInAllSymbolTablesAndReturnDataItem(StringToInt(currentCommandLine[0].lexeme));
	if (idDataItem == NULL)
	{
		printSemanticErrors(currentToken, UNDEFINED);
		return;
	}
	if (idDataItem->data->role == VARIABLE)
	{
		printSemanticErrors(currentToken, GENERIC_MSG);
		fprintf(semanticOutput, "This id is not a function.");
		return;
	}

	if (idDataItem->data->paramsCounter == 0)
	{
		if (currentCommandLine[1].kind != SEPERATION_SIGN_LEFT_PARENTHESES_token || currentCommandLine[2].kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
		{
			printSemanticErrors(currentToken, GENERIC_MSG);
			fprintf(semanticOutput, "There are too many paramas in function call.");
			return;
		}
	}
	else
	{
		int i = 1; // on (
		int currentArgsCounter = 1;
		while (currentCommandLine[i].kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
		{
			i++;
			if (currentArgsCounter > idDataItem->data->paramsCounter)
			{
				printSemanticErrors(currentToken, GENERIC_MSG);
				fprintf(semanticOutput, "There are too many paramas in function call.");
				return;
			}

			while (currentCommandLine[i].kind != SEPERATION_SIGN_COMMA_token && currentCommandLine[i].kind != SEPERATION_SIGN_RIGHT_PARENTHESES_token)
			{
				if (currentCommandLine[i].kind == FLOAT_NUM_token)
				{
					if (idDataItem->data->listOfParamsTypes[currentArgsCounter - 1].paramType != FLOAT_NUM_token)
					{
						printSemanticErrors(currentToken, GENERIC_MSG);
						fprintf(semanticOutput, "The type of parameter in function call doesn't match to the type in the definition.");
						return;
					}
				}

				if (currentCommandLine[i].kind == ID_token)
				{
					DataItem* currentIdItem = searchInAllSymbolTablesAndReturnDataItem(StringToInt(currentCommandLine[i].lexeme));
					if (currentIdItem == NULL)
					{
						printSemanticErrors(getCurrentToken(), UNDEFINED);
						fprintf(semanticOutput, "The lexeme: %s is undefined", currentCommandLine[i].lexeme);
						return;
					}
					if (currentIdItem->data->type == FLOAT)
					{
						if (idDataItem->data->listOfParamsTypes[currentArgsCounter - 1].paramType != FLOAT_NUM_token)
						{
							printSemanticErrors(currentToken, GENERIC_MSG);
							fprintf(semanticOutput, "The type of parameter in function call doesn't match to the type in the definition.");
							return;
						}
					}
				}
				i++;
			}
			currentArgsCounter++;
		}

		if (currentArgsCounter - 1 < idDataItem->data->paramsCounter)
		{
			printSemanticErrors(currentToken, GENERIC_MSG);
			fprintf(semanticOutput, "There are not enough paramas in function call.");
			return;
		}
	}
}
