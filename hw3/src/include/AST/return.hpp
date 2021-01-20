#ifndef __AST_RETURN_NODE_H
#define __AST_RETURN_NODE_H

#include "AST/ast.hpp"

class ReturnNode : public AstNode {
  public:
    ReturnNode(const uint32_t line, const uint32_t col,
               /* TODO: expression */
               AstNode* return_value
               );
    ~ReturnNode() = default;

    void print() override;

  private:
    // TODO: expression
    AstNode* return_value;
};

#endif
