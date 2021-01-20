#ifndef __AST_PROGRAM_NODE_H
#define __AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"

class ProgramNode : public AstNode {
  public:
    ProgramNode(const uint32_t line, const uint32_t col,
                const char* p_name,
                /* TODO: return type, declarations, functions,
                 *       compound statement */
                const char* returnType, vector<DeclNode*>* var_const_declarations,
                vector<FunctionNode*>* function_declarations, CompoundStatementNode* body
                );
    ~ProgramNode() = default;

    // visitor pattern version: const char *getNameCString() const; 

    void print() override;

  public:
    // TODO: return type, declarations, functions, compound statement

    //Location programLocation; //Line and column numbers of the program name
    std::string name; //program name
    std::string returnType; //return type
    vector<DeclNode*>* var_const_declarations; //a list of declaration nodes
    vector<FunctionNode*>* function_declarations; //a list of function nodes
    CompoundStatementNode* body; //a compound statement node
};

#endif
