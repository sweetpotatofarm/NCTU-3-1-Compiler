#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               char* name, vector<AstNode*>* arguments
                                               )
    : ExpressionNode{line, col}, name(name), arguments(arguments) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {
    printSpace();
    std::printf("function invocation <line: %u, col: %u> %s\n",
                location.line,
                location.col,
                name.c_str());
    if(arguments != NULL){
        for(int i=0; i<arguments->size(); i++){
            (*arguments)[i]->space = this->space + 2;
            (*arguments)[i]->print();
        }
    }
}

// void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
