#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
                                    char* unary_operator,
                                    AstNode* operand)
    : ExpressionNode{line, col}, unary_operator(unary_operator), operand(operand) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {
    printSpace();
    std::printf("unary operator <line: %u, col: %u> %s\n",
                location.line, location.col, unary_operator.c_str()
                );
    operand->space = this->space + 2;
    operand->print();
    
}

// void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
