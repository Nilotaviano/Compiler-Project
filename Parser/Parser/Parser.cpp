#include "Parser.h"

#include <stdio.h>
#include <iostream>
#include <algorithm>


Parser::Parser(char *file_name)
:scanner(fopen(file_name, "r")),
over_(false), current_scope_(0)
{
  map_string_type_["int"] = DeclarationType::INTEGER;
  map_string_type_["float"] = DeclarationType::FLOAT;
  map_string_type_["char"] = DeclarationType::CHAR;
}

Parser::~Parser()
{
}

void Parser::Begin()
{
  if (Program()) {
    cout << "Ok!\n";
  }
  else {
    cout << "Erro(s) encontrado(s)!\n";
  }
}

void Parser::ReportSemanticalError(string error)
{
  cout << "ERRO de sintaxe na linha " + to_string(scanner.GetCurrentLine()) + " coluna " + to_string(scanner.GetCurrentColumn()) + ", ultimo token lido '" + current_token_->ToString() + "': " + error + "\n";
}

void Parser::ReportSyntaxError(string error)
{
  cout << "ERRO de sintaxe na linha " + to_string(scanner.GetCurrentLine()) + " coluna " + to_string(scanner.GetCurrentColumn()) + ", ultimo token lido '" + current_token_->ToString() + "': " + error + "\n";
}

void Parser::ReportLexycalError(string error)
{

}

bool Parser::LexycalErrorOccurred()
{
  if (current_token_ == nullptr) {
    cout << scanner.GetError() << "\n";
    return true;
  }
  else {
    return false;
  }
}

bool Parser::Program()
{
  current_token_ = scanner.GetNextToken();

  //Error
  if (current_token_ == nullptr) {
    cout << scanner.GetError() << "\n";
    return false;
  }
  else if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD && current_token_->get_lexeme() == "int") {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (current_token_ == nullptr) {
      cout << scanner.GetError() << "\n";
      return false;
    }
    else if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD && current_token_->get_lexeme() == "main") {
      tokens_.push_back(current_token_);
      current_token_ = scanner.GetNextToken();

      if (current_token_ == nullptr) {
        cout << scanner.GetError() << "\n";
        return false;
      }
      else if (current_token_->get_token_class() == TokenClassEnum::L_PAREN) {
        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (current_token_ == nullptr) {
          cout << scanner.GetError() << "\n";
          return false;
        }
        else if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
          tokens_.push_back(current_token_);
          current_token_ = scanner.GetNextToken();

          if (current_token_ == nullptr) {
            cout << scanner.GetError() << "\n";
            return false;
          }
          else if (Block()) {
            return true;
          }
          else {
            return false;
          }
        }
        else {
          ReportSyntaxError("Esperado o simbolo '('");
          return false;
        }
      }
      else {
        ReportSyntaxError("Esperado o simbolo '('");
        return false;
      }
    }
    else {
      ReportSyntaxError("Esperada a palavra-chave 'main'");
      return false;
    }
  }
  else {
    ReportSyntaxError("Esperada a palavra-chave 'int'");
    return false;
  }
}

bool Parser::Block()
{
  if (current_token_->get_token_class() == TokenClassEnum::L_BRACE) {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();
    IncrementScope();

    if (!LexycalErrorOccurred()) {

      while (IsInFirst(current_token_, Production::VARIABLE_DECLARATION)) {
        if (!VariableDeclaration()) {
          return false;
        }

        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (LexycalErrorOccurred()) {
          return false;
        }
      }

      while (IsInFirst(current_token_, Production::COMMAND)) {
        if (!Command()) {
          return false;
        }

        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (LexycalErrorOccurred()) {
          return false;
        }
      }

      if (current_token_->get_token_class() == TokenClassEnum::R_BRACE) {
        DecrementScope();
        return true;
      }
      else {
        ReportSyntaxError("Esperado o simbolo '}'");
        return false;
      }
    }
    else {
      return false;
    }
  }
  else {
    ReportSyntaxError("Esperado o simbolo '{'");
    return false;
  }
}

//TODO checar se o nome da variavel ja foi declarada no escopo
bool Parser::VariableDeclaration()
{
  DeclarationType current_declaration_type;

  if (Type(&current_declaration_type)) {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();



    if (!LexycalErrorOccurred()) {
      if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER) {

        if (SymbolExistsOnTable()) {
          ReportSemanticalError("Variável já declarada no mesmo escopo.");
          return false;
        }

        PushSymbolToTable(current_declaration_type);

        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (!LexycalErrorOccurred()) {
          while (current_token_->get_token_class() == TokenClassEnum::COMMA) {

            tokens_.push_back(current_token_);
            current_token_ = scanner.GetNextToken();

            if (!LexycalErrorOccurred()) {

              if (current_token_->get_token_class() != IDENTIFIER) {
                ReportSyntaxError("Esperado um idenficador");
                return false;
              }
              else {

                if (SymbolExistsOnTable()) {
                  ReportSemanticalError("Variável já declarada no mesmo escopo.");
                  return false;
                }

                PushSymbolToTable(current_declaration_type);

                tokens_.push_back(current_token_);
                current_token_ = scanner.GetNextToken();

                if (LexycalErrorOccurred()) {
                  return false;
                }
              }
            }
            else {
              return false;
            }
          }//while comma

          if (current_token_->get_token_class() == TokenClassEnum::SEMICOLON) {
            return true;
          }
          else {
            ReportSyntaxError("Esperado o simbolo ';'");
            return false;
          }
        }
        else {
          return false;
        }
      }
      else {
        ReportSyntaxError("Esperado ao menos um identificador");
        return false;
      }
    }
    else {
      return false;
    }
  }
  return false;
}

bool Parser::Type(DeclarationType *p_declaration_type)
{
  if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD) {
    if (current_token_->get_lexeme() == "int" || current_token_->get_lexeme() == "float" || current_token_->get_lexeme() == "char") {
      *p_declaration_type = map_string_type_[current_token_->get_lexeme()];
      return true;
    }
  }
  return false;
}

bool Parser::Command()
{
  if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER || current_token_->get_token_class() == TokenClassEnum::L_BRACE) {
    return BasicCommand();
  }
  else if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD) {

    if (current_token_->get_lexeme() == "while" || current_token_->get_lexeme() == "do") {
      return Iteration();
    }
    else if (current_token_->get_lexeme() == "if") {
      tokens_.push_back(current_token_);
      current_token_ = scanner.GetNextToken();

      if (!LexycalErrorOccurred()) {
        if (current_token_->get_token_class() == TokenClassEnum::L_PAREN) {
          tokens_.push_back(current_token_);
          current_token_ = scanner.GetNextToken();

          if (!LexycalErrorOccurred()) {
            if (RelationalExpression()) {
              if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
                tokens_.push_back(current_token_);
                current_token_ = scanner.GetNextToken();

                if (!LexycalErrorOccurred()) {
                  if (Command()) {
                    tokens_.push_back(current_token_);
                    current_token_ = scanner.GetNextToken();

                    if (!LexycalErrorOccurred()) {
                      if (current_token_->get_lexeme() == "else") {
                        tokens_.push_back(current_token_);
                        current_token_ = scanner.GetNextToken();

                        if (!LexycalErrorOccurred()) {
                          return Command();
                        }
                        else {
                          return false;
                        }
                      }
                      else {
                        scanner.UngetToken(current_token_);

                        current_token_ = tokens_.back();
                        tokens_.pop_back();
                        return true;
                      }
                    }
                    else {
                      return false;
                    }
                  }
                  else {
                    ReportSyntaxError("Esperado ao menos um comando");
                    return false;
                  }
                }
                else {
                  return false;
                }
              }
              else {
                ReportSyntaxError("Esperado o simbolo ')'");
                return false;
              }
            }
            else {
              ReportSyntaxError("Esperada uma expressão relacional");
              return false;
            }
          }
          else {
            return false;
          }
        }
        else {
          ReportSyntaxError("Esperado o simbolo '('");
          return false;
        }
      }
      else {
        return false;
      }
    }
  }

  ReportSyntaxError("Comando inválido, esperado um comando básico, uma iteração ou um 'if'");
  return false;
}

bool Parser::BasicCommand()
{
  if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER) {
    return Assignment();
  }
  else if (current_token_->get_token_class() == TokenClassEnum::L_BRACE) {
    return Block();
  }
  return false;
}

bool Parser::Iteration()
{
  if (current_token_->get_lexeme() == "while") {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      if (current_token_->get_token_class() == TokenClassEnum::L_PAREN) {
        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (!LexycalErrorOccurred()) {
          if (RelationalExpression()) {
            //tokens_.push_back(current_token_);
            //current_token_ = scanner.GetNextToken();

            if (!LexycalErrorOccurred()) {
              if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
                tokens_.push_back(current_token_);
                current_token_ = scanner.GetNextToken();

                if (!LexycalErrorOccurred()) {
                  return Command();
                }
              }
              else{
                ReportSyntaxError("Esperado o simbolo ')'");
              }
            }
          }
        }
      }
      ReportSyntaxError("Esperado o simbolo '('");
    }
  }
  else if (current_token_->get_lexeme() == "do") {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      if (Command()) {
        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (!LexycalErrorOccurred()) {
          if (current_token_->get_lexeme() == "while") {
            tokens_.push_back(current_token_);
            current_token_ = scanner.GetNextToken();

            if (!LexycalErrorOccurred()) {
              if (current_token_->get_token_class() == TokenClassEnum::L_PAREN) {
                tokens_.push_back(current_token_);
                current_token_ = scanner.GetNextToken();

                if (!LexycalErrorOccurred()) {
                  if (RelationalExpression()) {
                    //tokens_.push_back(current_token_);
                    //current_token_ = scanner.GetNextToken();

                    if (!LexycalErrorOccurred()) {
                      if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
                        tokens_.push_back(current_token_);
                        current_token_ = scanner.GetNextToken();

                        if (!LexycalErrorOccurred()) {
                          if (current_token_->get_token_class() == TokenClassEnum::SEMICOLON) {
                            return true;
                          }
                          else {
                            ReportSyntaxError("Esperado ';' no final de uma expressão do while");
                          }
                        }
                      }
                      else {
                        ReportSyntaxError("Esperado o simbolo ')'");
                      }
                    }
                  }
                }
              }
              ReportSyntaxError("Esperado o simbolo '('");
            }
          }
          ReportSyntaxError("Esperada a palavra reservada 'while'");
        }
      }
    }
  }
  return false;
}

bool Parser::Assignment()
{
  if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER) {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      if (current_token_->get_token_class() == TokenClassEnum::ASSIGNMENT) {
        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (!LexycalErrorOccurred()) {
          if (ArithmeticExpression()) {
            //tokens_.push_back(current_token_);
            //current_token_ = scanner.GetNextToken();

            if (!LexycalErrorOccurred()) {
              if (current_token_->get_token_class() == TokenClassEnum::SEMICOLON) {
                return true;
              }
              else {
                ReportSyntaxError("Esperado o simbolo ';'");
              }
            }
          }
        }
      }
      else {
        ReportSyntaxError("Esperado o simbolo '=' para atribuição");
      }
    }
  }
  return false;
}

bool Parser::RelationalExpression()
{
  if (ArithmeticExpression()) {
    //tokens_.push_back(current_token_);
    //current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      if (current_token_->get_token_class() == TokenClassEnum::EQUALS ||
        current_token_->get_token_class() == TokenClassEnum::LESS ||
        current_token_->get_token_class() == TokenClassEnum::LESS_OR_EQUAL ||
        current_token_->get_token_class() == TokenClassEnum::GREATER ||
        current_token_->get_token_class() == TokenClassEnum::GREATER_OR_EQUAL ||
        current_token_->get_token_class() == TokenClassEnum::NOT_EQUAL)
      {
        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (!LexycalErrorOccurred()) {
          return ArithmeticExpression();
        }
      }
    }
    else {
      ReportSyntaxError("Esperado um operador booleano (==, !=, >, >=, <, ,=)");
    }
  }
  return false;
}

bool Parser::ArithmeticExpression()
{
  if (Term()) {
    //tokens_.push_back(current_token_);
    //current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      return ArithmeticExpressionAlt();
    }
  }
  return false;
}

bool Parser::ArithmeticExpressionAlt()
{
  if (current_token_->get_token_class() == TokenClassEnum::PLUS ||
    current_token_->get_token_class() == TokenClassEnum::MINUS)
  {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      if (Term()) {
        //tokens_.push_back(current_token_);
        //current_token_ = scanner.GetNextToken();

        if (!LexycalErrorOccurred()) {
          return ArithmeticExpressionAlt();
        }
      }
    }
  }
  else {
    return true;
  }
  return false;
}

bool Parser::Term()
{
  if (Factor()) {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      return TermAlt();
    }
  }
  return false;
}

bool Parser::TermAlt()
{
  if (current_token_->get_token_class() == TokenClassEnum::MULTIPLICATION ||
    current_token_->get_token_class() == TokenClassEnum::DIVISION)
  {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      if (Factor()) {
        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (!LexycalErrorOccurred()) {
          return TermAlt();
        }
      }
    }
  }
  else {
    return true;
  }
  return false;
}

bool Parser::Factor()
{
  if (current_token_->get_token_class() == TokenClassEnum::L_PAREN) {

    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      if (ArithmeticExpression()) {
        //tokens_.push_back(current_token_);
        //current_token_ = scanner.GetNextToken();
        if (!LexycalErrorOccurred()) {
          if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
            return true;
          }
          else {
            ReportSyntaxError("Esperado o simbolo ')'");
          }
        }
      }
    }
  }
  else if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER ||
    current_token_->get_token_class() == TokenClassEnum::INTEGER ||
    current_token_->get_token_class() == TokenClassEnum::FLOAT ||
    current_token_->get_token_class() == TokenClassEnum::CHAR)
  {
    return true;
  }
  return false;
}

bool Parser::IsInFirst(TokenPtr token, Production production)
{
  //<comando> ::= <comando_básico> | <iteração> | if "("<expr_relacional>")" <comando> {else <comando>}?
  if (production == Production::COMMAND) {
    if (token->get_token_class() == TokenClassEnum::IDENTIFIER) {
      return true;
    }
    else if (token->get_token_class() == TokenClassEnum::L_BRACE) {
      return true;
    }
    else if (token->get_token_class() == TokenClassEnum::RESERVED_WORD) {
      if (token->get_lexeme() == "while" || token->get_lexeme() == "do" || token->get_lexeme() == "if") {
        return true;
      }
    }
  }
  else if (production == Production::VARIABLE_DECLARATION) {
    if (token->get_token_class() == TokenClassEnum::RESERVED_WORD) {
      if (token->get_lexeme() == "int" || token->get_lexeme() == "float" || token->get_lexeme() == "char") {
        return true;
      }
    }
  }
  return false;
}

inline void Parser::IncrementScope()
{
  current_scope_++;
}

inline void Parser::DecrementScope()
{
  symbol_table_.remove_if([&](Symbol s){ return s.scope == current_scope_; });
  current_scope_--;
}

inline void Parser::PushSymbolToTable(DeclarationType current_declaration_type)
{
  Symbol symbol;

  symbol.scope = current_scope_;
  symbol.type = current_declaration_type;
  symbol.name = current_token_->get_lexeme();

  symbol_table_.push_front(symbol);
}

bool Parser::SymbolExistsOnTable() {
  auto tmp_current_token = current_token_;
  int tmp_current_scope = current_scope_;

  auto foundItem = std::find_if(symbol_table_.begin(), symbol_table_.end(),
    [tmp_current_token, tmp_current_scope](Symbol const& item)
  {
    return (item.scope == tmp_current_scope && item.name == tmp_current_token->get_lexeme());
  });;

  return foundItem != symbol_table_.end();
}