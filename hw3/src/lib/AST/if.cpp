#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col,           
                AstNode* Condition,
                CompoundStatementNode* body,
                CompoundStatementNode* body_of_else)
    : AstNode{line, col}, Condition(Condition),
      body(body), body_of_else(body_of_else) {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {
    printSpace();
    std::printf("if statement <line: %u, col: %u>\n", location.line,
            location.col);
    Condition->space = this->space + 2;
    Condition->print();
    body->space = this->space + 2;
    body->print();
    if(body_of_else != NULL){
      body_of_else->space = this->space + 2;
      body_of_else->print();
    }
}

// void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
