#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/expression.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
                       /* TODO: operator, expressions */
                       char* binary_operator,
                       AstNode* leftOperand,
                       AstNode* rightOperand
                       );
    ~BinaryOperatorNode() = default;

    void print() override;

  public:
    // TODO: operator, expressions
    //Location operatorLocation; //Line and column numbers of the operator
    std::string binary_operator;
    AstNode* leftOperand;
    AstNode* rightOperand;
};

#endif
