#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"


class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                          /* TODO: declarations, statements */
                          vector<DeclNode*>* var_const_decl,
                          vector<AstNode*>* Statements
                          );
    ~CompoundStatementNode() = default;

    void print() override;

  public:
    // TODO: declarations, statements
    //Location compoundStatementLocation; //Line and column numbers of keyword begin
    vector<DeclNode*>* var_const_decl; //A list of declaration nodes
    vector<AstNode*>* Statements; //A list of statement nodes
};

#endif
