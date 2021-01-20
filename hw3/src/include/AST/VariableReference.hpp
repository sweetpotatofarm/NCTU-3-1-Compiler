#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"

class VariableReferenceNode : public ExpressionNode {
  public:
    // normal reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          /* TODO: name */
                          char* name
                          );
    // array reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                         /* TODO: name, expressions */
                         char* name, vector<AstNode*>* indices
                          );
    ~VariableReferenceNode() = default;

    void print() override;

  public:
    // TODO: variable name, expressions
    std::string name;
    vector<AstNode*>* indices; //A list of expression nodes
};

#endif
