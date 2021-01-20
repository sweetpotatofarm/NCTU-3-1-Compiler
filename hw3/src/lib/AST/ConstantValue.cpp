#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                    char* value, char* type
                                    )
    : ExpressionNode{line, col}, value(value), type(type) {}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {
    printSpace();
    std::printf("constant <line: %u, col: %u> ",
                location.line,
                location.col
                );
    if(type == "real"){
        printf("%f\n", atof(value.c_str()));
    }
    else{
        for(int i=0; i<value.length(); i++){
            if(value[i] != '"'){
                printf("%c", value[i]);
            }
        }
        printf("\n");
    }
}

