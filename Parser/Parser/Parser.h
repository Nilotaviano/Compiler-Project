#pragma once

#include "Scanner.h"
#include "Token.h"
#include "list"

typedef std::list<TokenPtr> TokenList;

class Parser
{
private:
  enum  class DeclarationType {
    INTEGER = TokenClassEnum::INTEGER,
    FLOAT = TokenClassEnum::FLOAT,
    CHAR = TokenClassEnum::CHAR
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
  Parser(char *file_name);
  ~Parser();
  void Begin();
  bool IsOver() { return over_; };

private:

  Scanner scanner;
  TokenList tokens_;
  TokenPtr current_token_;
  int current_scope_;
  std::list<Symbol> symbol_table_;
  map<string, DeclarationType> map_string_type_;
  bool over_;

  void ReportSemanticalError(string error);
  void ReportSyntaxError(string error);
  void ReportLexycalError(string error);
  bool LexycalErrorOccurred();
  bool Program();					//<programa> ::= int main"("")" <bloco>
  bool Block();					//<bloco> ::= “{“ {<decl_var>}* {<comando>}* “}”
  bool VariableDeclaration();		//decl_var> :: = <tipo> <id> {, <id>}*;
  bool Type(DeclarationType *p_declaration_type);					//<tipo> :: = int | float | char
  bool Command();					//<comando> ::= <comando_básico> | <iteração> | if "("<expr_relacional>")" <comando> {else <comando>}?
  bool BasicCommand();			//<comando_básico> ::= <atribuição> | <bloco>
  bool Iteration();				//<iteração> ::= while "("<expr_relacional>")" <comando> | do <comando> while "("<expr_relacional>")"";"
  bool Assignment();				//<atribuição> ::= <id> "=" <expr_arit> ";"
  bool RelationalExpression();	//<expr_relacional> ::= <expr_arit> <op_relacional> <expr_arit>
  bool ArithmeticExpression();	//Removed left recursion: <expr_arit> ::= <termo><expr_arit'>			//<expr_arit> ::= <expr_arit> "+" <termo>   | <expr_arit> "-" <termo> | <termo>	- LEFT RECURSIVE
  bool ArithmeticExpressionAlt();	//expr_arit'> ::= <empty> | "+" <termo><expr_arit'> | "-" <termo><expr_arit'> 
  bool Term();					//Removed left recursion: <termo> ::= <fator><termo'>					//<termo> ::= <termo> "*" <fator> | <termo> “/” <fator> | <fator>					- LEFT RECURSIVE
  bool TermAlt();					//	<termo'> ::= <empty> | "*" <fator><termo'> | "/" <fator><termo'>
  bool Factor();					//<fator> ::= “(“ <expr_arit> “)” | <id> | <real> | <inteiro> | <char>

  bool IsInFirst(TokenPtr token, Production production);
  void IncrementScope();
  void DecrementScope();
  void PushSymbolToTable(DeclarationType current_declaration_type);
  bool SymbolExistsOnTable();
};

