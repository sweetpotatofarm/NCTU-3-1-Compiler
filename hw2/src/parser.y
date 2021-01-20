%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int32_t line_num;   /* declared in scanner.l */
extern char buffer[512];  /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

extern int yylex(void); 
static void yyerror(const char *msg);
%}

%token MOD ASSIGN LESS_EQUAL NOT_EQUAL GREATER_EQUAL AND OR NOT
%token ARRAY BG BOOLEAN DEF DO ELSE END FALSE FOR INTEGER IF OF PRINT READ REAL STRING THEN TO TRUE RETURN VAR WHILE
%token IDENTIFIER INT_DEC INT_OCT FLOAT SCIENTIFIC STR

%left NEG
%left '*' '/'
%left '+' '-' 
%left '>' '<' '='
%left NOT AND OR

%%

program: IDENTIFIER ';' zero_more_variable_const zero_more_function compound END
        ;
zero_more_variable_const: /*empty*/
        | variable_const_def
        ;
variable_const_def: variable_const variable_const_def
        | variable_const
        ;
zero_more_function: /*empty*/
        | function_def_dec
        ;
function_def_dec: function function_def_dec
        | function
        ;
function: IDENTIFIER '(' zero_more_arguments ')' ':' type ';' /*declaration*/
        | IDENTIFIER '(' zero_more_arguments ')' ':' type compound END /*definition*/
        | IDENTIFIER '(' zero_more_arguments ')' ';'
        | IDENTIFIER '(' zero_more_arguments ')' compound END
        ;
zero_more_arguments: /*empty*/
        | arguments
        ;
arguments: identifier_list ':' type ';' arguments
        | identifier_list ':' type
        ;
identifier_list: /*empty*/
        | IDENTIFIER ',' identifier_list
        | IDENTIFIER
        ;
variable_const: variable
        | constant
        ;
variable: VAR identifier_list ':' type ';'
        | VAR identifier_list ':' ARRAY integer_constant OF array_type ';'
        ;
array_type: ARRAY
        | ARRAY integer_constant OF array_type
        | type
        ;
integer_constant: INT_DEC
        ;
constant: VAR identifier_list ':' literal_constant ';'
        ;
literal_constant: number
        | STR
        | TRUE
        | FALSE
        ;
number: INT_DEC
        | INT_OCT
        | SCIENTIFIC
        | FLOAT
        ;
statement: compound
        | simple
        | conditional
        | while
        | for
        | return
        | procedure_call
        ;
compound: BG zero_more_variable_const zero_more_statement END
        ;
zero_more_statement: /*empty*/
        | statement zero_more_statement
        ;
simple: variable_ref ASSIGN expression ';'
        | PRINT variable_ref ';'
        | PRINT expression ';'
        | READ variable_ref ';'
        ;
variable_ref: IDENTIFIER array_ref
        ;
array_ref: /*empty*/
        | '[' expression ']' array_ref
        ;
conditional: IF expression THEN compound ELSE compound END IF
        | IF expression THEN compound END IF
        ;
while: WHILE expression DO compound END DO
        ;
for: FOR IDENTIFIER ASSIGN integer_constant TO integer_constant DO compound END DO
        ;
return: RETURN expression ';'
        ;
procedure_call: IDENTIFIER '(' expression_list ')' ';'
        ;
expression_list: /*empty*/
        | expression ',' expression_list
        | expression
        ;
expression: normal_expr
        | relational_logical_expr
        ;
normal_expr: negative
        | multiplication
        | division
        | addition
        | subtraction
        | IDENTIFIER
        | number
        | STR
        | function_invocation
        | '(' expression ')'
        | variable_ref
        ;
normal_relational_expr: normal_expr
        | relational
        ;
relational_logical_expr: relational
        | logical
        ;
negative: '-' number
        ;
multiplication: normal_expr '*' normal_expr
        ;
division: normal_expr '/' normal_expr
        | normal_expr MOD normal_expr %prec '*'
        ;
addition: normal_expr '+' normal_expr
        ;
subtraction: normal_expr '-' normal_expr
        ;
relational: normal_expr '>' normal_expr
	|normal_expr '<' normal_expr
	|normal_expr LESS_EQUAL normal_expr %prec '<'
	|normal_expr GREATER_EQUAL normal_expr %prec '>'
	|normal_expr '=' normal_expr
	|normal_expr NOT_EQUAL normal_expr %prec '>'
        ;
logical: normal_relational_expr AND normal_relational_expr
	|normal_relational_expr OR  normal_relational_expr
	|NOT normal_relational_expr
        ;
type: INTEGER
        | BOOLEAN
        | REAL
        | STRING
        ;
function_invocation: IDENTIFIER '(' expression_list ')'
        ;



%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./parser <filename>\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    assert(yyin != NULL && "fopen() fails.");

    yyparse();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
