#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"

class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col,
             /* TODO: identifiers, type */
             vector<VariableNode*>* variable, vector<char*>* type,
             ConstantValueNode *const_node
             );

    // constant variable declaration
    // DeclNode(const uint32_t, const uint32_t col,
    //          /* TODO: identifiers, constant */
    //          vector<VariableNode*> variable
    //          );

    ~DeclNode() = default;

    void print() override;

  public:
    // TODO: variables
    //Location declLocation; //Line and column numbers of parameter name || Line and column numbers of keyword var
    vector<VariableNode*>* variable; //a list of variable nodes
    vector<char*>* type;
    ConstantValueNode *const_node;
};

#endif
