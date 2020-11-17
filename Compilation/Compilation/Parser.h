#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Token.h"
#include "Semantic.h"
#include "TablesStack.h"

void parse_PROG();
void parse_GLOBAL_VARS();
void parse_GLOBAL_VARS_tag();
void parse_VAR_DEC();
void parse_VAR_DEC_tag();
void parse_TYPE(char* callFunc);
void parse_DIM_SIZES();
void parse_DIM_SIZES_tag();
void parse_FUNC_PREDEFS();
void parse_FUNC_PREDEFS_tag();
Token* parse_FUNC_PROTOTYPE();
void parse_FUNC_FULL_DEFS();
void parse_FUNC_FULL_DEFS_tag();
void parse_FUNC_WITH_BODY();
void parse_RETURNED_TYPE();
void parse_PARAMS();
void parse_PARAMS_LIST();
void parse_PARAMS_LIST_tag();
void parse_PARAM();
void parse_PARAM_tag();
void parse_COMP_STMT(int IsFunc, Token* idToken);
void parse_VAR_DEC_LIST();
void parse_STMT_LIST(Token* idToken);
void parse_STMT_LIST_tag(Token* idToken);
void parse_STMT(Token* idToken);
void parse_STMT_tag(DataItem* idDataItem);
void parse_IF_STMT(Token* idToken);
void parse_CALL();
eType parse_ARGS();
eType parse_ARG_LIST();
eType parse_ARG_LIST_tag();
void parse_RETURN_STMT(Token* idToken);
void parse_RETURN_STMT_tag(Token* idToken);
void parse_VAR();
eType parse_VAR_tag();
eType parse_EXPR_LIST();
eType parse_EXPR_LIST_tag();
void parse_CONDITION();
eType parse_EXPR(int isCondition);
eType parse_EXPR_tag(int isCondition);
eType parse_TERM(int isCondition);
eType parse_TERM_tag(int isCondition);
eType parse_FACTOR(int isCondition);
eType parse_FACTOR_tag();
void insertToSymbolTableIfIdIsValid(int isPreDef);
int insertVariableType();
//void toString(DataItem* DataItemToString);
void insertFuncParamsToSymbolTable();
void assignCurrentTokenToCommands(Token* CurrToken);
void checkAssignment();
DataItem* searchInAllSymbolTablesAndReturnDataItem(int key);
void checkFinalAssignmentAndPrintError(eType firstType, eType secType, DataItem* idDataItem);
eType checkListArrayIsInt(eType first, eType sec);
eType checkMulOrAddTypes(eType first, eType sec);
void checkIfReturnTypeMatch(eType type, Token* idToken);
void checkFunctionCall();