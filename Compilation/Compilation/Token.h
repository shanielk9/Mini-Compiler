#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin, *yyout , *syntacticOutput, *semanticOutput;

typedef enum eTOKENS
{
	INT_NUM_token,
	FLOAT_NUM_token,
	AR_OP_PlUS_token,
	AR_OP_MUL_token,
	REL_OP_NOT_EQUAL_token,
	REL_OP_ABOVE_token,
	REL_OP_ABOVE_EQUAL_token,
	REL_OP_EQUAL_token,
	REL_OP_BELOW_EQUAL_token,
	REL_OP_BELOW_token,
	ASSIGNMENT_OP_token,
	KEYWORD_INT_token,
	KEYWORD_FLOAT_token,
	KEYWORD_VOID_token,
	KEYWORD_IF_token,
	KEYWORD_RETURN_token,
	ID_token,
	SEPERATION_SIGN_COMMA_token,
	SEPERATION_SIGN_COLON_token,
	SEPERATION_SIGN_SEMI_COLON_token,
	SEPERATION_SIGN_LEFT_PARENTHESES_token,
	SEPERATION_SIGN_RIGHT_PARENTHESES_token,
	SEPERATION_SIGN_LEFT_BRACKETS_token,
	SEPERATION_SIGN_RIGHT_BRACKETS_token,
	SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token,
	SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token,
	EOF_token

}eTOKENS;

static const char *ENUM_strings[] =
{
	"INT_NUM_token",
	"FLOAT_NUM_token",
	"AR_OP_PlUS_token",
	"AR_OP_MUL_token",
	"REL_OP_NOT_EQUAL_token",
	"REL_OP_ABOVE_token",
	"REL_OP_ABOVE_EQUAL_token",
	"REL_OP_EQUAL_token",
	"REL_OP_BELOW_EQUAL_token",
	"REL_OP_BELOW_token",
	"ASSIGNMENT_OP_token",
	"KEYWORD_INT_token",
	"KEYWORD_FLOAT_token",
	"KEYWORD_VOID_token",
	"KEYWORD_IF_token",
	"KEYWORD_RETURN_token",
	"ID_token",
	"SEPERATION_SIGN_COMMA_token",
	"SEPERATION_SIGN_COLON_token",
	"SEPERATION_SIGN_SEMI_COLON_token",
	"SEPERATION_SIGN_LEFT_PARENTHESES_token",
	"SEPERATION_SIGN_RIGHT_PARENTHESES_token",
	"SEPERATION_SIGN_LEFT_BRACKETS_token",
	"SEPERATION_SIGN_RIGHT_BRACKETS_token",
	"SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token",
	"SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token",
	"EOF_token"
};

typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token *next_token();
Token *back_token();

#endif