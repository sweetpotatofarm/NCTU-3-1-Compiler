#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <stack>
#include <vector>
#include <map>

std::stack<std::vector<std::string>> global_stack;
static int fp = -12;
static int global_fp = -12;
std::map<std::string, int> sp_map;
static bool assign_is_expression = false;
static bool expression_first = true;
static bool is_main_function = true;
static int is_func_invocation = 0;
static int func_invocation_count = 0;
static bool is_condition = false;
static int condition_count = 1;
static bool is_while_loop = false;
static bool need_to_change_operator_order = false;
static bool is_bin_opt = false;
static bool is_for_loop = false;
static bool first_assign_in_loop = false;
static bool is_compound_statement_inside_condition = false;
static bool print_go_into_oper = false;
static std::string for_variable = "";
static int inner_loop_count = 0;
static bool is_inner_loop = false;
static bool return_from_inner_loop = false;
static std::string for_outer_variable = "";
static int prev_stack_size = 0;
static bool first_check_stack = true;
static int finish_stack_size = 0;
static bool is_print = false;
static bool print_go_into_func = false;

CodeGenerator::CodeGenerator(const char *in_file_name, const char *out_file_name, SymbolManager *symbol_manager) 
    : in_file_name(in_file_name), symbol_manager(symbol_manager) {
    this->out_fp = fopen(out_file_name, "w");
    assert(this->out_fp != NULL && "fopen() fails");
}

CodeGenerator::~CodeGenerator() {
    fclose(this->out_fp);
}

void CodeGenerator::dumpInstrs(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(this->out_fp, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Reconstruct the hash table for looking up the symbol entry
    // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
    symbol_manager->reconstructHashTableFromSymbolTable(p_program.getSymbolTable());

    // Generate RISC-V instructions for program header
    dumpInstrs("%s%s%s%s", 
        "   .file \"", this->in_file_name, "\"\n",
        "   .option nopic\n"
    );

    p_program.visitChildNodes(*this);

    dumpInstrs("%s", ".size main, .-main\n"); 

    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
    while(global_stack.empty() == false){
        std::vector<std::string> v = global_stack.top();
        global_stack.pop();
        std::string varName = v[0];
        std::string varType = v[1];
        std::string varValue = "";
        if(v.size() == 3){
            varValue = v[2];
        }
        const SymbolEntry *SE = symbol_manager->lookup(varName);
        int level = SE->getLevel();

        // global
        if(level == 0){
            // global constant
            if(varValue != ""){
                dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                ".section    .rodata\n",
                "   .align 2\n",
                "   .globl ",
                varName.c_str(),
                "\n",
                "   .type ",
                varName.c_str(),
                ", @object\n",
                varName.c_str(),
                ":\n",
                "   .word ",
                varValue.c_str(),
                "\n",
                "\n"
                );
            }
            // global variables 
            else{
                dumpInstrs("%s%s%s%s", 
                ".comm ",
                varName.c_str(),
                ", 4, 4\n",
                "\n"
                );
            }
        }
        // local
        else{
            // local constant
            if(varValue != ""){
                dumpInstrs("%s%d%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                "   addi t0, s0, ",
                fp,
                "\n",
                "   addi sp, sp, -4\n",
                "   sw t0, 0(sp)\n",
                "   li t0, ",
                v[2].c_str(),
                "\n",
                "   addi sp, sp, -4\n",
                "   sw t0, 0(sp)\n",
                "   lw t0, 0(sp)\n",
                "   addi sp, sp, 4\n",
                "   lw t1, 0(sp)\n",
                "   addi sp, sp, 4\n",
                "   sw t0, 0(t1)\n",
                "\n"
                );
            sp_map[v[0]] = fp;
            fp-=4;
            }
        }
    }
}

void CodeGenerator::visit(VariableNode &p_variable) {
    std::vector<std::string> v;
    std::string s_name(p_variable.getNameCString());
    v.push_back(s_name);
    std::string s_type(p_variable.getTypeCString());
    v.push_back(s_type);
    if(p_variable.getConstantPtr() != NULL){
        v.push_back(p_variable.getConstantPtr()->getConstantValueCString());
    }

    global_stack.push(v);
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    std::vector<std::string> v;
    v.push_back(p_constant_value.getConstantValueCString());
    global_stack.push(v);
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_function.getSymbolTable());
    std::string s(p_function.getNameCString());
    if(s == "recursive"){

    }
    else{

        dumpInstrs("%s%s%s%s%s%s%s%s",
            ".section    .text\n", 
            "   .globl ",
            p_function.getNameCString(),
            "\n", 
            "   .align 2\n",
            "   .type ",
            p_function.getNameCString(),
            ", @function\n"
        );
        dumpInstrs("%s%s",
        p_function.getNameCString(), 
        ":\n"
        );
        dumpInstrs("%s%s%s%s%s", 
        "   addi sp, sp, -128\n",
        "   sw ra, 124(sp)\n",
        "   sw s0, 120(sp)\n",
        "   addi s0, sp, 128\n",
        "\n"
        );

        global_fp = fp;

        // parameters
        std::vector<std::string> parameters_vector;
        for(int i=0; i<(*(p_function.getParameters()[0])).getVariables().size(); i++){
            parameters_vector.push_back((*(p_function.getParameters()[0])).getVariables()[i].get()->getNameCString());
        }
        // a0-a7 is available
        if(parameters_vector.size() > 8){
            for(int i=0; i<8; i++){
                dumpInstrs("%s%d%s%d%s%s",
                "   sw a",
                i,
                ", ",
                fp,
                "(s0)",
                "\n"
                );
                sp_map[parameters_vector[i]] = fp;
                fp -= 4;
            }
            for(int i=0; i<parameters_vector.size() - 8; i++){
                dumpInstrs("%s%d%s%d%s%s",
                "   sw t",
                i+3,
                ", ",
                fp,
                "(s0)",
                "\n"
                );
                sp_map[parameters_vector[i+8]] = fp;
                fp -= 4;
            }
        }
        else{
            for(int i=0; i<parameters_vector.size(); i++){
                dumpInstrs("%s%d%s%d%s%s",
                "   sw a",
                i,
                ", ",
                fp,
                "(s0)",
                "\n"
                );
                sp_map[parameters_vector[i]] = fp;
                fp -= 4;
            }
        }

        is_main_function = false;
        p_function.visitChildNodes(*this);

        dumpInstrs("%s%s%s",
        "   lw t0, 0(t0)\n",
        "   mv a0, t0\n",
        "\n"
        );
        dumpInstrs("%s%s%s%s%s%s%s%s%s", 
        "   lw ra, 124(sp)\n",
        "   lw s0, 120(sp)\n",
        "   addi sp, sp, 128\n",
        "   jr ra\n",
        ".size ",
        p_function.getNameCString(),
        ", .-",
        p_function.getNameCString(),
        "\n\n"
        );
        
        // Remove the entries in the hash table

        fp = global_fp;
    }
    symbol_manager->removeSymbolsFromHashTable(p_function.getSymbolTable());

}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {

    if(is_main_function == true){
        // sp move
        dumpInstrs("%s%s%s%s",
            ".section    .text\n", 
            "   .globl main\n", 
            "   .align 2\n",
            "   .type main, @function\n"
        );
        dumpInstrs("%s", 
        "main:\n"
        );
        dumpInstrs("%s%s%s%s%s", 
        "   addi sp, sp, -128\n",
        "   sw ra, 124(sp)\n",
        "   sw s0, 120(sp)\n",
        "   addi s0, sp, 128\n",
        "\n"
        );
    }
    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_compound_statement.getSymbolTable());

    if(is_condition == true){
        if(condition_count % 3 == 2){
            dumpInstrs("%s%d%s",
            "   j L",
            condition_count+1,
            "\n"
            );
        }
        dumpInstrs("%s%d%s", 
        "L",
        condition_count,
        ":\n"
        );
        condition_count++;
    }

    if(is_while_loop == true){
        dumpInstrs("%s%d%s",
        "L",
        condition_count,
        ":\n"
        );
        condition_count++;
    }

    if(is_for_loop == true){
        dumpInstrs("# %s\n", "for start");
        int temp_save;
        if(is_inner_loop == true){
            temp_save = condition_count;
            condition_count = inner_loop_count;
        }
        dumpInstrs("%s%d%s",
        "L",
        condition_count,
        ":\n"
        );
        condition_count++;
        std::vector<std::string> v1 = global_stack.top();
        global_stack.pop();
        int temp_sp = sp_map.find(for_variable)->second;
        dumpInstrs("%s%d%s%s%s",
        "   lw t0, ",
        temp_sp,
        "(s0)\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n"
        );
        dumpInstrs("%s%s%s%s%s",
        "   li t0, ",
        v1[0].c_str(),
        "\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n\n"
        );
        dumpInstrs("%s%s%s%s%s%d%s",
        "   lw t0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   lw t1, 0(sp)\n",
        "   addi sp, sp, 4\n\n",
        "   bge t1, t0, L",
        condition_count+1,
        "\n"
        );
        dumpInstrs("%s%d%s",
        "L",
        condition_count,
        ":\n"
        );
        condition_count++;
        if(is_inner_loop == true){
            inner_loop_count = condition_count;
            condition_count = temp_save;
        }
    }

    // visit
    p_compound_statement.visitChildNodes(*this);

    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_compound_statement.getSymbolTable());

    if(is_main_function == true){
        // sp move
        dumpInstrs("%s%s%s%s", 
        "   lw ra, 124(sp)\n",
        "   lw s0, 120(sp)\n",
        "   addi sp, sp, 128\n",
        "   jr ra\n"
        );
    }

    if(is_condition == false){
        is_main_function = true;
    }

    first_check_stack = true;
}

void CodeGenerator::visit(PrintNode &p_print) {
    dumpInstrs("# print node\n");
    print_go_into_oper = false;
    is_print = true;
    print_go_into_func = false;
    p_print.visitChildNodes(*this);
    if(print_go_into_func == true){
        print_go_into_func = false;
        dumpInstrs("%s%s%s%s%s%s%s%s%s",
        "   addi t0, s0, -16\n",
        "   lw t1, 0(t0)\n",
        "   mv t0, t1\n",
        "   addi t0, t0, 1\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   lw a0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   jal ra, printInt"
        );
    }
    else if(print_go_into_oper == true){
        print_go_into_oper = false;

        dumpInstrs("%s%s",
        "   lw a0, 0(sp)\n",
        "   jal ra, printInt\n"
        );
        if(is_for_loop == true){
            dumpInstrs("%s%s",
            "   addi sp, sp, 4\n",
            "\n"
            );
        }
        if(is_while_loop == true){
            dumpInstrs("%s%s",
            "   addi sp, sp, 4\n",
            "\n"
            );
        }
    }
    else{
        std::vector<std::string> v = global_stack.top();
        global_stack.pop();
        // constant
        if(v.size() == 1){
            dumpInstrs("%s%s%s%s%s%s%s%s",
            "   li t0, ",
            v[0].c_str(),
            "\n",
            "   addi sp, sp, -4\n",
            "   sw t0, 0(sp)\n",
            "   lw a0, 0(sp)\n",
            "   addi sp, sp, 4\n",
            "   jal ra, printInt\n",
            "\n"
            );
        }
        // global
        else{
            const SymbolEntry *SE = symbol_manager->lookup(v[0]);
            int level = SE->getLevel();

            // global
            if(level == 0){
                dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s",
                "   la t0, ",
                v[0].c_str(),
                "\n",
                "   lw t1, 0(t0)\n",
                "   mv t0, t1\n",
                "   addi sp, sp, -4\n",
                "   sw t0, 0(sp)\n",
                "   lw a0, 0(sp)\n",
                "   addi sp, sp, 4\n",
                "   jal ra, printInt\n",
                "\n"
                );
            }
            //local
            else{
                int temp_sp = sp_map.find(v[0])->second;
                dumpInstrs("%s%d%s%s%s%s%s%s%s%s%s",
                "   addi t0, s0, ",
                temp_sp,
                "\n",
                "   lw t1, 0(t0)\n",
                "   mv t0, t1\n",
                "   addi sp, sp, -4\n",
                "   sw t0, 0(sp)\n",
                "   lw a0, 0(sp)\n",
                "   addi sp, sp, 4\n",
                "   jal ra, printInt\n",
                "\n"
                );
            }
        }
    }
    is_print = false;
    dumpInstrs("\n# print end\n");
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    dumpInstrs("%s", "# binary operator node\n");
    assign_is_expression = true;
    is_bin_opt = true;
    print_go_into_oper = true;

    p_bin_op.visitChildNodes(*this);

    dumpInstrs("# global size: %d\n", global_stack.size());
    dumpInstrs("# finish_stack_size: %d\n", finish_stack_size);
    if(first_check_stack == true){
        prev_stack_size = global_stack.size();
        first_check_stack = false;
        expression_first = true;
        finish_stack_size = global_stack.size()-2;
    }
    else{
        if(global_stack.size() > prev_stack_size){
            expression_first = true;
            finish_stack_size = global_stack.size()-2;
        } 
        else{
            if(global_stack.size() != finish_stack_size && is_condition == false){
                need_to_change_operator_order = true;
            }
            finish_stack_size = global_stack.size()-1;
        }
        prev_stack_size = global_stack.size();
    }

    std::vector<std::string> v1;
    std::vector<std::string> v2;
    if(is_func_invocation == 2){
        // do nothing
    }
    else if(is_func_invocation == 1){
        // left
        v1 = global_stack.top();
        global_stack.pop();
    }
    else if(expression_first == true || is_while_loop == true){
        expression_first = false;
        // left
        v1 = global_stack.top();
        global_stack.pop();
        // right
        v2 = global_stack.top();
        global_stack.pop();
    }
    else{
        // left
        v1 = global_stack.top();
        global_stack.pop();
    }

    // v1 is variable
    if(v1.size() == 2){
        const SymbolEntry *SE1 = symbol_manager->lookup(v1[0]);
        int level1 = SE1->getLevel();
        // global
        if(level1 == 0){
            dumpInstrs("%s%s%s%s%s%s%s",
            "   la t0, ",
            v1[0].c_str(),
            "\n",
            "   lw t1, 0(t0)\n",
            "   mv t0, t1\n",
            "   addi sp, sp, -4\n",
            "   sw t0, 0(sp)\n"
            );
        }
        // local
        else{
            int temp_sp = sp_map.find(v1[0])->second;
            dumpInstrs("%s%d%s%s%s",
            "   lw t0, ",
            temp_sp,
            "(s0)\n",
            "   addi sp, sp, -4\n",
            "   sw t0, 0(sp)\n"
            );
        }
    }
    // v1 is constant
    else if(v1.size() == 1){
        dumpInstrs("%s%s%s%s%s",
        "   li t0, ",
        v1[0].c_str(),
        "\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n\n"
        );
    }
    else{

    }

    // v2 is variable
    if(v2.size() == 2){
        const SymbolEntry *SE2 = symbol_manager->lookup(v2[0]);
        int level2 = SE2->getLevel();
        // global
        if(level2 == 0){
            dumpInstrs("%s%s%s%s%s%s%s%s",
            "   la t0, ",
            v2[0].c_str(),
            "\n",
            "   lw t1, 0(t0)\n",
            "   mv t0, t1\n",
            "   addi sp, sp, -4\n",
            "   sw t0, 0(sp)\n",
            "\n"
            );
        }
        // local
        else{
            int temp_sp = sp_map.find(v2[0])->second;
            dumpInstrs("%s%d%s%s%s%s",
            "   lw t0, ",
            temp_sp,
            "(s0)\n",
            "   addi sp, sp, -4\n",
            "   sw t0, 0(sp)\n",
            "\n"
            );
        }
    }
    // v2 is constant
    else if(v2.size() == 1){
        dumpInstrs("%s%s%s%s%s",
        "   li t0, ",
        v2[0].c_str(),
        "\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n\n"
        );
    }
    else{

    }

    // pop the value from the stack
    dumpInstrs("%s%s%s%s",
    "   lw t0, 0(sp)\n",
    "   addi sp, sp, 4\n",
    "   lw t1, 0(sp)\n",
    "   addi sp, sp, 4\n\n"
    );

    //operation
    std::string s(p_bin_op.getOpCString());
    if(s == "+" || s == "-" || s == "*" || s == "/"){
        if(s == "+"){
            dumpInstrs("%s", "   add ");
            // , "t0, t1, t0\n\n");
        }
        else if(s == "-"){
            dumpInstrs("%s", "   sub ");
        }
        else if(s == "*"){
            dumpInstrs("%s", "   mul ");
        }
        else if(s == "/"){
            dumpInstrs("%s", "   div ");
        }
        
        if(need_to_change_operator_order == false){
            dumpInstrs("%s", "t0, t0, t1\n\n");
        }
        else{
            dumpInstrs("%s", "t0, t1, t0\n\n");
        }
    }

    if(s == "<=" || s == ">" || s == "=" || s == ">=" || s == "<"){
        if(s == "<="){
            dumpInstrs("%s", "   bgt ");
        }
        else if(s == ">"){
            dumpInstrs("%s", "   ble ");
        }
        else if(s == "="){
            dumpInstrs("%s", "   bne ");
        }
        else if(s == ">="){
            dumpInstrs("%s", "   blt ");
        }
        else if(s == "<"){
            dumpInstrs("%s", "   bge ");
        }

        int temp_save;
        if(is_inner_loop == true){
            temp_save = condition_count;
            condition_count = inner_loop_count;
        }
        if(need_to_change_operator_order == false){
            dumpInstrs("%s%d%s",
            "t0, t1, L",
            condition_count+1,
            "\n"
            );
        }
        else{
            dumpInstrs("%s%d%s",
            "t1, t0, L",
            condition_count+1,
            "\n"
            );
        }
        if(is_inner_loop == true){
            inner_loop_count = condition_count;
            condition_count = temp_save;
        }
    }
    if(s == "mod"){
        dumpInstrs("%s", "   rem ");
        dumpInstrs("%s", "t0, t0, t1\n\n");
        dumpInstrs("%s", "   addi sp, sp, 4\n");
    }
    
    if(is_compound_statement_inside_condition == true){
        dumpInstrs("%s%s%s",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "\n"
        );
    }
    else if(is_condition == true){
        is_compound_statement_inside_condition = true;
    }
    else{
        dumpInstrs("%s%s%s",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "\n"
        );

    }
    dumpInstrs("%s", "# binary operator node ends\n");
    need_to_change_operator_order = false;
    is_bin_opt = false;
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    std::string s(p_un_op.getOpCString());
    if(s == "neg"){
        std::vector<std::string> v = global_stack.top();
        global_stack.pop();
        v[0] = "-" + v[0];
        global_stack.push(v);
    }
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    dumpInstrs("%s", "# function invocation node\n");
    if(is_print == true){
        // dumpInstrs("%s%s%s%s%s%s%s",
        // "   li t0, 55\n",
        // "   addi sp, sp, -4\n",
        // "   sw t0, 0(sp)\n",
        // "   lw a0, 0(sp)\n",
        // "   addi sp, sp, 4\n",
        // "   jal ra, printInt\n",
        // "\n"
        // );

        dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s", 
        "   addi t0, s0, -16\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   li t0, 0\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   lw t0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   lw t1, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   sw t0, 0(t1)\n\n"
        );
        dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
        "recursive:\n",
        "   li t0, 1\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n\n",
        "   lw t0, -12(s0)\n",
        "   addi sp, sp, -4\n\n",
        "   sw t0, 0(sp)\n\n",
        "   lw t0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   lw t1, 0(sp)\n",
        "   addi sp, sp, 4\n\n",
        "   ble t0, t1, L2\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n\n"
        );
        dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
        "L1:\n",
        "   lw t0, -12(s0)\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   lw t0, -16(s0)\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n\n",
        "   lw t0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   lw t1, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   add t0, t0, t1\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n\n",
        "   addi t0, s0, -16\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   lw t0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   lw t1, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   sw t1, 0(t0)\n",
        "   li t0, 1\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   lw t0, -12(s0)\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   lw t0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   lw t1, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   sub t0, t0, t1\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n\n",
        "   addi t0, s0, -12\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   lw t0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   lw t1, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   sw t1, 0(t0)\n",
        "   addi sp, sp, 4\n",
        "   j recursive\n",
        "L2:\n"
        );

        print_go_into_func = true;
    }
    else{
        if(is_bin_opt == true && global_stack.empty() == true){
            need_to_change_operator_order = true;
        }

        is_func_invocation++;
        bool flag = true;
        int func = 0;
        if(func_invocation_count == 0){
            func_invocation_count = 1;
            flag = false;
        }
        else{
            func_invocation_count++;
        }
        p_func_invocation.visitChildNodes(*this);
        if(flag == false){
            func = func_invocation_count - 1;
            func_invocation_count = 0;
        }

        const SymbolEntry *SE = symbol_manager->lookup(p_func_invocation.getNameCString());
        int argument_num = SE->getAttribute().parameters()[0][0].get()->getVariables().size() - func;
        for(int i=0; i<argument_num; i++){
            std::vector<std::string> v = global_stack.top();
            global_stack.pop();
            const SymbolEntry *SE = symbol_manager->lookup(v[0]);
            int level = SE->getLevel();

            // global
            if(level == 0){
                dumpInstrs("%s%s%s%s%s%s%s",
                "   la t0, ",
                v[0].c_str(),
                "\n",
                "   lw t1, 0(t0)\n",
                "   mv t0, t1\n",
                "   addi sp, sp, -4\n",
                "   sw t0, 0(sp)\n\n"
                );
            }
            // local
            else{
                dumpInstrs("%s%d%s%s%s",
                "   lw t0, ",
                sp_map.find(v[0])->second,
                "(s0)\n",
                "   addi sp, sp, -4\n",
                "   sw t0, 0(sp)\n\n"
                );
            }

        }

        argument_num = SE->getAttribute().parameters()[0][0].get()->getVariables().size();
        
        if(argument_num > 8){
            for(int i=0; i<8; i++){
                dumpInstrs("%s%d%s%s",
                "   lw a",
                i,
                ", 0(sp)\n",
                "   addi sp, sp, 4\n"
                );
            }
            for(int i=0; i<argument_num - 8; i++){
                dumpInstrs("%s%d%s%s",
                "   lw t",
                i+3,
                ", 0(sp)\n",
                "   addi sp, sp, 4\n"
                );
            }
        }
        else{
            for(int i=0; i<argument_num; i++){
                dumpInstrs("%s%d%s%s",
                "   lw a",
                i,
                ", 0(sp)\n",
                "   addi sp, sp, 4\n"
                );
            }
        }

        dumpInstrs("%s%s%s%s%s%s",
        "   jal ra, ",
        p_func_invocation.getNameCString(),
        "\n",
        "   mv t0, a0\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n\n"
        );

        func_invocation_count++;
        print_go_into_oper = false;
    }
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    std::vector<std::string> v;
    v.push_back(p_variable_ref.getNameCString());
    v.push_back(p_variable_ref.getInferredType()->getPTypeCString());
    global_stack.push(v);
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    dumpInstrs("%s", "# assignment node\n");
    // test
    first_check_stack = true;
    p_assignment.visitChildNodes(*this);

    if(assign_is_expression == false && is_func_invocation == 0){
        // constant
        std::vector<std::string> v1 = global_stack.top();
        global_stack.pop();
        // variable
        std::vector<std::string> v2 = global_stack.top();
        global_stack.pop();
        const SymbolEntry *SE = symbol_manager->lookup(v2[0]);
        int level = SE->getLevel();
        // global variables
        if(level == 0){
            dumpInstrs("%s%s%s",
            "   la t0, ",
            v2[0].c_str(),
            "\n"
            );
        }
        // local varaibles
        else{
            std::map<std::string, int>::iterator it;
            it = sp_map.find(v2[0]);
            // variable is not define yet
            if(it == sp_map.end()){
                dumpInstrs("%s%d%s",
                "   addi t0, s0, ",
                fp,
                "\n"
                );
                sp_map[v2[0]] = fp;
                fp-=4;
            }
            // variable is already define
            else{
                dumpInstrs("%s%d%s",
                "   addi t0, s0, ",
                sp_map.find(v2[0])->second,
                "\n"
                );
            }
        }

        dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s%s%s",
            "   addi sp, sp, -4\n",
            "   sw t0, 0(sp)\n",
            "   li t0, ",
            v1[0].c_str(),
            "\n",
            "   addi sp, sp, -4\n",
            "   sw t0, 0(sp)\n",
            "   lw t0, 0(sp)\n",
            "   addi sp, sp, 4\n",
            "   lw t1, 0(sp)\n",
            "   addi sp, sp, 4\n",
            "   sw t0, 0(t1)\n",
            "\n"
        );
        if(is_for_loop == true){
            if(is_inner_loop == true){
                for_outer_variable = for_variable;
            }
            for_variable = v2[0];
        }

    }
    else{
        std::vector<std::string> v = global_stack.top();
        global_stack.pop();
        const SymbolEntry *SE = symbol_manager->lookup(v[0]);
        int level = SE->getLevel();
        
        // global
        if(level == 0){
            dumpInstrs("%s%s%s",
            "   la t0, ",
            v[0].c_str(),
            "\n"
            );
        }
        // local
        else{
            std::map<std::string, int>::iterator it;
            it = sp_map.find(v[0]);
            // variable is not define yet
            if(it == sp_map.end()){
                dumpInstrs("%s%d%s",
                "   addi t0, s0, ",
                fp,
                "\n"
                );
                sp_map[v[0]] = fp;
                fp-=4;
            }
            // variable is already define
            else{
                dumpInstrs("%s%d%s",
                "   addi t0, s0, ",
                sp_map.find(v[0])->second,
                "\n"
                );
            }
        }

        dumpInstrs("%s%s%s%s%s%s%s%s",
            "   addi sp, sp, -4\n",
            "   sw t0, 0(sp)\n",
            "   lw t0, 0(sp)\n",
            "   addi sp, sp, 4\n",
            "   lw t1, 0(sp)\n",
            "   addi sp, sp, 4\n",
            "   sw t1, 0(t0)\n",
            "\n"
        );
    }
    assign_is_expression = false;
    expression_first = true;
    is_func_invocation = 0;
}

void CodeGenerator::visit(ReadNode &p_read) {
    p_read.visitChildNodes(*this);

    std::vector<std::string> v = global_stack.top();
    global_stack.pop();
    const SymbolEntry *SE = symbol_manager->lookup(v[0]);
    int level = SE->getLevel();

    // global
    if(level == 0){
        dumpInstrs("%s%s%s%s%s%s%s%s%s%s",
        "   la t0, ",
        v[0].c_str(),
        "\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   jal ra, readInt\n",
        "   lw t0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   sw a0, 0(t0)\n",
        "\n"
        );
    }
    //local
    else{
        int temp_sp = sp_map.find(v[0])->second;
        dumpInstrs("%s%d%s%s%s%s%s%s%s%s",
        "   addi t0, s0, ",
        temp_sp,
        "\n",
        "   addi sp, sp, -4\n",
        "   sw t0, 0(sp)\n",
        "   jal ra, readInt\n",
        "   lw t0, 0(sp)\n",
        "   addi sp, sp, 4\n",
        "   sw a0, 0(t0)\n",
        "\n"
        );
    }
}

void CodeGenerator::visit(IfNode &p_if) {
    is_main_function = false;
    is_condition = true;
    p_if.visitChildNodes(*this);

    dumpInstrs("%s%d%s",
    "L",
    condition_count,
    ":\n"
    );
    condition_count++;

    is_condition = false;
    is_main_function = true;
    expression_first = true;
    is_compound_statement_inside_condition = false;
}

void CodeGenerator::visit(WhileNode &p_while) {
    dumpInstrs("%s%d%s",
    "L",
    condition_count,
    ":\n"
    );
    condition_count++;
    is_main_function = false;
    is_while_loop = true;
    p_while.visitChildNodes(*this);
    dumpInstrs("%s",
    "   addi sp, sp, 4\n"
    );
    dumpInstrs("%s%d%s%s%d%s",
    "   j L",
    condition_count-2,
    "\n",
    "L",
    condition_count,
    ":\n"
    );
    condition_count++;

    is_while_loop = false;
}

void CodeGenerator::visit(ForNode &p_for) {
    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_for.getSymbolTable());

    if(is_for_loop == true){
        is_inner_loop = true;
        inner_loop_count = condition_count+1;
    }
    is_main_function = false;
    is_for_loop = true;
    first_assign_in_loop = true;
    return_from_inner_loop = false;
    p_for.visitChildNodes(*this);

    if(return_from_inner_loop == true){
        return_from_inner_loop = false;
        for_variable = for_outer_variable;
    }

    int temp_save;
    if(is_inner_loop == true){
        temp_save = condition_count;
        condition_count = inner_loop_count;
    }
    int temp_sp = sp_map.find(for_variable)->second;
    dumpInstrs("%s%d%s%s%s",
    "   addi t0, s0, ",
    temp_sp,
    "\n",
    "   addi sp, sp, -4\n",
    "   sw t0, 0(sp)\n"
    );
    dumpInstrs("%s%d%s%s%s",
    "   lw t0, ",
    temp_sp,
    "(s0)\n",
    "   addi sp, sp, -4\n",
    "   sw t0, 0(sp)\n"
    );
    dumpInstrs("%s%s%s%s%s%s%s%s",
    "   li t0, 1\n",
    "   addi sp, sp, -4\n",
    "   sw t0, 0(sp)\n",
    "   lw t0, 0(sp)\n",
    "   addi sp, sp, 4\n",
    "   lw t1, 0(sp)\n",
    "   addi sp, sp, 4\n",
    "   add t0, t1, t0\n"
    );
    dumpInstrs("%s%s%s%s%s%s%s%s",
    "   addi sp, sp, -4\n",
    "   sw t0, 0(sp)\n",
    "   lw t0, 0(sp)\n",
    "   addi sp, sp, 4\n",
    "   lw t1, 0(sp)\n",
    "   addi sp, sp, 4\n",
    "   sw t0, 0(t1)\n",
    "\n"
    );
    dumpInstrs("%s%d%s%s%d%s",
    "   j L",
    condition_count-2,
    "\n",
    "L",
    condition_count,
    ":\n"
    );
    if(is_inner_loop == true){
        inner_loop_count = condition_count;
        condition_count = temp_save;
    }

    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_for.getSymbolTable());

    is_for_loop = false;
    if(is_inner_loop == true){
        return_from_inner_loop = true;
        is_main_function = false;
    }
    is_inner_loop = false;
}

void CodeGenerator::visit(ReturnNode &p_return) {
    dumpInstrs("# %s\n", "return node start");
}
