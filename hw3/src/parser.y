%{
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"

#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

/* Declared by scanner.l */
extern uint32_t line_num;
extern char buffer[512];
extern FILE *yyin;
extern char *yytext;
/* End */


static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%code requires {
    #include "include/AST/ast.hpp"
    #include "include/AST/assignment.hpp"
    #include "include/AST/AstDumper.hpp"
    #include "include/AST/BinaryOperator.hpp"
    #include "include/AST/CompoundStatement.hpp"
    #include "include/AST/ConstantValue.hpp"
    #include "include/AST/decl.hpp"
    #include "include/AST/expression.hpp"
    #include "include/AST/for.hpp"
    #include "include/AST/function.hpp"
    #include "include/AST/FunctionInvocation.hpp"
    #include "include/AST/if.hpp"
    #include "include/AST/print.hpp"
    #include "include/AST/program.hpp"
    #include "include/AST/read.hpp"
    #include "include/AST/return.hpp"
    #include "include/AST/UnaryOperator.hpp"
    #include "include/AST/variable.hpp"
    #include "include/AST/VariableReference.hpp"
    #include "include/AST/while.hpp"
    #include <vector>
    using namespace std;
}

    /* For yylval */
%union {
    /* basic semantic value */
    char* identifier;
    /* node */
    AstNode* node;
    /* constant value*/
    ConstantValueNode* constValueNode;
    /* decl Node */
    DeclNode* deNode;
    /* compoundstatement node */
    CompoundStatementNode* csNode;
    /* variable reference node */
    VariableReferenceNode* var_ref_node;
    /* function node */
    FunctionNode* fNode;

    vector<char*>* strList;
	vector<AstNode*>* nodeList;
	vector<VariableNode*>* vNodeList;
	vector<DeclNode*>* dNodeList;
    vector<FunctionNode*>* fNodeList;
};

%type <identifier> ProgramName FunctionName
%type <identifier> NegOrNot  MINUS UNARY_MINUS ScalarType INTEGER REAL STRING BOOLEAN ReturnType
%type <node> Statement
%type <node> Simple Condition While For Return FunctionCall Expression
%type <node> FunctionInvocation
%type <strList> ArrDecl ArrType Type StringAndBoolean IntegerAndReal  
%type <nodeList> Statements StatementList ArrRefs ArrRefList
%type <nodeList> ExpressionList Expressions
%type <vNodeList> IdList
%type <dNodeList>  DeclarationList Declarations FormalArgList FormalArgs
%type <constValueNode> LiteralConstant
%type <deNode> Declaration FormalArg
%type <csNode> CompoundStatement ElseOrNot
%type <var_ref_node> VariableReference
%type <fNodeList> FunctionList Functions
%type <fNode> Function FunctionDeclaration FunctionDefinition

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token <identifier> ID

    /* Literal */
%token <identifier> INT_LITERAL
%token <identifier> REAL_LITERAL
%token <identifier> STRING_LITERAL

%%
    /*
       Program Units
                     */

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column,
                               $1, "void", $3, $4, $5);

        free($1);
    }
;

ProgramName:
    ID{
        $$ = $1;
    }
;

DeclarationList:
    Epsilon{
        $$ = NULL;
    }
    |
    Declarations{
        $$ = $1;
    }
;

Declarations:
    Declaration{
        vector<DeclNode*> *temp = new vector<DeclNode*> ();
		temp->push_back($1);
		$$ = temp;
    }
    |
    Declarations Declaration{
        $1->push_back($2);
        $$ = $1;
    }
;

FunctionList:
    Epsilon{
        $$ = NULL;
    }
    |
    Functions{
        $$ = $1;
    }
;

Functions:
    Function{
        vector<FunctionNode*>* temp = new vector<FunctionNode*>();
		temp->push_back($1);
		$$ = temp;
    }
    |
    Functions Function{
        $1->push_back($2);
        $$ = $1;
    }
;

Function:
    FunctionDeclaration{
        $$ = $1;
    }
    |
    FunctionDefinition{
        $$ = $1;
    }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON{
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, NULL);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END{
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, $6);
    }
;

FunctionName:
    ID{
        $$ = $1;
    }
;

FormalArgList:
    Epsilon{
        $$ = NULL;
    }
    |
    FormalArgs{
        $$ = $1;
    }
;

FormalArgs:
    FormalArg{
        vector<DeclNode*> *temp = new vector<DeclNode*> ();
		temp->push_back($1);
		$$ = temp;
    }
    |
    FormalArgs SEMICOLON FormalArg{
        $1->push_back($3);
        $$ = $1;
    }
;

FormalArg:
    IdList COLON Type{
        $$ = new DeclNode(@1.first_line, @1.first_column, $1, $3, NULL);
    }
;

IdList:
    ID{
        $$ = new vector<VariableNode*>();
        VariableNode* temp = new VariableNode(@1.first_line, @1.first_column, $1);
        $$->push_back(temp);
    }
    |
    IdList COMMA ID{
        VariableNode* temp = new VariableNode(@3.first_line, @3.first_column, $3);
        $1->push_back(temp);
        $$ = $1;
    }
;

ReturnType:
    COLON ScalarType{
        $$ = $2;
    }
    |
    Epsilon{
        $$ = "";
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON{
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4, NULL);
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON{
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, NULL, $4);
    }
;

Type:
    ScalarType{
        $$ = new vector<char*> ();
		$$->push_back($1);
    }
    |
    ArrType{
        $$ = $1;
    }
;

ScalarType:
    INTEGER{
        $$ = "integer";
    }
    |
    REAL{
        $$ = "real";
    }
    |
    STRING{
        $$ = "string";
    }
    |
    BOOLEAN{
        $$ = "boolean";
    }
;

ArrType:
    ArrDecl ScalarType{
        $1->insert($1->begin(), $2);
		$$ = $1;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF{
        $$ = new vector<char*> ();
		$$->push_back($2);
    }
    |
    ArrDecl ARRAY INT_LITERAL OF{
        $1->push_back($3);
        $$ = $1;
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL{
        if($1 == ""){
            $$ = new ConstantValueNode(@2.first_line, @2.first_column, $2, "integer");
        }
        else{
            char* temp = new char;
            strcat(temp, $1);
            strcat(temp, $2);
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, temp, "integer");
        }
    }
    |
    NegOrNot REAL_LITERAL{
        if($1 == ""){
            $$ = new ConstantValueNode(@2.first_line, @2.first_column, $2, "real");
        }
        else{
            char* temp = new char;
            strcat(temp, $1);
            strcat(temp, $2);
            $$ = new ConstantValueNode(@1.first_line, @1.first_column, temp, "real");
        }
    }
    |
    StringAndBoolean{
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, (*$1)[1], (*$1)[0]);
    }
;

NegOrNot:
    Epsilon{
        $$ = "";
    }
    |
    MINUS %prec UNARY_MINUS{
        $$ = "-";
    }
;

StringAndBoolean:
    STRING_LITERAL{
        $$ = new vector<char*>();
		$$->push_back("string");
		$$->push_back($1);
    }
    |
    TRUE{
        $$ = new vector<char*>();
		$$->push_back("boolean");
		$$->push_back("true");
    }
    |
    FALSE{
        $$ = new vector<char*>();
		$$->push_back("boolean");
		$$->push_back("false");
    }
;

IntegerAndReal:
    INT_LITERAL{
        $$ = new vector<char*> ();
		$$->push_back("integer");
		$$->push_back($1);
    }
    |
    REAL_LITERAL{
        $$ = new vector<char*> ();
		$$->push_back("real");
		$$->push_back($1);
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement{
        $$ = $1;
    }
    |
    Simple{
        $$ = $1;
    }
    |
    Condition{
        $$ = $1;
    }
    |
    While{
        $$ = $1;
    }
    |
    For{
        $$ = $1;
    }
    |
    Return{
        $$ = $1;
    }
    |
    FunctionCall{
        $$ = $1;
    }
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END{
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON{
        $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);
    }
    |
    PRINT Expression SEMICOLON{
        $$ = new PrintNode(@1.first_line, @1.first_column, $2);
    }
    |
    READ VariableReference SEMICOLON{
        $$ = new ReadNode(@1.first_line, @1.first_column, $2);
    }
;

VariableReference:
    ID ArrRefList{
        $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2);
    }
;

ArrRefList:
    Epsilon{
        $$ =  NULL;
    }
    |
    ArrRefs{
       $$ = $1; 
    }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET{
        $$ = new vector<AstNode*>();
		$$->push_back($2);
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET{
        $1->push_back($3);
		$$ = $1;
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF{
        $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
    }
;

ElseOrNot:
    ELSE
    CompoundStatement{
        $$ = $2;
    }
    |
    Epsilon{
        $$ = NULL;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO{
        $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4);
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO{
        vector<VariableNode*>* var_list = new vector<VariableNode*>();
		vector<char*>* str_list  = new vector<char*>();
		VariableNode* var_node = new VariableNode(@2.first_line, @2.first_column, $2);
		var_list->push_back(var_node);
		str_list->push_back("integer");
	    DeclNode* decl_Node = new DeclNode(@2.first_line, @2.first_column, var_list, str_list, NULL);
		VariableReferenceNode* var_ref_node = new VariableReferenceNode(@2.first_line, @2.first_column, $2, NULL);
	    ConstantValueNode* const_val_node = new ConstantValueNode(@4.first_line, @4.first_column, $4, "integer");
		AssignmentNode* assign_node = new AssignmentNode(@3.first_line, @3.first_column, var_ref_node, const_val_node);
		ConstantValueNode* const_val_node2 = new ConstantValueNode(@6.first_line, @6.first_column, $6, "integer");
		$$ = new ForNode(@1.first_line, @1.first_column, decl_Node, assign_node, const_val_node2, $8);
    }
;

Return:
    RETURN Expression SEMICOLON{
        $$ = new ReturnNode(@1.first_line, @1.first_column, $2);
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON{
        $$ = $1;
    }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS{
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3);
    }
;

ExpressionList:
    Epsilon{ 
        $$ = NULL;
    }
    |
    Expressions{
        $$ = $1;
    }
;

Expressions:
    Expression{
        $$ = new vector<AstNode*> ();
		$$->push_back($1);
    }
    |
    Expressions COMMA Expression{
        $1->push_back($3);
        $$ = $1;
    }
;

StatementList:
    Epsilon{
        $$ = NULL;
    }
    |
    Statements{
        $$ = $1;
    }
;

Statements:
    Statement{
        $$ = new vector<AstNode*> ();
        $$->push_back($1);
    }
    |
    Statements Statement{
        $1->push_back($2);
        $$ = $1;
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS { $$ = $2;}
    |
    MINUS Expression %prec UNARY_MINUS { 
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "neg", $2);
    }
    |
    Expression MULTIPLY Expression  {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "*", $1, $3);
    }
    |
    Expression DIVIDE Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "/", $1, $3);
    }
    |
    Expression MOD Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "mod", $1, $3);
    }
    |
    Expression PLUS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "+", $1, $3);
    }
    |
    Expression MINUS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "-", $1, $3);
    }
    |
    Expression LESS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<", $1, $3);
    }
    |
    Expression LESS_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<=", $1, $3);
    }
    |
    Expression GREATER Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">", $1, $3);
    }
    |
    Expression GREATER_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">=", $1, $3);
    }
    |
    Expression EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "=", $1, $3);
    }
    |
    Expression NOT_EQUAL Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<>", $1, $3);
    }
    |
    NOT Expression { 
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "not", $2);
    }
    |
    Expression AND Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "and", $1, $3);
    }
    |
    Expression OR Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "or", $1, $3);
    }
    |
    IntegerAndReal{
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, (*$1)[1], (*$1)[0]);
    }
    |
    StringAndBoolean{
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, (*$1)[1], (*$1)[0]);
    }
    |
    VariableReference { $$ = $1; }
    |
    FunctionInvocation{ $$ = $1; }
;

    /*
       misc
            */
Epsilon:
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
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    assert(yyin != NULL && "fopen() fails.");

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        root->print();
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
