#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/assignment.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/CompoundStatement.hpp"

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
            /* TODO: declaration, assignment, expression,
             *       compound statement */
            DeclNode* loop_var_decl,
            AssignmentNode* init_statement,
            ConstantValueNode* condition,
            CompoundStatementNode* body
            );
    ~ForNode() = default;

    void print() override;

  public:
    // TODO: declaration, assignment, expression, compound statement
    DeclNode* loop_var_decl; //Loop variable declaration
    AssignmentNode* init_statement; //An assignment node //The expression node in this assignment node will only be a constant value node.
    ConstantValueNode* condition; //A constant value node
    CompoundStatementNode* body; //A compound statement node
};

#endif
