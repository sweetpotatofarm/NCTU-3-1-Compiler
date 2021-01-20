#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */
                 char* name, vector<DeclNode*>* Parameters,
                 char* returntype, CompoundStatementNode* body
                 );
    ~FunctionNode() = default;

    void print() override;

  public:
    // TODO: name, declarations, return type, compound statement
    //Location functionLocation; //Line and column numbers of the function name
    std::string name;
    vector<DeclNode*>* Parameters; //A list of declaration nodes
    std::string returntype;
    CompoundStatementNode* body; //An optional compound statement node
};

#endif
