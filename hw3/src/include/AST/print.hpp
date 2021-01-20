#ifndef __AST_PRINT_NODE_H
#define __AST_PRINT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"

class PrintNode : public AstNode {
  public:
    PrintNode(const uint32_t line, const uint32_t col,
              /* TODO: expression */
              AstNode* Target
              );
    ~PrintNode() = default;

    void print() override;

  private:
    // TODO: expression
    //Location printLocation; //Line and column numbers of keyword print
    AstNode* Target; //An expression node
};

#endif
