#include "Token.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int currentIndex = 0;
int out_of_bounds = 0;
Node* currentNode = NULL;

#define TOKEN_ARRAY_SIZE 1000

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{ 
	int length = strlen(lexeme)+1;
	
	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if(currentNode == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->tokensArray = 
			(Token*) calloc(sizeof(Token),TOKEN_ARRAY_SIZE);
		if(currentNode->tokensArray == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if(currentNode == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray = 
			 (Token*)calloc(sizeof(Token),TOKEN_ARRAY_SIZE);

			if(currentNode->tokensArray == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;	
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char)*length);
	#ifdef _WIN32
		strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
	#else
		strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
	#endif		
}




/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token *back_token()
{
	if (currentIndex == 0)
	{
		if (currentNode->prev != NULL)
		{
			currentNode = currentNode->prev;
			currentIndex = TOKEN_ARRAY_SIZE - 1;
		}
		else
		{
			if (out_of_bounds)
			{
				fprintf(syntacticOutput, "Back token was expected, but first token returned");
			}
			else
			{
				out_of_bounds = 1;
			}
		}
	}
	else
	{
		currentIndex--;
	}

	return &currentNode->tokensArray[currentIndex];
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token): 
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token *next_token()
{
	if (currentNode != NULL)
	{
		if (currentNode->tokensArray[currentIndex + 1].lexeme != NULL && currentIndex < TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex++;
		}
		else
		{
			if (currentNode->next != NULL)
			{
				currentNode = currentNode->next;
				currentIndex = 0;
			}
			else
			{
				if (currentNode->tokensArray[currentIndex].kind != EOF_token)
				{
					yylex();
				}
			}

		}
	}

	else
	{
		yylex();
	}

	out_of_bounds = 0;
	return &currentNode->tokensArray[currentIndex];
}

Token* getCurrentToken()
{
	return &currentNode->tokensArray[currentIndex];
}

int match(eTOKENS token)
{
	Token * curr_token = next_token();

	if (curr_token->kind != token)
	{
		fprintf(syntacticOutput, "Expected: token '%s' at line %d ", ENUM_strings[token], curr_token->lineNumber);
		fprintf(syntacticOutput, "Actual token of type: '%s', lexeme “ %s ” \n", ENUM_strings[curr_token->kind], curr_token->lexeme);
		return 0;
	}

	return 1;
}

void free_storage()
{
	Node* temp = NULL;

	
	while (currentNode->next != NULL)
	{
		currentNode = currentNode->next;
	}

	while (currentNode->prev != NULL)
	{
		temp = currentNode;
		for (int i = 0; i < TOKEN_ARRAY_SIZE; i++)
		{
			if (temp->tokensArray[i].lexeme != NULL)
			{
				free(temp->tokensArray[i].lexeme);
			}
		}
		free(temp->tokensArray);
		currentNode = currentNode->prev;
		free(temp);

	}
	for (int i = 0; i < TOKEN_ARRAY_SIZE; i++)
	{
		if (currentNode->tokensArray[i].lexeme != NULL)
		{
			free(currentNode->tokensArray[i].lexeme);
		}
	}

	free(currentNode->tokensArray);
	free(currentNode);

	currentNode = NULL;
	currentIndex = 0;

}
