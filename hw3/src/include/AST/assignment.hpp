#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"

class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col,
                   /* TODO: variable reference, expression */
                   AstNode* Lvalue,
                   AstNode* expression
                   );
    ~AssignmentNode() = default;

    void print() override;

  public:
    // TODO: variable reference, expression
    AstNode* Lvalue; //A variable reference node
    AstNode* expression; //An expression node
};

#endif
