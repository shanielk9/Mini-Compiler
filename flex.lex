%option noyywrap

%{
int line_num = 1;
%}

DIGIT    [0-9]
LOWER	 [a-z]
UPPER	 [A-Z]
ALPHA	 [a-zA-Z]
UNDERSCORE "_"

/*Numbers*/
INT_NUM		0|[1-9][0-9]*
FLOAT_NUM	{INT_NUM}"."{DIGIT}+("e"|"E")("+"|"-")?{INT_NUM}

/*Operations*/

/*ar_op*/
MULTIPLACTION "*"
PLUS "+"

/*rel_op*/
BELOW "<"
BELOW_EQUAL "<="
EQUAL "=="
ABOVE_EQUAL ">="
ABOVE ">"
NOT_EQUAL "!="

/*assigment_op*/
ASSIGNMENT "="

/*Keywords*/
KEYWORD_INT "int"
KEYWORD_FLOAT "float"
KEYWORD_VOID "void"
KEYWORD_IF "if"
KEYWORD_RETURN "return"

/*ID*/
ID {LOWER}(({ALPHA}|{DIGIT})*{UNDERSCORE}?({ALPHA}|{DIGIT})+)*

/*Separation signs*/
COLON ":"
COMMA ","
SEMI_COLON ";"
LEFT_PARENTHESES "("
RIGHT_PARENTHESES ")"
LEFT_BRACKETS "["
RIGHT_BRACKETS "]"
RIGHT_CURLY_BRACKETS "}"
LEFT_CURLY_BRACKETS "{"
NEW_LINE	"\n"
DOT .

/*Eof*/
EOF <<EOF>>

%x C_COMMENT


%%

{INT_NUM}       { create_and_store_token(INT_NUM_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type INT_NUM , lexeme: '%s'\n", line_num, yytext); return 1;}
{FLOAT_NUM}     { create_and_store_token(FLOAT_NUM_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type FLOAT_NUM , lexeme: '%s'\n", line_num, yytext);return 1;}
{PLUS}       { create_and_store_token(AR_OP_PlUS_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type AR_OP_PLUS , lexeme: '%s'\n", line_num, yytext);return 1;}
{MULTIPLACTION}       { create_and_store_token(AR_OP_MUL_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type AR_OP_MUL , lexeme: '%s'\n", line_num, yytext);return 1;}
{NOT_EQUAL}       { create_and_store_token(REL_OP_NOT_EQUAL_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type REL_OP_NOT_EQUAL , lexeme: '%s'\n", line_num, yytext);return 1;}
{ABOVE}       { create_and_store_token(REL_OP_ABOVE_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type REL_OP_ABOVE , lexeme: '%s'\n", line_num, yytext);return 1;}
{ABOVE_EQUAL}       { create_and_store_token(REL_OP_ABOVE_EQUAL_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type REL_OP_ABOVE_EQUAL , lexeme: '%s'\n", line_num, yytext);return 1;}
{EQUAL}       { create_and_store_token(REL_OP_EQUAL_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type REL_OP_EQUAL , lexeme: '%s'\n", line_num, yytext);return 1;}
{BELOW_EQUAL}       { create_and_store_token(REL_OP_BELOW_EQUAL_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type REL_OP_BELOW_EQUAL , lexeme: '%s'\n", line_num, yytext);return 1;}
{BELOW}       { create_and_store_token(REL_OP_BELOW_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type REL_OP_BELOW , lexeme: '%s'\n", line_num, yytext);return 1;}
{ASSIGNMENT}       { create_and_store_token(ASSIGNMENT_OP_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type ASSIGNMENT_OP , lexeme: '%s'\n", line_num, yytext);return 1;}
{KEYWORD_INT}       { create_and_store_token(KEYWORD_INT_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type KEYWORD_INT , lexeme: '%s'\n", line_num, yytext);return 1;}
{KEYWORD_FLOAT}     { create_and_store_token(KEYWORD_FLOAT_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type KEYWORD_FLOAT , lexeme: '%s'\n", line_num, yytext);return 1;}
{KEYWORD_VOID}      { create_and_store_token(KEYWORD_VOID_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type KEYWORD_VOID , lexeme: '%s'\n", line_num, yytext);return 1;}
{KEYWORD_IF}       { create_and_store_token(KEYWORD_IF_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type KEYWORD_IF , lexeme: '%s'\n", line_num, yytext);return 1;}
{KEYWORD_RETURN}       { create_and_store_token(KEYWORD_RETURN_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type KEYWORD_RETURN , lexeme: '%s'\n", line_num, yytext);return 1;}
{ID}       { create_and_store_token(ID_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type ID , lexeme: '%s'\n", line_num, yytext);return 1;}
{COMMA}       { create_and_store_token(SEPERATION_SIGN_COMMA_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type SEPERATION_SIGN_COMMA , lexeme: '%s'\n", line_num, yytext);return 1;}
{COLON}       { create_and_store_token(SEPERATION_SIGN_COLON_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type SEPERATION_SIGN_COLON , lexeme: '%s'\n", line_num, yytext);return 1;}
{SEMI_COLON}       { create_and_store_token(SEPERATION_SIGN_SEMI_COLON_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type SEPERATION_SIGN_SEMI_COLON , lexeme: '%s'\n", line_num, yytext);return 1;}
{LEFT_PARENTHESES}       { create_and_store_token(SEPERATION_SIGN_LEFT_PARENTHESES_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type SEPERATION_SIGN_LEFT_PARENTHESES , lexeme: '%s'\n", line_num, yytext);return 1;}
{RIGHT_PARENTHESES}       { create_and_store_token(SEPERATION_SIGN_RIGHT_PARENTHESES_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type SEPERATION_SIGN_RIGHT_PARENTHESES , lexeme: '%s'\n", line_num, yytext);return 1;}
{LEFT_BRACKETS}       { create_and_store_token(SEPERATION_SIGN_LEFT_BRACKETS_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type SEPERATION_SIGN_LEFT_BRACKETS , lexeme: '%s'\n", line_num, yytext);return 1;}
{RIGHT_BRACKETS}       { create_and_store_token(SEPERATION_SIGN_RIGHT_BRACKETS_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type SEPERATION_SIGN_RIGHT_BRACKETS , lexeme: '%s'\n", line_num, yytext);return 1;}
{RIGHT_CURLY_BRACKETS}       { create_and_store_token(SEPERATION_SIGN_RIGHT_CURLY_BRACKETS_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type SEPERATION_SIGN_RIGHT_CURLY_BRACKETS , lexeme: '%s'\n", line_num, yytext);return 1;}
{LEFT_CURLY_BRACKETS}       { create_and_store_token(SEPERATION_SIGN_LEFT_CURLY_BRACKETS_token,  yytext, line_num); fprintf(yyout,"Line %d : found token of type SEPERATION_SIGN_LEFT_CURLY_BRACKETS , lexeme: '%s'\n", line_num, yytext);return 1;}
{NEW_LINE}	++line_num;
<<EOF>> { create_and_store_token(EOF_token,  "", line_num); fprintf(yyout,"Line %d : found token of type EOF", line_num);return 0;}
[ \t] 			{}
.       		fprintf(yyout ,"Character:'%s' in line: %d does not begin any legal token in the language\n", yytext, line_num );
"/*"            BEGIN(C_COMMENT); 
<C_COMMENT>\n   line_num++;
<C_COMMENT>[^*\n]+ {}
<C_COMMENT>"*/" BEGIN(INITIAL);


%%

int main(int argc, char **argv )
{
	yyin = fopen("C:\\temp\\test1.txt", "r");

	yyout = fopen("C:\\temp\\test1_output_lex.txt", "w");
	syntacticOutput = fopen("C:\\temp\\test1_output_syntactic.txt", "w");
	semanticOutput = fopen("C:\\temp\\test1_output_semantic.txt", "w");

	parse_PROG();
	match(EOF_token);

	fclose(yyin);
	fclose(yyout);
	fclose(syntacticOutput);
	free_storage();

	yyin = fopen("C:\\temp\\test2.txt", "r");

	yyrestart(yyin);
	line_num = 1;

	yyout = fopen("C:\\temp\\test2_output_lex.txt", "w");
	syntacticOutput = fopen("C:\\temp\\test2_output_syntactic.txt", "w");
	semanticOutput = fopen("C:\\temp\\test2_output_semantic.txt", "w");

	parse_PROG();
	match(EOF_token);

	fclose(yyin);
	fclose(yyout);
	fclose(syntacticOutput);
	free_storage();

	yylex_destroy();
}