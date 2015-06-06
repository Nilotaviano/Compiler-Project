#pragma once

#include "Scanner.h"
#include "Token.h"
#include "list"

class Parser
{
//Auxiliar structures
private:
  enum  class DeclarationType {
    INTEGER,
    FLOAT,
    CHAR,
    NONE
  };

  struct Symbol{
    int scope;
    std::string name;
    DeclarationType type;
  };

  enum class Production {
    PROGRAM,
    BLOCK,
    VARIABLE_DECLARATION,
    TYPE,
    COMMAND,
    BASIC_COMMAND,
    ITERATION,
    ASSIGNMENT,
    RELATIONAL_EXPRESSION,
    ARITHMETIC_EXPRESSION,
    TERM,
    FACTOR
  };
public:
  Parser(FILE* fp);
  ~Parser();
  void Begin();

private:
  Scanner scanner;
  std::list<TokenPtr> tokens_;
  TokenPtr current_token_;
  int current_scope_;
  std::list<Symbol> symbol_table_;
  map<string, DeclarationType> map_string_type_;

  //Error reporting and checking
  void ReportSemanticalError(string error);
  void ReportSyntaxError(string error);
  void ReportLexycalError(string error);
  bool LexycalErrorOccurred();  //If an error occurred, will also print it. If end of file was found when this is called, this method will call ReportSyntaxError.

  //Parser productions
  bool Program();					                                //<programa> ::= int main"("")" <bloco>
  bool Block();					                                  //<bloco> ::= “{“ {<decl_var>}* {<comando>}* “}”
  bool VariableDeclaration();		                          //decl_var> :: = <tipo> <id> {, <id>}*;
  bool Type(DeclarationType *p_declaration_type);			    //<tipo> :: = int | float | char
  bool Command();					                                //<comando> ::= <comando_básico> | <iteração> | if "("<expr_relacional>")" <comando> {else <comando>}?
  bool If();
  bool BasicCommand();			                              //<comando_básico> ::= <atribuição> | <bloco>
  bool Iteration();				                                //<iteração> ::= while "("<expr_relacional>")" <comando> | do <comando> while "("<expr_relacional>")"";"
  bool While();
  bool DoWhile();
  bool Assignment();				                              //<atribuição> ::= <id> "=" <expr_arit> ";"
  bool RelationalExpression(string *p_rel_expr_code);     //<expr_relacional> ::= <expr_arit> <op_relacional> <expr_arit>
  bool ArithmeticExpression(DeclarationType *my_type);	  //Removed left recursion: <expr_arit> ::= <termo><expr_arit'>			//<expr_arit> ::= <expr_arit> "+" <termo>   | <expr_arit> "-" <termo> | <termo>	- LEFT RECURSIVE
  bool ArithmeticExpressionAlt(DeclarationType *my_type); //expr_arit'> ::= <empty> | "+" <termo><expr_arit'> | "-" <termo><expr_arit'> 
  bool Term(DeclarationType *my_type);					          //Removed left recursion: <termo> ::= <fator><termo'>					//<termo> ::= <termo> "*" <fator> | <termo> “/” <fator> | <fator>					- LEFT RECURSIVE
  bool TermAlt(DeclarationType *my_type);					        //<termo'> ::= <empty> | "*" <fator><termo'> | "/" <fator><termo'>
  bool Factor(DeclarationType *my_type);					        //<fator> ::= “(“ <expr_arit> “)” | <id> | <real> | <inteiro> | <char>

  //Auxiliar methods
  bool IsInFirst(TokenPtr token, Production production);

  //Semantic analysis
  void IncrementScope();
  void DecrementScope();
  void PushSymbolToTable(DeclarationType current_declaration_type);
  bool WasVariableDeclared();
  DeclarationType GetVarType(string var_name);
  bool IsCompatible(DeclarationType l_type, DeclarationType r_type);
  DeclarationType GetHigherType(DeclarationType l_type, DeclarationType r_type);

  // Code generation
  string current_operand_;
  int label_count_;
  int temp_var_count_;

  inline void PrintCode(string code_str);
  inline void PrintLabel(string label);
  inline string GetNextLabel() { return "l" + std::to_string(label_count_++);; }
  inline string GetNextTempVar() { return "t" + std::to_string(temp_var_count_++); }
};

