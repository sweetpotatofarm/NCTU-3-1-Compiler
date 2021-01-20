#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
                 /* TODO: variable name, type, constant value */
                 const char* name
                 );
    ~VariableNode() = default;

    void print() override;

    void print_type(std::vector<char*>* type);
	  void print_const(ConstantValueNode* const_node);

  public:
    // TODO: variable name, type, constant value
    //Location varLocation; //Line and column numbers of the variable name
    std::string name; //variable name
};

#endif
