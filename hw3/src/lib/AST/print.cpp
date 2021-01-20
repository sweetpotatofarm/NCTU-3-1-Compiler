#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col,
                    AstNode* Target)
    : AstNode{line, col}, Target(Target) {}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {
    printSpace();
    std::printf("print statement <line: %u, col: %u>\n",
                location.line, location.col);
    if( Target != NULL){
        Target->space = this->space + 2;
        Target->print();
    }
}

// void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
