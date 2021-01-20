#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
                        AstNode* condition,
                        CompoundStatementNode* body)
    : AstNode{line, col}, condition(condition),body(body) {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {
    printSpace();
    std::printf("while statement <line: %u, col: %u>\n", location.line,
            location.col);
    condition->space = this->space + 2;
    condition->print();
    body->space = this->space + 2;
    body->print();
}

// void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
