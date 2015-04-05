#pragma once

#include <list>
#include <string>
#include <memory>

#include "Token.h"

typedef std::shared_ptr<Token> TokenPtr;
using std::string;

class Scanner
{
public:
	Scanner(FILE* fp);
	~Scanner();
  TokenPtr GetNextToken();
  int GetCurrentLine() { return num_lines_; }
  int GetCurrentColumn() { return num_columns_; }
  string GetError() { return error_msg_; }
  bool IsEndOfFile(){ return end_of_file_; }
  TokenPtr GetCurrentToken() { return current_token_; }
  void UngetToken(TokenPtr token) { returned_token_ = token; }  //This is necessary for the 'Command' production, without it an 'if' statement not followed by an 'else' would consume an extra token.

private:
	FILE* file_pointer_;
  char current_char_;
  TokenPtr current_token_;
  TokenPtr returned_token_; //Not using a list/stack because only one token will be returned at a time.
	int num_lines_;
	int num_columns_;
  string error_msg_;
  bool end_of_file_;
  string reserved_words_[9];

  void HandleWhiteSpace();
  bool inline IsWhiteSpace(char ch) { return (ch == ' ' || ch == '\t' || ch == '\n'); }
  bool inline IsArithmeticOp(char ch) { return (ch =='+' || ch =='-' || ch =='/' || ch =='*' || ch == '='); }
  bool inline IsSpecialCharacter(char ch) { return (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ',' || ch == ';'); }
  bool inline IsRelationalOp(char ch) { return (ch == '<' || ch == '>' || ch == '!'); } //== will be treated first by HandleArithmeticOp
  bool inline IsReservedWord(string current_lexeme) { for (string rw : reserved_words_) { if (rw == current_lexeme) { return true; } } return false; }
  TokenPtr HandleNumber(string current_lexeme);
  TokenPtr HandleFloat(string current_lexeme);
  TokenPtr HandleChar(string current_lexeme);
  TokenPtr HandleArithmeticOp(string current_lexeme);
  TokenPtr HandleRelationalOp(string current_lexeme);
  TokenPtr HandleIdentifier(string current_lexeme);
  TokenPtr HandleReservedWord(string current_lexeme);
  TokenPtr HandleSpecial(string current_lexeme);
  TokenPtr HandleLineComment();
  TokenPtr HandleBlockComment();
};