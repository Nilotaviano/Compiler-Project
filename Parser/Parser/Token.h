#pragma once

#include <string>
#include <map>

using std::map;
using std::string;

enum class TokenClassEnum
{
  //TODO
  IDENTIFIER,           //[a-zA-Z][a-zA-Z0-9|_]*
  GREATER,              //>
  LESS,                 //<
  GREATER_OR_EQUAL,     //>=
  LESS_OR_EQUAL,        //<=
  EQUALS,                //==
  NOT_EQUAL,            //!=
  PLUS,
  MINUS,
  MULTIPLICATION,
  DIVISION,
  ASSIGNMENT,
  L_PAREN,
  R_PAREN,
  L_BRACE,
  R_BRACE,
  COMMA,
  SEMICOLON,
  RESERVED_WORD,        //[(main) | (if) | (else) | (while) | (do) | (for) | (int) | (float) | (char)]
  INTEGER,              //[0-9]+
  FLOAT,                //[0-9]*.[0-9]+
  CHAR                  //'[a-zA-Z]'
};

//Will simply hold the lexeme and the class of a token
class Token
{
public:
	Token(string lexeme, TokenClassEnum token_class);
	~Token();
  string get_lexeme() { return lexeme_; }
  TokenClassEnum get_token_class() { return token_class_; }
  string GetTokenClassString() { return token_class_string_map[token_class_]; }
  string ToString() { return token_class_string_map[token_class_] + ": " + lexeme_; }
private:
	string lexeme_;
	TokenClassEnum token_class_;
  map<TokenClassEnum, string> token_class_string_map;
};