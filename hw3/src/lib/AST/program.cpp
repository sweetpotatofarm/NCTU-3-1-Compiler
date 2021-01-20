#include "AST/program.hpp"
#include "AST/AstDumper.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char* p_name, const char* returnType,
                         vector<DeclNode*>* var_const_declarations,
                         vector<FunctionNode*>* function_declarations,
                         CompoundStatementNode* body
                         )
    : AstNode{line, col}, name(p_name), returnType(returnType),
    var_const_declarations(var_const_declarations), function_declarations(function_declarations),
    body(body) {}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();
    std::string s = "void";
    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), s.c_str());

    if(var_const_declarations != NULL){
        for(int i=0; i<var_const_declarations->size(); i++){
            (*var_const_declarations)[i]->space = this->space + 2;
            (*var_const_declarations)[i]->print();
        }
    }

    if(function_declarations != NULL){
        for(int i=0; i<function_declarations->size(); i++){
            (*function_declarations)[i]->space = this->space + 2;
            (*function_declarations)[i]->print();
        }
    }

    body->space = this->space + 2;
    body->print();

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
}


// void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
//     /* TODO
//      *
//      * for (auto &decl : var_decls) {
//      *     decl->accept(p_visitor);
//      * }
//      *
//      * // functions
//      *
//      * body->accept(p_visitor);
//      */
// }
