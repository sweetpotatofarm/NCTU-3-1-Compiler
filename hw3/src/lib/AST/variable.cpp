#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                            const char* name
                            )
    : AstNode{line, col}, name(name) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {
    
}

void VariableNode::print_const(ConstantValueNode* const_node){
    printSpace();
	std::printf("variable <line: %u, col: %u> %s %s\n",
                location.line, location.col, name.c_str(), const_node->type.c_str());
	
	const_node->space = this->space + 2;
	const_node->print();
}

void VariableNode::print_type(std::vector<char*>* type){
	printSpace();
	
	std::printf("variable <line: %u, col: %u> %s ",
              location.line, location.col, name.c_str());
	for(int i = 0; i < type->size(); i++){
		if(i==0){
			std::printf("%s ", (*type)[i]);
		}
		else{
			std::printf("[%s]", (*type)[i]);
		}
	}
	std::printf("\n");			
}	

// void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
