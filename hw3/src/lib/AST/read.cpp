#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col,
                    VariableReferenceNode* target)
    : AstNode{line, col}, target(target) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {
        printSpace();
        std::printf("read statement <line: %u, col: %u>\n",
                location.line, location.col);
        target->space = this->space + 2;
        target->print();
}

// void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
