#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
                                        char* binary_operator,
                                        AstNode* leftOperand,
                                        AstNode* rightOperand)
    : ExpressionNode{line, col}, binary_operator(binary_operator),
    leftOperand(leftOperand), rightOperand(rightOperand) {}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {
    printSpace();
    std::printf("binary operator <line: %u, col: %u> %s\n",
                location.line, location.col,
                binary_operator.c_str());
    leftOperand->space = this->space+2;
    leftOperand->print();
    rightOperand->space = this->space+2;
    rightOperand->print();
    
}

// void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
