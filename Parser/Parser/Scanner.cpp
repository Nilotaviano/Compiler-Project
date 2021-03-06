﻿#include "Scanner.h"

using std::to_string;

Scanner::Scanner(FILE* fp)
:file_pointer_(fp),
returned_token_(nullptr)
{
  num_lines_ = 1;
  num_columns_ = 1;
  current_char_ = fgetc(file_pointer_);
  end_of_file_ = false;
  string reserved_words[9] = { "main", "if", "else", "while", "do", "for", "int", "float", "char" };

  for (int i = 0; i < 9; i++) {
    reserved_words_[i] = reserved_words[i];
  }
}

Scanner::~Scanner()
{
  fclose(file_pointer_);
}

//If nullptr is returned, use GetError()
TokenPtr Scanner::GetNextToken()
{
  if (returned_token_ != nullptr) {
    TokenPtr aux = returned_token_;
    returned_token_ = nullptr;
    return aux;
  }

  string current_lexeme = "";
  while (IsWhiteSpace(current_char_)) {
    HandleWhiteSpace();
  }

  if (isdigit(current_char_)) {
    return HandleNumber(current_lexeme);
  }
  else if (current_char_ == '.') {
    return HandleFloat(current_lexeme);
  }
  else if (current_char_ == '\'') {
    return HandleChar(current_lexeme);
  }
  else if (IsArithmeticOp(current_char_)) {
    return HandleArithmeticOp(current_lexeme);
  }
  else if (IsRelationalOp(current_char_)) {
    return HandleRelationalOp(current_lexeme);
  }
  else if (isalpha(current_char_) || current_char_ == '_') {
    return HandleIdentifier(current_lexeme);
  }
  else if (IsSpecialCharacter(current_char_)) {
    return HandleSpecial(current_lexeme);
  }
  else if (current_char_ == EOF) {
    end_of_file_ = true;
    error_msg_ = "End of file";
    return nullptr;
  }
  else {
    num_columns_++;
    error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + current_char_ + "': Caractere inválido";
    current_char_ = fgetc(file_pointer_);
    return nullptr;
  }

}

void Scanner::HandleWhiteSpace()
{
  switch (current_char_) {
  case ' ':
    ++num_columns_;
    break;
  case '\t':
    num_columns_ += 4;
    break;
  case '\n':
    num_columns_ = 0;
    ++num_lines_;
    break;
  }
  current_char_ = fgetc(file_pointer_);
}

TokenPtr Scanner::HandleNumber(string current_lexeme)
{
  current_lexeme += current_char_;
  current_char_ = fgetc(file_pointer_);
  num_columns_++;

  while (isdigit(current_char_)) {
    current_lexeme += current_char_;
    current_char_ = fgetc(file_pointer_);
    num_columns_++;
  }

  if (current_char_ == '.') {
    return HandleFloat(current_lexeme);
  }
  else {
    return TokenPtr(new Token(current_lexeme,TokenClassEnum::INTEGER));
  }
}

TokenPtr Scanner::HandleFloat(string current_lexeme)
{
  char last_char = current_char_;
  current_lexeme += current_char_;
  current_char_ = fgetc(file_pointer_);
  num_columns_++;

  if (isdigit(current_char_)) {
    current_lexeme += current_char_;
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    while (isdigit(current_char_)) {
      current_lexeme += current_char_;
      current_char_ = fgetc(file_pointer_);
      num_columns_++;
    }

    TokenPtr token = TokenPtr(new Token(current_lexeme,TokenClassEnum::FLOAT));
    return token;
  }
  else {
    error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + last_char + "': Float mal formado";
    return nullptr;
  }
}

TokenPtr Scanner::HandleChar(string current_lexeme)
{
  char last_char = current_char_;
  current_lexeme += current_char_;
  current_char_ = fgetc(file_pointer_);
  num_columns_++;

  if (current_char_ != '\'') {

    if (current_char_ == EOF) {
      end_of_file_ = true;
      error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + last_char + "': Fim de arquivo dentro de caractere";
      return nullptr;
    }
    if (current_char_ == '\n') {
      current_char_ = fgetc(file_pointer_);
      error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + last_char + "': Char mal formado";
      num_columns_ = 0;
      num_lines_++;
      return nullptr;
    }

    current_lexeme += current_char_;
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    if (current_char_ == '\'') {
      current_lexeme += current_char_;
      current_char_ = fgetc(file_pointer_);
      num_columns_++;

      TokenPtr token = TokenPtr(new Token(current_lexeme,TokenClassEnum::CHAR));
      return token;
    }
    else {
      while (current_char_ != '\'') {
        if (current_char_ == '\n') {
          current_char_ = fgetc(file_pointer_);
          error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + last_char + "': Char mal formado";
          num_columns_ = 0;
          num_lines_++;
          return nullptr;
        }
        else if (current_char_ == EOF) {
          end_of_file_ = true;
          error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + last_char + "': Fim de arquivo dentro de caractere";
          return nullptr;
        }
        num_columns_++;
        last_char = current_char_;
        current_lexeme += current_char_;
        current_char_ = fgetc(file_pointer_);
      }
      num_columns_++;
      last_char = current_char_;
      current_lexeme += current_char_;
      current_char_ = fgetc(file_pointer_);

      error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + last_char + "': Char mal formado";
      return nullptr;
    }
  }
  else {
    last_char = current_char_;
    current_lexeme += current_char_;
    current_char_ = fgetc(file_pointer_);
    num_columns_++;
    error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + last_char + "': Char mal formado, esperado ao menos um caractere";
    return nullptr;
  }
}

TokenPtr Scanner::HandleArithmeticOp(string current_lexeme)
{
  TokenPtr token = nullptr;
  current_lexeme += current_char_;

  switch (current_char_) {
  case '+':
    token = TokenPtr(new Token(current_lexeme,TokenClassEnum::PLUS));
    break;
  case '-':
    token = TokenPtr(new Token(current_lexeme,TokenClassEnum::MINUS));
    break;
  case '*':
    token = TokenPtr(new Token(current_lexeme,TokenClassEnum::MULTIPLICATION));
    break;
  case '/':
    //Gotta check for line and block comments
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    if (current_char_ == '*') {
      return HandleBlockComment();
    }
    else if (current_char_ == '/') {
      return HandleLineComment();
    }
    else {
      return TokenPtr(new Token(current_lexeme,TokenClassEnum::DIVISION));
    }
    break;
  case '=':
    //Gotta check for "=="
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    if (current_char_ == '=') {
      return HandleRelationalOp(current_lexeme);
    }
    else {
      return TokenPtr(new Token(current_lexeme,TokenClassEnum::ASSIGNMENT));
    }
  }

  current_char_ = fgetc(file_pointer_);
  num_columns_++;

  return token;
}

TokenPtr Scanner::HandleRelationalOp(string current_lexeme)
{
  if (current_char_ == '!') {
    char last_char = current_char_;
    current_lexeme += current_char_;
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    if (current_char_ == '=') {
      current_lexeme += current_char_;
      current_char_ = fgetc(file_pointer_);
      num_columns_++;

      return TokenPtr(new Token(current_lexeme,TokenClassEnum::NOT_EQUAL));
    }
    else {
      error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + last_char + "': Exclamação não seguida de '='";
      return nullptr;
    }
  }
  else if (current_char_ == '=') {
    current_lexeme += current_char_;  //It's already guaranteed to be followed by '='
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    return TokenPtr(new Token(current_lexeme,TokenClassEnum::EQUALS));
  }
  else {
    current_lexeme += current_char_;
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    if (current_char_ == '=') {
      current_lexeme += current_char_;
      current_char_ = fgetc(file_pointer_);
      num_columns_++;
    }

    if (current_lexeme == ">") {
      return TokenPtr(new Token(current_lexeme,TokenClassEnum::GREATER));
    }
    else if (current_lexeme == "<") {
      return TokenPtr(new Token(current_lexeme,TokenClassEnum::LESS));
    }
    else if (current_lexeme == ">=") {
      return TokenPtr(new Token(current_lexeme,TokenClassEnum::GREATER_OR_EQUAL));
    }
    else if (current_lexeme == "<=") {
      return TokenPtr(new Token(current_lexeme,TokenClassEnum::LESS_OR_EQUAL));
    }
  }
}

TokenPtr Scanner::HandleIdentifier(string current_lexeme)
{
  current_lexeme += current_char_;
  current_char_ = fgetc(file_pointer_);
  num_columns_++;

  while (isalpha(current_char_) || isdigit(current_char_) || current_char_ == '_') {
    current_lexeme += current_char_;
    current_char_ = fgetc(file_pointer_);
    num_columns_++;
  }

  if (IsReservedWord(current_lexeme)) {
    return HandleReservedWord(current_lexeme);
  }
  else {
    return TokenPtr(new Token(current_lexeme,TokenClassEnum::IDENTIFIER));
  }
}

TokenPtr Scanner::HandleReservedWord(string current_lexeme) {
  return TokenPtr(new Token(current_lexeme,TokenClassEnum::RESERVED_WORD));
}

TokenPtr Scanner::HandleSpecial(string current_lexeme)
{
  TokenPtr token = nullptr;

  current_lexeme += current_char_;

  switch (current_char_) {
  case '(':
    token = TokenPtr(new Token(current_lexeme,TokenClassEnum::L_PAREN));
    break;
  case ')':
    token = TokenPtr(new Token(current_lexeme,TokenClassEnum::R_PAREN));
    break;
  case '{':
    token = TokenPtr(new Token(current_lexeme,TokenClassEnum::L_BRACE));
    break;
  case '}':
    token = TokenPtr(new Token(current_lexeme,TokenClassEnum::R_BRACE));
    break;
  case',':
    token = TokenPtr(new Token(current_lexeme,TokenClassEnum::COMMA));
    break;
  case ';':
    token = TokenPtr(new Token(current_lexeme,TokenClassEnum::SEMICOLON));
    break;
  }

  current_char_ = fgetc(file_pointer_);
  num_columns_++;

  return token;
}

TokenPtr Scanner::HandleLineComment() {
  current_char_ = fgetc(file_pointer_);
  num_columns_++;

  while (current_char_ != '\n' && current_char_ != EOF) {
    current_char_ = fgetc(file_pointer_);
    num_columns_++;
  }

  if (current_char_ == EOF) {
    end_of_file_ = true;
    error_msg_ = "End of file";
    return nullptr;
  }
  else {
    num_lines_++;
    num_columns_ = 0;
    current_char_ = fgetc(file_pointer_);
    return GetNextToken();
  }
}

TokenPtr Scanner::HandleBlockComment() {
  char last_char = current_char_;
  while ((current_char_ = fgetc(file_pointer_)) != EOF) {
    num_columns_++;
    last_char = current_char_;

    if (current_char_ == '*') {
      current_char_ = fgetc(file_pointer_);
      num_columns_++;

      if (current_char_ == '/') {
        current_char_ = fgetc(file_pointer_);
        num_columns_++;

        return GetNextToken();
      }
      else {
        //Have to write the current_char_ back to stream
        //Or else "/***/" would cause this loop to consume the whole file
        ungetc(current_char_, file_pointer_);
      }
    }
    else if (current_char_ == '\n') {
      num_columns_ = 0;
      num_lines_++;
    }
  }

  if (current_char_ == EOF) {
    end_of_file_ = true;
    error_msg_ = "Erro na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo caractere lido '" + last_char + "': Fim de arquivo dentro de comentario em bloco";
    return nullptr;
  }
  else {
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    return GetNextToken();
  }
}