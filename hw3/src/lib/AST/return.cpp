#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col, AstNode* return_value)
    : AstNode{line, col}, return_value(return_value) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {
    printSpace();
    std::printf("return statement <line: %u, col: %u>\n",
            location.line, location.col);
    return_value->space = this->space + 2;
    return_value->print();
}

// void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
