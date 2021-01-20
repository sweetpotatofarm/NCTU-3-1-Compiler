#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
              /* TODO: expression, compound statement */
              AstNode* condition, CompoundStatementNode* body
              );
    ~WhileNode() = default;

    void print() override;

  public:
    // TODO: expression, compound statement
    AstNode* condition; //An expression node
    CompoundStatementNode* body; //A compound statement node
};

#endif
