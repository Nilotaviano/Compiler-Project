#include "Scanner.h"

typedef std::shared_ptr<Token> TokenPtr;

Scanner::Scanner(FILE* fp)
:file_pointer_(fp)
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

//Will return the next token read from file
//If nullptr is returned, use GetError()
TokenPtr Scanner::GetNextToken()
{
  string current_lexeme = "";
  while (isWhiteSpace(current_char_)) {
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
  else if (isArithmeticOp(current_char_)) {
    return HandleArithmeticOp(current_lexeme);
  }
  else if (isRelationalOp(current_char_)) {
    return HandleRelationalOp(current_lexeme);
  }
  else if (isalpha(current_char_) || current_char_ == '_') {
    return HandleIdentifier(current_lexeme);
  }
  else if (isSpecialCharacter(current_char_)) {
    return HandleSpecial(current_lexeme);
  }
  else if (current_char_ == EOF) {
    end_of_file_ = true;
    error_msg_ = "End of file";
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
    num_columns_ += 4;  //TAB counts as 4 columns
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
    return TokenPtr(new Token(current_lexeme, INTEGER));
  }
}

TokenPtr Scanner::HandleFloat(string current_lexeme)
{
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

    TokenPtr token = TokenPtr(new Token(current_lexeme, FLOAT));
    return token;
  }
  else {
    error_msg_ = "ERRO na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo token lido t: Float mal formado";
    return nullptr;
  }
}

TokenPtr Scanner::HandleChar(string current_lexeme)
{
  current_lexeme += current_char_;
  current_char_ = fgetc(file_pointer_);
  num_columns_++;

  if (current_char_ != '\'') {
    current_lexeme += current_char_;
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    if (current_char_ == '\'') {
      current_lexeme += current_char_;
      current_char_ = fgetc(file_pointer_);
      num_columns_++;

      TokenPtr token = TokenPtr(new Token(current_lexeme, CHAR));
      return token;
    }
    else {
      error_msg_ = "ERRO na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo token lido t: Char mal formado";
      return nullptr;
    }
  }
  else {
    error_msg_ = "ERRO na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo token lido t: Char mal formado, esperado ao menos um caractere";
    return nullptr;
  }
}

TokenPtr Scanner::HandleArithmeticOp(string current_lexeme)
{
  TokenPtr token = nullptr;
  current_lexeme += current_char_;

  switch (current_char_) {
  case '+':
    token = TokenPtr(new Token(current_lexeme, PLUS));
    break;
  case '-':
    token = TokenPtr(new Token(current_lexeme, MINUS));
    break;
  case '*':
    token = TokenPtr(new Token(current_lexeme, MULTIPLICATION));
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
      return TokenPtr(new Token(current_lexeme, DIVISION));
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
      return TokenPtr(new Token(current_lexeme, ASSIGNMENT));
    }
  }

  current_char_ = fgetc(file_pointer_);
  num_columns_++;

  return token;
}

TokenPtr Scanner::HandleRelationalOp(string current_lexeme)
{
  if (current_char_ == '!') {
    current_lexeme += current_char_;
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    if (current_char_ == '=') {
      current_lexeme += current_char_;
      current_char_ = fgetc(file_pointer_);
      num_columns_++;

      return TokenPtr(new Token(current_lexeme, NOT_EQUAL));
    }
    else {
      error_msg_ = "ERRO na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo token lido t: Exclamação não seguida de '='";
      return nullptr;
    }
  }
  else if (current_char_ == '=') {
    current_lexeme += current_char_;  //It's already guaranteed to be followed by '='
    current_char_ = fgetc(file_pointer_);
    num_columns_++;

    return TokenPtr(new Token(current_lexeme, EQUALS));
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
      return TokenPtr(new Token(current_lexeme, GREATER));
    }
    else if (current_lexeme == "<") {
      return TokenPtr(new Token(current_lexeme, LESS));
    }
    else if (current_lexeme == ">=") {
      return TokenPtr(new Token(current_lexeme, GREATER_OR_EQUAL));
    }
    else if (current_lexeme == "<=") {
      return TokenPtr(new Token(current_lexeme, LESS_OR_EQUAL));
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

  if (isReservedWord(current_lexeme)) {
    return HandleReservedWord(current_lexeme);
  }
  else {
    return TokenPtr(new Token(current_lexeme, IDENTIFIER));
  }
}

TokenPtr Scanner::HandleReservedWord(string current_lexeme) {
  return TokenPtr(new Token(current_lexeme, RESERVED_WORD));
}

TokenPtr Scanner::HandleSpecial(string current_lexeme)
{
  TokenPtr token = nullptr;

  current_lexeme += current_char_;

  switch (current_char_) {
  case '(':
    token = TokenPtr(new Token(current_lexeme, L_PAREN));
    break;
  case ')':
    token = TokenPtr(new Token(current_lexeme, R_PAREN));
    break;
  case '{':
    token = TokenPtr(new Token(current_lexeme, L_BRACE));
    break;
  case '}':
    token = TokenPtr(new Token(current_lexeme, R_BRACE));
    break;
  case',':
    token = TokenPtr(new Token(current_lexeme, COMMA));
    break;
  case ';':
    token = TokenPtr(new Token(current_lexeme, SEMICOLON));
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

  while ((current_char_ = fgetc(file_pointer_)) != EOF) {
    num_columns_++;

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
    error_msg_ = "ERRO na linha " + to_string(num_lines_) + " coluna " + to_string(num_columns_) + ", ultimo token lido t: Fim de arquivo dentro de comentario em bloco";
    return nullptr;
  }
  else {
    current_char_ == fgetc(file_pointer_);
    num_columns_++;

    return GetNextToken();
  }
}