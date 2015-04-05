#include "Parser.h"

#include <stdio.h>
#include <iostream>

using std::cout;
using std::to_string;


Parser::Parser(FILE* fp)
:scanner(fp)
{
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
  if (Type()) {
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER) {
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

bool Parser::Type()
{
  if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD) {
    if (current_token_->get_lexeme() == "int" || current_token_->get_lexeme() == "float" || current_token_->get_lexeme() == "char") {
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
  else {
    ReportSyntaxError("Esperado ao menos um comando");
  }

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
    tokens_.push_back(current_token_);
    current_token_ = scanner.GetNextToken();

    if (!LexycalErrorOccurred()) {
      if (current_token_->get_token_class() == TokenClassEnum::ASSIGNMENT) {
        tokens_.push_back(current_token_);
        current_token_ = scanner.GetNextToken();

        if (!LexycalErrorOccurred()) {
          if (ArithmeticExpression()) {

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
  return false;
}

bool Parser::RelationalExpression()
{
  if (ArithmeticExpression()) {

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
    else {
      ReportSyntaxError("Esperado um operador booleano (==, !=, >, >=, <,=)");
    }
  }
  return false;
}

bool Parser::ArithmeticExpression()
{
  if (Term()) {

    return ArithmeticExpressionAlt();
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

        return ArithmeticExpressionAlt();
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

          if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
            return true;
          }
          else {
            ReportSyntaxError("Esperado o simbolo ')'");
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
  else {
    ReportSyntaxError("Esperado um identificador, literal numerico, caractere ou o simbolo '('.");
  }
  return false;
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