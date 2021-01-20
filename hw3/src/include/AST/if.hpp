#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"

class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col,
           /* TODO: expression, compound statement, compound statement */
           AstNode* Condition,
           CompoundStatementNode* body,
           CompoundStatementNode* body_of_else
           );
    ~IfNode() = default;

    void print() override;

  public:
    // TODO: expression, compound statement, compound statement
    AstNode* Condition; //An expression node
    CompoundStatementNode* body; //A compound statement node
    CompoundStatementNode* body_of_else; //An optional compound statement node
};

#endif
