#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, 
                    vector<VariableNode*>* variable, vector<char*>* type,
                    ConstantValueNode* const_node
                    )
    : AstNode{line, col}, variable(variable), type(type), const_node(const_node) {}


// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {
    printSpace();
    std::printf("declaration <line: %u, col: %u> \n",
                location.line, location.col
                );
    for(int i = 0; i < variable->size(); i++){
		(*variable)[i]->space = this->space + 2;
		if(this->const_node != NULL){
            (*variable)[i]->print_const(this->const_node);
        }
		else{
            (*variable)[i]->print_type(this->type);
        }
	}
}

// void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
