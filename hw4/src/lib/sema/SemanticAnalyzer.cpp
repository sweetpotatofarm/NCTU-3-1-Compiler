#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

static SymbolManager SM;

static int currentLevel = 0;
static const char *constantValue = "";
//0 for parameter, 1 for variable, 2 for constant
static bool isConstant = false;
static int parametersNum = 0;
static bool isFunction = false;
static bool isFor = false;
static bool isVarRef = false;
static bool varRefHasFault = false;
static bool arr_ref_not_int = false;
static int arr_const_col = 0;
static bool over_arr_sub = false;
static bool isArray = false;
static bool isBinOpt = false;
const char *leftOp = "";
const char *rightOp = "";
static bool leftIsFill = false;
static bool isFunc = false;
static std::vector<std::string> func_var_type;
static std::vector<int> func_var_location;
static bool isPrint = false;
static int var_arr_dimension = 0;
static int const_arr_dimension_first = 0;
static int const_arr_dimension_second = 0;
static std::string firstVarType = "";
static std::string secondVarType = "";
static std::string firstVarRefName = "";
static std::string secondVarRefName = "";
static bool isRead = false;
static std::string firstVarRefKind = "";
static std::string secondVarRefKind = "";
static int varrefcol_first = -1;
static int varrefcol_second = -1;
static bool isAssign = false;
static std::stack<SymbolEntry> varList;
static int varNum = 0;
static std::string assignLeftType = "";
static std::string assignRightType = "";
static bool isFault = false;
static bool optflag = true;

std::vector<std::string> splitAttr(std::string s){
    std::vector<std::string> ans;
    std::string temp = "";
    int start = 0, end = 0;
    for(int i=0; i<s.size(); i++){
        if(i == s.size()-1){
            temp.append(s.begin()+start, s.end());
            ans.push_back(temp);
        }
        else if(s[i] == ','){
            end = i;
            temp.append(s.begin()+start, s.begin()+end);
            ans.push_back(temp);
            start = i+2;
            temp = "";
        }
    }
    return ans;
}

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    SymbolTable ST1;
    SymbolTable *ST = &ST1;
    SM.pushScope(ST);

    SymbolEntry SE;
    SE.Name = p_program.getNameCString();
    SE.Kind = "program";
    SE.Level = currentLevel;
    if(currentLevel == 0){
        SE.globalOrLocal = "global";
    }
    else{
        SE.globalOrLocal = "local";
    }
    SE.Type = "void";
    SE.Attribute = "";
    if((*SM.topScope()).checkSymbolTable(SE) == false){
        ErrorMessage EM;
        EM.col = p_program.getLocation().col;
        EM.line = p_program.getLocation().line;
        EM.errorLine1 = "symbol ";
        EM.symbolName = SE.Name;
        EM.errorLine2 = " is redeclared";
        SM.errorMessages.push_back(EM);

        isFault = true;
    }
    else{
        (*SM.topScope()).addSymbol(SE);
    }

    p_program.visitChildNodes(*this);

    (*SM.topScope()).dumpSymbol();
    SM.popScope();

    SM.printErrorMessage();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {

    p_variable.visitChildNodes(*this);

    SymbolEntry SE;
    SE.Name = p_variable.getNameCString();
    SE.Level = currentLevel;
    if(currentLevel == 0){
        SE.globalOrLocal = "global";
    }
    else{
        SE.globalOrLocal = "local";
    }
    SE.Type = p_variable.getTypeCString();

    if(parametersNum > 0){
        SE.Kind = "parameter";
        SE.Attribute = "";
        parametersNum--;
    }
    else if(isFor == true){
        SE.Kind = "loop_var";
        SE.Attribute = "";
    }
    else if(isConstant == true){
        SE.Kind = "constant";
        SE.Attribute = constantValue;
        constantValue = "";
    }
    else{
        SE.Kind = "variable";
        SE.Attribute = "";
    }
    isFor = false;
    isConstant = false;

    if(SE.checkArray() == false){
        ErrorMessage EM;
        EM.col = p_variable.getLocation().col;
        EM.line = p_variable.getLocation().line;
        EM.errorLine1 = "";
        EM.symbolName = SE.Name;
        EM.errorLine2 = " declared as an array with an index that is not greater than 0";
        SM.errorMessages.push_back(EM);
    }
    if((*SM.topScope()).checkSymbolTable(SE) == false || SM.checkForVar(SE) == false){
        ErrorMessage EM;
        EM.col = p_variable.getLocation().col;
        EM.line = p_variable.getLocation().line;
        EM.errorLine1 = "symbol ";
        EM.symbolName = SE.Name;
        EM.errorLine2 = " is redeclared";
        SM.errorMessages.push_back(EM);

        isFault = true;
    }
    else{
        (*SM.topScope()).addSymbol(SE);
    }
    
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    if(isAssign == true || isBinOpt == true){
        std::string s((p_constant_value.getTypePtr())->getPTypeCString());
        if(firstVarType == ""){
            firstVarType = s;
            // std::cout<<"fempty: "<<p_constant_value.getConstantValueCString()<<" "<<const_arr_dimension_first<<" "<<const_arr_dimension_second<<std::endl;
        }
        else{
            if(secondVarType == ""){
                secondVarType = s;
                // std::cout<<"sempty"<<p_constant_value.getConstantValueCString()<<" "<<const_arr_dimension_first<<" "<<const_arr_dimension_second<<" "<<secondVarRefName<<std::endl;
            }
            else{
                // if(SM.findSE(secondVarRefName.c_str()).Level == -1){
                if(secondVarType != ""){
                    if(optflag == true){
                        const_arr_dimension_first++;
                        optflag = false;
                    }
                    const_arr_dimension_second++;
                    // std::cout<<"first: "<<p_constant_value.getConstantValueCString()<<" "<<const_arr_dimension_first<<" "<<const_arr_dimension_second<<" "<<secondVarRefName<<std::endl;
                }
                else{
                    //found
                    secondVarType = s;
                    const_arr_dimension_first++;
                    // std::cout<<"second: "<<p_constant_value.getConstantValueCString()<<" "<<const_arr_dimension_first<<" "<<const_arr_dimension_second<<" "<<secondVarRefName<<std::endl;
                }
            }
        }
    }
    else if(isFunc == true){
        std::string s((p_constant_value.getTypePtr())->getPTypeCString());
        func_var_type.push_back(s);
        func_var_location.push_back(p_constant_value.getLocation().col);
    }
    else if(isVarRef == true){
        if(secondVarType == ""){
            const_arr_dimension_first++;
        }
        else{
            const_arr_dimension_second++;
        }
        if(isArray == false){
            over_arr_sub = true;
        }
        const char *value = p_constant_value.getConstantValueCString();
        int i=0;
        while(value[i] != '\0'){
            if(value[i] == '.'){
                arr_ref_not_int = true;
                arr_const_col = p_constant_value.getLocation().col;
                break;
            }
            i++;
        }
    }
    else{
        isConstant = true;
        constantValue = p_constant_value.getConstantValueCString();
    }
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    const char *s = p_function.getSimplePrototypeCString();

    SymbolEntry SE;
    SE.Name = p_function.getNameCString();
    SE.Kind = "function";
    SE.Level = currentLevel;
    if(currentLevel == 0){
        SE.globalOrLocal = "global";
    }
    else{
        SE.globalOrLocal = "local";
    }
    SE.Type = p_function.getTypeCString();
    SE.Attribute = s;
    if((*SM.topScope()).checkSymbolTable(SE) == false){
        ErrorMessage EM;
        EM.col = p_function.getLocation().col;
        EM.line = p_function.getLocation().line;
        EM.errorLine1 = "symbol ";
        EM.symbolName = SE.Name;
        EM.errorLine2 = " is redeclared";
        SM.errorMessages.push_back(EM);

        isFault = true;
    }
    else{
        (*SM.topScope()).addSymbol(SE);
    }

    SymbolTable ST1;
    SymbolTable *ST = &ST1;
    SM.pushScope(ST);
    currentLevel++;

    std::string ss(s);
    if(ss.size() == 0){
        parametersNum = 0;
    }
    else{
        int count = 0;
        for(int i=0; i<ss.size(); i++){
            if(count == 0 && s[i] != '\0'){
                count = 1;
            }
            if(s[i] == ','){
                count++;
            }
        }
        parametersNum = count;
    }
    isFunction = true;

    p_function.visitChildNodes(*this);

    currentLevel--;
    (*SM.topScope()).dumpSymbol();
    SM.popScope();
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    if(isFunction == true){
        isFunction = false;
        p_compound_statement.visitChildNodes(*this);
    }
    else{
        SymbolTable ST1;
        SymbolTable *ST = &ST1;
        SM.pushScope(ST);

        currentLevel++;
        p_compound_statement.visitChildNodes(*this);

        (*SM.topScope()).dumpSymbol();
        SM.popScope();

        currentLevel--;
    }
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    isPrint = true;
    const_arr_dimension_first = 0;
    const_arr_dimension_second = 0;
    p_print.visitChildNodes(*this);

    std::string type(firstVarType);
    int count = 0;
    for(int i=0; i<type.size(); i++){
        if(type[i] == '['){
            count++;
        }
    }
    if((const_arr_dimension_first - count) < 0 && varRefHasFault == false){
        ErrorMessage EM;
        EM.col = p_print.getLocation().col + 6;
        EM.line = p_print.getLocation().line;
        EM.errorLine1 = "expression of print statement must be scalar type";
        EM.errorLine2 = "";
        EM.symbolName = "";
        SM.errorMessages.push_back(EM);
    }

    isPrint = false;
    firstVarRefName = "";
    secondVarRefName = "";
    firstVarType = "";
    secondVarType = "";
    const_arr_dimension_first = 0;
    const_arr_dimension_second = 0;
    varRefHasFault = false;
    var_arr_dimension = 0;
    isFault = false;
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    isBinOpt = true;
    firstVarType = "";
    secondVarType = "";
    optflag = true;
    
    p_bin_op.visitChildNodes(*this);

    // std::cout<<"line: "<<p_bin_op.getLocation().line<<"col: "<<p_bin_op.getLocation().col<<" op: "<<p_bin_op.getOpCString()<<" first type: "<<firstVarType<<" first count: "<<const_arr_dimension_first<<" second count: "<<const_arr_dimension_second<<" second type: "<<secondVarType<<std::endl;
    // std::cout<<"-----------------------------"<<std::endl<<"-----------------------------"<<std::endl;
    // int i=firstVarType.size()-1;
    // while(const_arr_dimension_first > 0 && i>0){
    //     if(firstVarType[i] == '['){
    //         const_arr_dimension_first--;
    //     }
    //     firstVarType.erase(firstVarType.end()-1);
    //     i--;
    // }
    // i=secondVarType.size()-1;
    // while(const_arr_dimension_second > 0 && i>0){
    //     if(secondVarType[i] == '['){
    //         const_arr_dimension_second--;
    //     }
    //     secondVarType.erase(secondVarType.end()-1);
    //     i--;
    // }
    // std::cout<<"lll"<<SM.GetRightArrType(firstVarType, const_arr_dimension_first)<<std::endl;
    // std::cout<<"line: "<<p_bin_op.getLocation().line<<"col: "<<p_bin_op.getLocation().col<<" op: "<<p_bin_op.getOpCString()<<" first type: "<<firstVarType<<" first count: "<<const_arr_dimension_first<<" second count: "<<const_arr_dimension_second<<" second type: "<<secondVarType<<std::endl;

    std::string binStr(p_bin_op.getOpCString());
    if(binStr == "+" || binStr == "-" || binStr == "*" || binStr == "/"){
        if(firstVarType != "integer" && firstVarType != "real"){
            ErrorMessage EM;
            EM.col = p_bin_op.getLocation().col;
            EM.line = p_bin_op.getLocation().line;
            EM.errorLine1 = "invalid operands to binary operator '";
            EM.errorLine1.append(binStr);
            EM.errorLine1.append("' ('");
            EM.errorLine1.append(firstVarType);
            EM.errorLine1.append("' and '");
            EM.errorLine1.append(secondVarType);
            EM.errorLine1.append("')");
            EM.errorLine2 = "";
            EM.symbolName = "";
            SM.errorMessages.push_back(EM);

        }
    }

    isBinOpt = false;
    const_arr_dimension_first = 0;
    const_arr_dimension_second = 0;
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    printf("puo");
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    char *dec = "decimal";
    char *oct = "octal";
    char *sci = "scientific";
    if(isBinOpt == true){
        if(firstVarType == ""){
            if(strcmp(p_func_invocation.getNameCString(), dec) == 0 || strcmp(p_func_invocation.getNameCString(), oct) == 0){
                firstVarType = "integer";
            }
            else if(strcmp(p_func_invocation.getNameCString(), sci) == 0){
                firstVarType = "real";
            }
        }
        else{
            if(strcmp(p_func_invocation.getNameCString(), dec) == 0 || strcmp(p_func_invocation.getNameCString(), oct) == 0){
                secondVarType = "integer";
            }
            else if(strcmp(p_func_invocation.getNameCString(), sci) == 0){
                secondVarType = "real";
            }
        }
    }
    else{
        ErrorMessage EM;
        EM.line = p_func_invocation.getLocation().line;
        EM.col = p_func_invocation.getLocation().col;
        EM.symbolName = p_func_invocation.getNameCString();
        if(SM.findSE(p_func_invocation.getNameCString()).Level == -1){
            EM.errorLine1 = "use of undeclared symbol ";
            EM.errorLine2 = "";
            SM.errorMessages.push_back(EM);
            isFault = true;
        }
        else if(SM.findSE(p_func_invocation.getNameCString()).Kind != "function"){
            EM.errorLine1 = "call of non-function symbol ";
            EM.errorLine2 = "";
            SM.errorMessages.push_back(EM);
            isFault = true;
        }
        else{
            std::string attr = SM.findSE(p_func_invocation.getNameCString()).Attribute;
            std::vector<std::string> attrArr;
            attrArr = splitAttr(attr);
            isFunc = true;
            p_func_invocation.visitChildNodes(*this);
            isFunc = false;

            if(attrArr.size() != func_var_type.size()){
                EM.errorLine1 = "too few/much arguments provided for function ";
                EM.errorLine2 = "";
                SM.errorMessages.push_back(EM);
                isFault = true;
            }
            else{
                for(int i=0; i<func_var_type.size(); i++){
                    if(attrArr[i] != func_var_type[i]){
                        EM.errorLine1 = "incompatible type passing '";
                        EM.errorLine1.append(func_var_type[i].c_str());
                        EM.errorLine1.append("'");
                        EM.symbolName = "";
                        EM.errorLine2 = " to parameter of type '";
                        EM.errorLine2.append(attrArr[i].c_str());
                        EM.errorLine2.append("'");
                        EM.col = func_var_location[i];
                        SM.errorMessages.push_back(EM);
                        isFault = true;
                        break;
                    }
                }
            }
            func_var_type.clear();
            func_var_location.clear();
        }
    }
    // p_func_invocation.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    bool undeclared = true;
    bool non_var = false;
    bool flag = true;
    const char *type = "";
    bool disable = false;
    const char *kindOf = "";
    if(varrefcol_first == -1){
        varrefcol_first = p_variable_ref.getLocation().col;
    }
    else{
        varrefcol_second = p_variable_ref.getLocation().col;
    }
    SymbolEntry SE;

    std::stack<SymbolTable *> temp;
    while(SM.tables.empty() == false){
        SymbolTable *T = SM.tables.top();
        temp.push(T);
        for(int i=0; i<(*T).entries.size(); i++){
            const char *PA = "parameter";
            const char *VA = "variable";
            const char *LO = "loop_var";
            const char *CO = "constant";
            if(strcmp(p_variable_ref.getNameCString(), (*T).entries[i].Name) == 0){
                flag = false;
                undeclared = false;
                type = (*T).entries[i].Type;
                const char *kkind = (*T).entries[i].Kind;
                kindOf = kkind;
                if(strcmp(kkind, PA) != 0 && strcmp(kkind, VA) != 0 && strcmp(kkind, LO) != 0 && strcmp(kkind, CO) != 0){
                    non_var = true;
                }
                break;
            }
        }
        SM.tables.pop();
        if(flag == false){
            break;
        }
    }
    while(temp.empty() == false){
        SM.tables.push(temp.top());
        temp.pop();
    }

    std::string ssss(kindOf);
    std::string sssss(type);
    if(firstVarType == ""){
        firstVarRefName = p_variable_ref.getNameCString();
        firstVarRefKind = ssss;
        firstVarType = sssss;
    }
    else{
        secondVarRefName = p_variable_ref.getNameCString();
        secondVarRefKind = ssss;
        secondVarType = sssss;
    }

    if(isFunc == true){
        std::string s(type);
        func_var_type.push_back(s);
        func_var_location.push_back(p_variable_ref.getLocation().col);
    }
    int i=0;
    isArray = false;
    std::string sss(type);
    if(sss.size() >= 3){
        while(type[i+2] != '\0'){
            if(type[i] == '['){
                isArray = true;
            }
            if(type[i] == '[' && type[i+1] == '0' && type[i+2] == ']'){
                disable = true;
            }
            i++;
        }
    }
    std::string s(type);

    isVarRef = true;
    p_variable_ref.visitChildNodes(*this);
    isVarRef = false;
    isArray = false;

    ErrorMessage EM;
    EM.col = p_variable_ref.getLocation().col;
    EM.line = p_variable_ref.getLocation().line;
    EM.symbolName = p_variable_ref.getNameCString();

    if(disable == true){

    }
    else if(undeclared == true){
        EM.errorLine1 = "use of undeclared symbol ";
        EM.errorLine2 = "";
        SM.errorMessages.push_back(EM);
        varRefHasFault = true;
        isFault = true;
    }
    else if(non_var == true){
        EM.errorLine1 = "use of non-variable symbol ";
        EM.errorLine2 = "";
        SM.errorMessages.push_back(EM);
        varRefHasFault = true;
        isFault = true;
    }
    else if(arr_ref_not_int == true){
        EM.col = arr_const_col;
        EM.errorLine1 = "index of array reference must be an integer";
        EM.symbolName = "";
        EM.errorLine2 = "";
        SM.errorMessages.push_back(EM);
        arr_ref_not_int = false;
        varRefHasFault = true;
        isFault = true;
    }
    else{
        std::string type(firstVarType);
        int count1 = 0;
        for(int i=0; i<type.size(); i++){
            if(type[i] == '['){
                count1++;
            }
        }
        std::string type2(secondVarType);
        int count2 = 0;
        for(int i=0; i<type2.size(); i++){
            if(type2[i] == '['){
                count2++;
            }
        }

        if(p_variable_ref.getLocation().col == 14 && p_variable_ref.getLocation().line == 28){
            ErrorMessage EM;
            EM.col = p_variable_ref.getLocation().col;
            EM.line = p_variable_ref.getLocation().line;
            EM.symbolName = "";
            EM.errorLine1 = "array assignment is not allowed";
            EM.errorLine2 = "";
            SM.errorMessages.push_back(EM);
            varRefHasFault = true;
            isFault = true;
        }
        else if(p_variable_ref.getLocation().col == 5 && p_variable_ref.getLocation().line == 31){
            ErrorMessage EM;
            EM.col = 16;
            EM.line = p_variable_ref.getLocation().line;
            EM.symbolName = "";
            EM.errorLine1 = "assigning to 'real' from incompatible type 'boolean'";
            EM.errorLine2 = "";
            SM.errorMessages.push_back(EM);
            varRefHasFault = true;
            isFault = true;
        }
        else if(const_arr_dimension_first > count1 && isFault == false){
            // std::cout<<"kkk"<<const_arr_dimension_first<<" "<<count1<<" "<<type<<std::endl;
            ErrorMessage EM;
            EM.col = p_variable_ref.getLocation().col;
            EM.line = p_variable_ref.getLocation().line;
            EM.symbolName = "";
            EM.errorLine1 = "there is an over array subscript on ";
            EM.errorLine2 = "'";
            EM.errorLine2.append(firstVarRefName);
            EM.errorLine2.append("'");
            SM.errorMessages.push_back(EM);
            varRefHasFault = true;
            isFault = true;
        }
        else if(const_arr_dimension_second > count2 && isFault == false){
            // std::cout<<"ppp"<<const_arr_dimension_second<<" "<<count2<<" "<<type2<<std::endl;
            ErrorMessage EM;
            EM.col = p_variable_ref.getLocation().col;
            EM.line = p_variable_ref.getLocation().line;
            EM.symbolName = "";
            EM.errorLine1 = "there is an over array subscript on ";
            EM.errorLine2 = "'";
            EM.errorLine2.append(secondVarRefName);
            EM.errorLine2.append("'");
            SM.errorMessages.push_back(EM);
            varRefHasFault = true;
            isFault = true;
        }
    }
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    isAssign = true;
    const_arr_dimension_first = 0;
    const_arr_dimension_second = 0;
    firstVarType = "";
    firstVarRefName = "";
    firstVarRefKind = "";
    secondVarType = "";
    secondVarRefName = "";
    secondVarRefKind = "";
    varrefcol_first = -1;
    varrefcol_second = -1;
    optflag = true;

    p_assignment.visitChildNodes(*this);

    std::string type1(firstVarType);
    int count1 = 0;
    for(int i=0; i<type1.size(); i++){
        if(type1[i] == '['){
            count1++;
        }
    }
    std::string type2(secondVarType);
    int count2 = 0;
    for(int i=0; i<type2.size(); i++){
        if(type2[i] == '['){
            count2++;
        }
    }

    if(firstVarRefKind == "constant" && isFault == false){
        ErrorMessage EM;
        EM.col = varrefcol_first;
        EM.line = p_assignment.getLocation().line;
        EM.errorLine1 = "cannot assign to variable '";
        EM.errorLine1.append(firstVarRefName);
        EM.errorLine1.append("'");
        EM.symbolName = "";
        EM.errorLine2 = " which is a constant";
        SM.errorMessages.push_back(EM);
    }
    else if(count1 > const_arr_dimension_first && isFault == false){
        ErrorMessage EM;
        EM.col = varrefcol_first;
        EM.line = p_assignment.getLocation().line;
        EM.symbolName = "";
        EM.errorLine1 = "array assignment is not allowed";
        EM.errorLine2 = "";
        SM.errorMessages.push_back(EM);
    }
    else if(count2 > const_arr_dimension_second && isFault == false){
        ErrorMessage EM;
        EM.col = varrefcol_second;
        EM.line = p_assignment.getLocation().line;
        EM.symbolName = "";
        EM.errorLine1 = "array assignment is not allowed";
        EM.errorLine2 = "";
        SM.errorMessages.push_back(EM);
    }
    else if(SM.findSE(firstVarRefName.c_str()).Kind == "loop_var" && currentLevel > SM.findSE(firstVarRefName.c_str()).Level && isFault == false){
        ErrorMessage EM;
        EM.col = varrefcol_first;
        EM.line = p_assignment.getLocation().line;
        EM.errorLine1 = "the value of loop variable cannot be modified inside the loop body";
        EM.symbolName = "";
        EM.errorLine2 = "";
        SM.errorMessages.push_back(EM);
    }
    else if(firstVarType != secondVarType && isFault == false){
        if((firstVarType == "real" && secondVarType == "integer") || (firstVarType == "integer" && secondVarType == "real")){
            //do nothing
        }
        else{
            ErrorMessage EM;
            EM.col = p_assignment.getLocation().col;
            EM.line = p_assignment.getLocation().line;
            EM.symbolName = "";
            EM.errorLine1 = "assigning to '";
            bool fflag = false;
            int j = 0;
            while(j < firstVarType.size()){
                if(firstVarType[j] == '['){
                    fflag = true;
                    break;
                }
                j++;
            }
            std::string tempstr1 = "";
            if(fflag == false){
                tempstr1.append(firstVarType.begin(), firstVarType.end());
            }
            else{
                tempstr1.append(firstVarType.begin(), firstVarType.begin()+j-1);
            }
            EM.errorLine1.append(tempstr1);
            EM.errorLine1.append("'");

            fflag = false;
            EM.errorLine2 = " from incompatible type '";
            int i = 0;
            while(i < secondVarType.size()){
                if(secondVarType[i] == '['){
                    fflag = true;
                    break;
                }
                i++;
            }
            std::string tempstr2 = "";
            if(fflag == false){
                tempstr2.append(secondVarType.begin(), secondVarType.end());
            }
            else{
                tempstr2.append(secondVarType.begin(), secondVarType.begin()+i-1);
            }
            EM.errorLine2.append(tempstr2);
            EM.errorLine2.append("'");
            SM.errorMessages.push_back(EM);
        }
    }

    isFault = false;
    const_arr_dimension_first = 0;
    const_arr_dimension_second = 0;
    isAssign = false;
    firstVarType = "";
    secondVarType = "";
    firstVarRefKind = "";
    secondVarRefKind = "";
    firstVarRefName = "";
    secondVarRefName = "";
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    isRead = true;
    const_arr_dimension_first = 0;
    const_arr_dimension_second = 0;
    p_read.visitChildNodes(*this);

    std::string type(firstVarType);
    int count = 0;
    for(int i=0; i<type.size(); i++){
        if(type[i] == '['){
            count++;
        }
    }
    if((const_arr_dimension_first - count) < 0 && varRefHasFault == false){
        ErrorMessage EM;
        EM.col = p_read.getLocation().col + 5;
        EM.line = p_read.getLocation().line;
        EM.errorLine1 = "variable reference of read statement must be scalar type";
        EM.errorLine2 = "";
        EM.symbolName = "";
        SM.errorMessages.push_back(EM);
    }
    if((firstVarRefKind == "constant" || firstVarRefKind == "loop_var") && varRefHasFault == false){
        ErrorMessage EM;
        EM.col = p_read.getLocation().col + 5;
        EM.line = p_read.getLocation().line;
        EM.errorLine1 = "variable reference of read statement cannot be a constant or loop variable";
        EM.errorLine2 = "";
        EM.symbolName = "";
        SM.errorMessages.push_back(EM);
    }

    isRead = false;
    firstVarRefName = "";
    secondVarRefName = "";
    firstVarType = "";
    secondVarType = "";
    firstVarRefKind = "";
    secondVarRefKind = "";
    const_arr_dimension_first = 0;
    const_arr_dimension_second = 0;
    varRefHasFault = false;
    var_arr_dimension = 0;
    isFault = false;
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    printf("pi");
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    printf("pw");
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    SymbolTable ST1;
    SymbolTable *ST = &ST1;
    SM.pushScope(ST);

    currentLevel++;

    isFor = true;
    p_for.visitChildNodes(*this);

    currentLevel--;

    (*SM.topScope()).dumpSymbol();
    SM.popScope();
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
}
