#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                                AstNode* Lvalue,
                                AstNode* expression)
    : AstNode{line, col}, Lvalue(Lvalue), expression(expression) {}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {
    printSpace();
    std::printf("assignment statement <line: %u, col: %u>\n",
                location.line,
                location.col);
    Lvalue->space = this->space+2;
    Lvalue->print();
    expression->space = this->space+2;
    expression->print();
}

// void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
