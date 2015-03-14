#include "Token.h"


Token::Token(string lexeme, TokenClassEnum token_class)
:lexeme_(lexeme), token_class_(token_class)
{
  token_class_string_map[IDENTIFIER] = "IDENTIFIER";
  token_class_string_map[GREATER] = "GREATER";
  token_class_string_map[LESS] = "LESS";
  token_class_string_map[GREATER_OR_EQUAL] = "GREATER_OR_EQUAL";
  token_class_string_map[LESS_OR_EQUAL] = "LESS_OR_EQUAL";
  token_class_string_map[EQUALS] = "EQUALS";
  token_class_string_map[NOT_EQUAL] = "NOT_EQUAL";
  token_class_string_map[PLUS] = "PLUS";
  token_class_string_map[MINUS] = "MINUS";
  token_class_string_map[MULTIPLICATION] = "MULTIPLICATION";
  token_class_string_map[DIVISION] = "DIVISION";
  token_class_string_map[ASSIGNMENT] = "ASSIGNMENT";
  token_class_string_map[L_PAREN] = "L_PAREN";
  token_class_string_map[R_PAREN] = "R_PAREN";
  token_class_string_map[L_BRACE] = "L_BRACE";
  token_class_string_map[R_BRACE] = "R_BRACE";
  token_class_string_map[COMMA] = "COMMA";
  token_class_string_map[SEMICOLON] = "SEMICOLON";
  token_class_string_map[RESERVED_WORD] = "RESERVED_WORD";
  token_class_string_map[INTEGER] = "INTEGER";
  token_class_string_map[FLOAT] = "FLOAT";
  token_class_string_map[CHAR] = "CHAR";
}


Token::~Token()
{
}
