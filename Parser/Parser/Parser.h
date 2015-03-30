#pragma once

#include "Scanner.h"
#include "Token.h"

typedef std::list<TokenPtr> TokenList;

class Parser
{
public:
  Parser(char *file_name);
  ~Parser();
  void Begin();
  bool IsOver() { return over_; };

private:
  Scanner scanner;
  TokenList tokens_;
  TokenPtr current_token_;
  bool over_;

  void ReportSyntaxError(string error);
  void ReportLexycalError(string error);
  bool Program();             //  <programa> ::= int main"("")" <bloco>
  bool Block();               //  <bloco> ::= “{“ {<decl_var>}* {<comando>}* “}”
  bool Command();             //  <comando> ::= <comando_básico> | <iteração> | if "("<expr_relacional>")" <comando> {else <comando>}?
  bool BasicCommand();        //  <comando_básico> ::= <atribuição> | <bloco>
  bool Iteration();           //  <iteração> ::= while "("<expr_relacional>")" <comando> | do <comando> while "("<expr_relacional>")"";"
  bool Assignment();          //  <atribuição> ::= <id> "=" <expr_arit> ";"
  bool RelationalOperation(); //  <expr_relacional> ::= <expr_arit> <op_relacional> <expr_arit>
  bool ArithmeticOperation(); //  <expr_arit> ::= <expr_arit> "+" <termo>   | <expr_arit> "-" <termo> | <termo>
  bool Term();                //  <termo> ::= <termo> "*" <fator> | <termo> “/” <fator> | <fator>
  bool Factor();              //  <fator> ::= “(“ <expr_arit> “)” | <id> | <real> | <inteiro> | <char>
};

