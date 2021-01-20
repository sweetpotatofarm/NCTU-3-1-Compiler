#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"

class FunctionInvocationNode : public ExpressionNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           /* TODO: function name, expressions */
                          char* name, vector<AstNode*>* arguments
                          );
    ~FunctionInvocationNode() = default;

    const char *getNameCString() const;

    void print() override;

  public:
    // TODO: function name, expressions
    std::string name;
    vector<AstNode*>* arguments; //A list of expression nodes
};

#endif
