#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             char* name)
    : ExpressionNode{line, col},name(name) {}

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             char* name, vector<AstNode*>* indices)
    : ExpressionNode{line, col}, name(name), indices(indices) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {
    printSpace();
    std::printf("variable reference <line: %u, col: %u> %s\n",
            this->location.line,
            this->location.col,
            name.c_str());
    if(indices != NULL){
        for(int i=0; i<indices->size(); i++){
            (*indices)[i]->space = this->space + 2;
            (*indices)[i]->print();
        }
    }
}

// void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
