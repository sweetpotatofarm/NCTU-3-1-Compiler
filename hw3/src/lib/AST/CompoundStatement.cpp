#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             vector<DeclNode*>* var_const_decl,
                                             vector<AstNode*>* Statements
                                             )
    : AstNode{line, col}, var_const_decl(var_const_decl), Statements(Statements) {}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {
    printSpace();
    std::printf("compound statement <line: %u, col: %u>\n",
                location.line,
                location.col);
    if(var_const_decl != NULL){
        for(int i = 0; i < var_const_decl->size(); i++ ){
			(*var_const_decl)[i]->space = this->space + 2;
			(*var_const_decl)[i]->print();
		}
    }
    if(Statements != NULL){
        for(int i = 0; i < Statements->size(); i++ ){
			(*Statements)[i]->space = this->space + 2;
			(*Statements)[i]->print();
		}
    }
}

// void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
