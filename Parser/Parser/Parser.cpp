#include "Parser.h"

#include <stdio.h>
#include <iostream>
#include <algorithm>

using std::cout;
using std::to_string;

Parser::Parser(FILE* fp)
:scanner(fp),
current_scope_(0)
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
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (current_token_ == nullptr) {
      cout << "Ok!\n";
    }
    else {
      ReportSyntaxError("Não pode haver código fora da função 'main'");
    }
  }
  else {
    cout << "Erro(s) encontrado(s)!\n";
  }
}

void Parser::ReportSemanticalError(string error)
{
  cout << "ERRO de semântica na linha " + to_string(scanner.GetCurrentLine()) + " coluna " + to_string(scanner.GetCurrentColumn()) + ", ultimo token lido '" + current_token_->ToString() + "': " + error + "\n";
}

void Parser::ReportSyntaxError(string error)
{
  if (current_token_ != nullptr) {
    cout << "ERRO de sintaxe na linha " + to_string(scanner.GetCurrentLine()) + " coluna " + to_string(scanner.GetCurrentColumn()) + ", ultimo token lido '" + current_token_->ToString() + "': " + error + "\n";
  }
  else {
    cout << "ERRO de sintaxe na linha " + to_string(scanner.GetCurrentLine()) + " coluna " + to_string(scanner.GetCurrentColumn()) + ", ultimo token lido '" + tokens_.back()->ToString() + "': " + error + "\n";
  }
}

bool Parser::LexycalErrorOccurred()
{
  if (current_token_ == nullptr) {
    if (!scanner.IsEndOfFile()) {
      cout << scanner.GetError() << "\n";
    }
    else {
      ReportSyntaxError("Fim inesperado de arquivo");
    }
    return true;

  }
  else {
    return false;
  }
}

void Parser::PrintCode(string code_str)
{
  cout << code_str << '\n';
}

bool Parser::Program()
{
  current_token_ = scanner.GetNextToken();

  if (!LexycalErrorOccurred())  {
    if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD && current_token_->get_lexeme() == "int") {
      tokens_.push_back(current_token_);
      current_token_ = scanner.GetNextToken();

      if (!LexycalErrorOccurred())  {
        if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD && current_token_->get_lexeme() == "main") {
          tokens_.push_back(current_token_);
          current_token_ = scanner.GetNextToken();

          if (!LexycalErrorOccurred())  {
            if (current_token_->get_token_class() == TokenClassEnum::L_PAREN) {
              tokens_.push_back(current_token_);
              current_token_ = scanner.GetNextToken();

              if (!LexycalErrorOccurred())  {
                if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
                  tokens_.push_back(current_token_);
                  current_token_ = scanner.GetNextToken();

                  if (!LexycalErrorOccurred())  {
                    if (Block()) {
                      return true;
                    }
                  }
                }
                else {
                  ReportSyntaxError("Esperado o simbolo '('");
                }
              }
            }
            else {
              ReportSyntaxError("Esperado o simbolo '('");
            }
          }
        }
        else {
          ReportSyntaxError("Esperada a palavra-chave 'main'");
        }
      }
    }
    else {
      ReportSyntaxError("Esperada a palavra-chave 'int'");
    }
  }

  return false;
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

bool Parser::VariableDeclaration()
{
  DeclarationType current_declaration_type;

  if (Type(&current_declaration_type)) {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();



    if (!LexycalErrorOccurred()) {
      if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER) {

        if (WasVariableDeclared()) {
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

              if (current_token_->get_token_class() != TokenClassEnum::IDENTIFIER) {
                ReportSyntaxError("Esperado um idenficador");
                return false;
              }
              else {

                if (WasVariableDeclared()) {
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
          }

          if (current_token_->get_token_class() == TokenClassEnum::SEMICOLON) {
            return true;
          }
          else {
            ReportSyntaxError("Esperado o simbolo ';'");
          }
        }
      }
      else {
        ReportSyntaxError("Esperado ao menos um identificador");
      }
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
                      }
                      else {
                        scanner.UngetToken(current_token_);

                        current_token_ = tokens_.back();
                        tokens_.pop_back();
                        return true;
                      }
                    }
                  }
                  else {
                    return false;
                  }
                }
              }
              else {
                ReportSyntaxError("Esperado o simbolo ')'");
              }
            }
          }
        }
        else {
          ReportSyntaxError("Esperado o simbolo '('");
        }
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
      else {
        ReportSyntaxError("Esperado o simbolo '('");
      }
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

                    if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
                      tokens_.push_back(current_token_);
                      current_token_ = scanner.GetNextToken();

                      if (!LexycalErrorOccurred()) {
                        if (current_token_->get_token_class() == TokenClassEnum::SEMICOLON) {
                          return true;
                        }
                        else {
                          ReportSyntaxError("Esperado o simbolo ';' no final de uma expressão 'do' 'while'");
                        }
                      }
                    }
                    else {
                      ReportSyntaxError("Esperado o simbolo ')'");
                    }
                  }
                }
              }
              else {
                ReportSyntaxError("Esperado o simbolo '('");
              }
            }
          }
          else {
            ReportSyntaxError("Esperada a palavra reservada 'while'");
          }
        }
      }
    }
  }
  return false;
}

bool Parser::Assignment()
{
  if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER) {
    DeclarationType var_type = GetVarType(current_token_->get_lexeme());

    if (var_type == DeclarationType::NONE) {
      ReportSemanticalError("Variável não declarada");
    }
    else {
      tokens_.push_back(current_token_);
      current_token_ = scanner.GetNextToken();

      if (!LexycalErrorOccurred()) {
        if (current_token_->get_token_class() == TokenClassEnum::ASSIGNMENT) {
          tokens_.push_back(current_token_);
          current_token_ = scanner.GetNextToken();

          if (!LexycalErrorOccurred()) {

            DeclarationType expr_type = DeclarationType::NONE;
            if (ArithmeticExpression(&expr_type)) {

              if (var_type != expr_type) {
                if (var_type != DeclarationType::FLOAT || expr_type != DeclarationType::INTEGER) {
                  ReportSemanticalError("Tipo da expressão incompatível com variável sendo atribuída.");
                  return false;
                }
              }

              if (current_token_->get_token_class() == TokenClassEnum::SEMICOLON) {
                return true;
              }
              else {
                ReportSyntaxError("Esperado o simbolo ';'");
              }
            }
          }
        }
        else {
          ReportSyntaxError("Esperado o simbolo '=' para atribuição");
        }
      }
    }
  }
  return false;
}

bool Parser::RelationalExpression()
{
  DeclarationType l_expr_type = DeclarationType::NONE;
  if (ArithmeticExpression(&l_expr_type)) {

    if (current_token_->get_token_class() == TokenClassEnum::EQUALS ||
      current_token_->get_token_class() == TokenClassEnum::LESS ||
      current_token_->get_token_class() == TokenClassEnum::LESS_OR_EQUAL ||
      current_token_->get_token_class() == TokenClassEnum::GREATER ||
      current_token_->get_token_class() == TokenClassEnum::GREATER_OR_EQUAL ||
      current_token_->get_token_class() == TokenClassEnum::NOT_EQUAL)
    {
      tokens_.push_back(current_token_);
      current_token_ = scanner.GetNextToken();

      DeclarationType r_expr_type = DeclarationType::NONE;
      if (!LexycalErrorOccurred()) {
        if (ArithmeticExpression(&r_expr_type)) {
          if (IsCompatible(l_expr_type, r_expr_type)) {
            return true;
          }
          else {
            ReportSemanticalError("Tipos incompatíveis para comparação.");
          }
        }
      }
    }
    else {
      ReportSyntaxError("Esperado um operador booleano (==, !=, >, >=, <,=)");
    }
  }
  return false;
}

bool Parser::ArithmeticExpression(DeclarationType *my_type)
{
  DeclarationType l_expr_type;

  if (Term(&l_expr_type)) {
    DeclarationType r_expr_type = DeclarationType::NONE;

    if (ArithmeticExpressionAlt(&r_expr_type)) {
      if (r_expr_type == DeclarationType::NONE) {
        *my_type = l_expr_type;
        return true;
      }
      else if (IsCompatible(l_expr_type, r_expr_type)) {
        *my_type = GetHigherType(l_expr_type, r_expr_type);
        return true;
      }
      else {
        ReportSemanticalError("Operandos de tipos incompativeis");
      }
    }
  }
  return false;
}

bool Parser::ArithmeticExpressionAlt(DeclarationType *my_type)
{
  if (current_token_->get_token_class() == TokenClassEnum::PLUS ||
    current_token_->get_token_class() == TokenClassEnum::MINUS)
  {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      DeclarationType l_expr_type = DeclarationType::NONE;

      if (Term(&l_expr_type)) {
        DeclarationType r_expr_type = DeclarationType::NONE;

        if (ArithmeticExpressionAlt(&r_expr_type)) {
          if (r_expr_type == DeclarationType::NONE) {
            *my_type = l_expr_type;
            return true;
          }
          else if (IsCompatible(l_expr_type, r_expr_type)) {
            *my_type = GetHigherType(l_expr_type, r_expr_type);
            return true;
          }
          else {
            ReportSemanticalError("Operandos de tipos incompativeis");
          }
        }
      }
    }
  }
  else {
    return true;
  }

  return false;
}

bool Parser::Term(DeclarationType *my_type)
{
  DeclarationType l_expr_type;

  if (Factor(&l_expr_type)) {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      DeclarationType r_expr_type = DeclarationType::NONE;

      if (TermAlt(&r_expr_type)) {
        if (r_expr_type == DeclarationType::NONE) {
          *my_type = l_expr_type;
          return true;
        }
        else if (IsCompatible(l_expr_type, r_expr_type)) {
          *my_type = GetHigherType(l_expr_type, r_expr_type);
          return true;
        }
        else {
          ReportSemanticalError("Operandos de tipos incompativeis");
        }
      }
    }
  }

  return false;
}

bool Parser::TermAlt(DeclarationType *my_type)
{
  if (current_token_->get_token_class() == TokenClassEnum::MULTIPLICATION ||
    current_token_->get_token_class() == TokenClassEnum::DIVISION)
  {
    bool is_division = false;

    if (current_token_->get_token_class() == TokenClassEnum::DIVISION) {
      is_division = true;
    }

    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      DeclarationType l_expr_type;

      if (Factor(&l_expr_type)) {
        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (!LexycalErrorOccurred()) {
          DeclarationType r_expr_type = DeclarationType::NONE;

          if (TermAlt(&r_expr_type)) {
            if (r_expr_type == DeclarationType::NONE) {
              if (is_division && l_expr_type == DeclarationType::INTEGER) {
                *my_type = DeclarationType::FLOAT;
              }
              else {
                *my_type = l_expr_type;
              }

              return true;
            }
            else if (IsCompatible(l_expr_type, r_expr_type)) {
              if (is_division && l_expr_type == DeclarationType::INTEGER) {
                *my_type = DeclarationType::FLOAT;
              }
              else {
                *my_type = GetHigherType(l_expr_type, r_expr_type);
              }

              return true;
            }
            else {
              ReportSemanticalError("Operandos de tipos incompativeis");
            }
          }
        }
      }
    }
  }
  else {
    return true;
  }

  return false;
}

bool Parser::Factor(DeclarationType *my_type)
{
  bool result;

  if (current_token_->get_token_class() == TokenClassEnum::L_PAREN) {

    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      DeclarationType expr_type;

      if (ArithmeticExpression(&expr_type)) {

        if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
          *my_type = expr_type;
          result = true;
        }
        else {
          ReportSyntaxError("Esperado o simbolo ')'");
          result = false;
        }
      }
    }
  }
  else if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER) {
    *my_type = GetVarType(current_token_->get_lexeme());

    if (*my_type != DeclarationType::NONE) {
      result = true;
    }
    else {
      ReportSemanticalError("Variável não declarada.");
      result = false;
    }
  }
  else if (current_token_->get_token_class() == TokenClassEnum::INTEGER) {
    *my_type = DeclarationType::INTEGER;
    result = true;
  }
  else if (current_token_->get_token_class() == TokenClassEnum::FLOAT) {
    *my_type = DeclarationType::FLOAT;
    result = true;
  }
  else if(current_token_->get_token_class() == TokenClassEnum::CHAR) {
    *my_type = DeclarationType::CHAR;
    result = true;
  }
  else {
    ReportSyntaxError("Esperado um identificador, literal numerico, caractere ou o simbolo '('.");
    result = false;
  }

  return result;
}

bool Parser::IsInFirst(TokenPtr token, Production production)
{
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

bool Parser::WasVariableDeclared()
{

  auto found_item = std::find_if(symbol_table_.begin(), symbol_table_.end(),
    [&](Symbol const& item)
  {
    return (item.scope == current_scope_ && item.name == current_token_->get_lexeme());
  });;

  return found_item != symbol_table_.end();
}

Parser::DeclarationType Parser::GetVarType(string var_name)
{
  auto found_item = std::find_if(symbol_table_.begin(), symbol_table_.end(),
    [var_name](Symbol const& item)
  {
    return item.name == var_name;
  });;

  return found_item != symbol_table_.end() ? found_item->type : DeclarationType::NONE;
}

bool Parser::IsCompatible(DeclarationType l_type, DeclarationType r_type)
{
  if (l_type != DeclarationType::NONE && r_type != DeclarationType::NONE) {
    return (l_type == r_type ||
      (l_type == DeclarationType::FLOAT && r_type == DeclarationType::INTEGER) ||
      (l_type == DeclarationType::INTEGER&& r_type == DeclarationType::FLOAT));
  }
  else {
    cout << "IsCompatible: TIPOS DESCONHECIDOS";
    return false;
  }
}

Parser::DeclarationType Parser::GetHigherType(DeclarationType l_type, DeclarationType r_type)
{
  DeclarationType type = DeclarationType::NONE;
  if (l_type == r_type) {
    type = l_type;
  }
  else if (l_type == DeclarationType::FLOAT && r_type == DeclarationType::INTEGER) {
    type = DeclarationType::FLOAT;
  }
  else if (l_type == DeclarationType::INTEGER && r_type == DeclarationType::FLOAT) {
    type = DeclarationType::FLOAT;
  }

  return type;
}
