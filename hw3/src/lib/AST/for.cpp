#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
                DeclNode* loop_var_decl,
                AssignmentNode* init_statement,
                ConstantValueNode* condition,
                CompoundStatementNode* body)
    : AstNode{line, col}, loop_var_decl(loop_var_decl), init_statement(init_statement)
                        , condition(condition),body(body) {}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {
    printSpace();
    std::printf("for statement <line: %u, col: %u>\n", location.line,
            location.col);
    loop_var_decl->space = this->space + 2;
    loop_var_decl->print();
    init_statement->space = this->space + 2;
    init_statement->print();
    condition->space = this->space + 2;
    condition->print();
    body->space = this->space + 2;
    body->print();
}

// void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
