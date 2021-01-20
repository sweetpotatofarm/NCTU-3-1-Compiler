#ifndef __SEMA_SEMANTIC_ANALYZER_H
#define __SEMA_SEMANTIC_ANALYZER_H

#include "visitor/AstNodeVisitor.hpp"
#include <iostream>
#include <vector>
#include <stack>
#include <string.h>
#include <stdio.h>

extern std::vector<std::string> readLines;

class SemanticAnalyzer : public AstNodeVisitor {
  public:
    SemanticAnalyzer() = default;
    ~SemanticAnalyzer() = default;

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;


  private:
    // TODO: something like symbol manager (manage symbol tables)
    //       context manager, return type manager

};

class ErrorMessage{
  public:
    int col;
    int line;
    std::string errorLine1;
    std::string errorLine2;
    const char* symbolName;
};

class SymbolEntry{
  public: 
    SymbolEntry() = default;

    void printEntry(){
      printf("%-33s", this->Name);
      printf("%-11s", this->Kind);
      char *s = new char[10];
      strcpy(s, "(");
      strcat(s, this->globalOrLocal);
      strcat(s, ")");
      printf("%d%-10s", this->Level, s);
      printf("%-17s", this->Type);
      printf("%-11s", this->Attribute);
      puts("");
    }

    bool checkArray(){
      bool flag = true;
      int i=0;
      while(Type[i] != '\0'){
        if(Type[i] == '[' && Type[i+1] == '0' && Type[i+2] == ']'){
          flag = false;
          break;
        }
        i++;
      }
      return flag;
    }

  public:
    const char *Name;
    //Kind: program, function, parameter, variable, loop_var, constant
    const char *Kind;
    //Level: 0 for global, 1 for local
    int Level;
    //return type
    const char *Type;
    //integer, real, boolean, string, or the signature of an array
    const char *Attribute;
    const char *globalOrLocal;
};

class SymbolTable{
  public:
    SymbolTable() = default;

    bool checkSymbolTable(SymbolEntry entry){
      bool flag = true;
      for(int i=0; i<entries.size(); i++){
        if(strcmp(entry.Name, entries[i].Name) == 0){
          flag = false;
          break;
        }
      }
      return flag;
    }

    void addSymbol(SymbolEntry entry){
      entries.push_back(entry);
    }

    void dumpDemarcation(const char chr) {
    for (size_t i = 0; i < 110; ++i) {
    printf("%c", chr);
    }
    puts("");
  }

  void dumpSymbol() {
      dumpDemarcation('=');
      printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type",
                                          "Attribute");
      dumpDemarcation('-');

      for(int i=0; i<entries.size(); i++){
        entries[i].printEntry();
      }
      
      dumpDemarcation('-');
  }

  public:
    std::vector<SymbolEntry> entries;
};

class SymbolManager{
  public:
    SymbolManager() = default;

    void pushScope(SymbolTable *new_scope){
      tables.push(new_scope);
    }

    void popScope(){
      tables.pop();
    }

    SymbolTable *topScope(){
      return tables.top();
    }

    void printSpace(int num){
      for(int i=0; i<num; i++){
        printf(" ");
      }
    }

    void printErrorMessage(){
      if(errorMessages.empty()){
        printf("\n"
        "|---------------------------------------------------|\n"
        "|  There is no syntactic error and semantic error!  |\n"
        "|---------------------------------------------------|\n");
      }
      else{
        for(int i=0; i<errorMessages.size(); i++){
          printf("<Error> Found in line %d, column %d: ", errorMessages[i].line, errorMessages[i].col);
          printf("%s", errorMessages[i].errorLine1.c_str());
          char *s = "";
          if(strcmp(errorMessages[i].symbolName, s) != 0){
            printf("'");
            printf("%s", errorMessages[i].symbolName);
            printf("'");
          }
          printf("%s", errorMessages[i].errorLine2.c_str());
          printf("\n");
          printf("    %s\n", readLines[errorMessages[i].line-1].c_str());
          printSpace(errorMessages[i].col-1);
          printf("    ^\n");
        }
      }
    }

    bool checkForVar(SymbolEntry entry){
      bool flag = true;
      std::stack<SymbolTable *> temp;
      while(tables.empty() == false){
        SymbolTable *T = tables.top();
        temp.push(T);
        for(int i=0; i<(*T).entries.size(); i++){
          const char *tt = "loop_var";
          if(strcmp(entry.Name, (*T).entries[i].Name) == 0 && strcmp((*T).entries[i].Kind, tt) == 0){
            flag = false;
            break;
          }
        }
        tables.pop();
        if(flag == false){
          break;
        }
      }
      while(temp.empty() == false){
        tables.push(temp.top());
        temp.pop();
      }
      return flag;
    }

    std::string GetRightArrType(std::string var_name, int num){
      std::string prefix="";
      for(int i=0,flag=0,ff=1;i<var_name.size();i++){
        if(var_name[i] != ' '&& ff){
          prefix += var_name[i];
          //std::cout<<prefix<<std::endl;
        }
        else if(var_name[i] == ' '){
          if(num>0)
            prefix += ' ';
          ff=0;
        }
        else if(num>0 && var_name[i]=='['){
          prefix += var_name[i];
          flag = 1;
          //std::cout<<prefix<<std::endl;
        }
        else if(num > 0 &&var_name[i]==']'){
          prefix += var_name[i];
          flag = 0;
          num--;
        }
        else if(flag){
          prefix += var_name[i];
        }
      }
      return prefix;
    }

    SymbolEntry findSE(const char *SEname){
      SymbolEntry SE;
      SE.Level = -1;
      bool flag = true;
      std::stack<SymbolTable *> temp;
      while(tables.empty() == false){
        SymbolTable *T = tables.top();
        temp.push(T);
        for(int i=0; i<(*T).entries.size(); i++){
          if(strcmp(SEname, (*T).entries[i].Name) == 0){
            SE = (*T).entries[i];
            flag = false;
            break;
          }
        }
        tables.pop();
        if(flag == false){
          break;
        }
      }
      while(temp.empty() == false){
        tables.push(temp.top());
        temp.pop();
      }

      return SE;
    }

  public:
    std::stack<SymbolTable *> tables;
    std::vector<ErrorMessage> errorMessages;
};

#endif
