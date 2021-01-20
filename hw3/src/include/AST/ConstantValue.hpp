#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "AST/ast.hpp"

class ConstantValueNode : public ExpressionNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      /* TODO: constant value */
                     char* value, char* type
                      );
    ~ConstantValueNode() = default;

    void print() override;

  public:
    // TODO: constant value
    //Location const_valueLocation; //Line and column numbers of the constant value
    std::string value;
    std::string type;
};

#endif
