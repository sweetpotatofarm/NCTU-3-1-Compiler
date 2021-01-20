#ifndef __AST_H
#define __AST_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

using namespace std;

struct Location {
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}

    uint32_t line;
    uint32_t col;
};

class AstNode {
  public:
    AstNode(const uint32_t line, const uint32_t col);
    virtual ~AstNode() = 0;

    const Location &getLocation() const;
    virtual void print() = 0;

    int space = 0;

    void printSpace(){
      for(int i=0; i<space; i++){
        std::printf(" ");
      }
    }

  protected:
    const Location location;
};

// enum bin_OperatorType{
//   op_plus,
//   op_minus,
//   op_multiply,
//   op_divide,
//   op_mod,
//   op_assign,
//   op_less,
//   op_less_or_equal,
//   op_not_equal,
//   op_greater_or_equal,
//   op_greater,
//   op_equal,
//   op_and,
//   op_or
// };

// enum una_OperatorType{
//   opt_not,
//   opt_minus
// };

// enum typeSet{
//   scalar_type,
//   arr_type,
//   constant_type
// };

// enum type{
//   var_integer,
//   var_real,
//   var_string,
//   var_boolean,
//   var_void
// };

// typedef struct Variable_Info{
//     typeSet var_type_set;
//     type var_type;
//     int int_literal;
//     double real_literal;
//     std::string string_literal;
//     bool boolean_literal;
//     vector<int> array_literal;
// } VariableInfo;

// struct nodeWithType{
//     AstNode* Node;
//     VariableInfo* type;
//     int size;
// };

// struct id_node{
//     std::string name;
//     int first_line;
//     int first_column;
// };

// typedef vector<class AstNode*> nodeList;

#endif
