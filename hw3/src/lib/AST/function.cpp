#include "AST/function.hpp"
#include "AST/decl.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
                            char* name, vector<DeclNode*>* Parameters,
                            char* returntype, CompoundStatementNode* body
                            )
    : AstNode{line, col}, name(name), Parameters(Parameters), returntype(returntype),
    body(body) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {
    printSpace();
    if(returntype == ""){
		char* temp = "void";
		std::printf("function declaration <line: %u, col: %u> %s %s ",
					location.line, location.col, name.c_str(), temp);
	}
	else{
		std::printf("function declaration <line: %u, col: %u> %s %s ",
					location.line, location.col, name.c_str(), returntype.c_str());
	}

    if(Parameters != NULL){
        std::printf("(");
		for (int i = 0; i < Parameters->size() ; i++ ){
			if(i != 0){
				printf(", ");
			}
			for(int j = 0; j < (*Parameters)[i]->type->size() ; j++){
				for(int k = 0; k < (*Parameters)[i]->variable->size() ; k++ ){
					if(k != 0){
						printf(", ");
					}
					if(j==0){
						std::printf("%s", (*((*Parameters)[i]->type))[j]);
					}
					else{
						if(j == 1)
							std::printf(" [%s]", (*((*Parameters)[i]->type))[j]);
						else
							std::printf("[%s]", (*((*Parameters)[i]->type))[j]);
					}
				}
			}
		}
		std::printf(")\n");
	}
	else{
		std::printf("()\n");
    }

	if(Parameters != NULL){
		for(int i = 0; i < Parameters->size(); i++){
			(*Parameters)[i]->space = this->space + 2;
			(*Parameters)[i]->print();
		}
	}

	if(body != NULL){
		body->space = this->space + 2;
		body->print();
	}
}

// void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
