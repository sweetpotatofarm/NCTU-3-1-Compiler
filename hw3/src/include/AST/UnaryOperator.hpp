#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"

class UnaryOperatorNode : public ExpressionNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
                      /* TODO: operator, expression */
                      char* unary_operator,
                      AstNode* operand
                      );
    ~UnaryOperatorNode() = default;

    void print() override;

  public:
    // TODO: operator, expression
    std::string unary_operator;
    AstNode* operand;
};

#endif
