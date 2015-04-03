#include "Token.h"


Token::Token(string lexeme, TokenClassEnum token_class)
:lexeme_(lexeme), token_class_(token_class)
{
  token_class_string_map[TokenClassEnum::IDENTIFIER] = "IDENTIFIER";
  token_class_string_map[TokenClassEnum::GREATER] = "GREATER";
  token_class_string_map[TokenClassEnum::LESS] = "LESS";
  token_class_string_map[TokenClassEnum::GREATER_OR_EQUAL] = "GREATER_OR_EQUAL";
  token_class_string_map[TokenClassEnum::LESS_OR_EQUAL] = "LESS_OR_EQUAL";
  token_class_string_map[TokenClassEnum::EQUALS] = "EQUALS";
  token_class_string_map[TokenClassEnum::NOT_EQUAL] = "NOT_EQUAL";
  token_class_string_map[TokenClassEnum::PLUS] = "PLUS";
  token_class_string_map[TokenClassEnum::MINUS] = "MINUS";
  token_class_string_map[TokenClassEnum::MULTIPLICATION] = "MULTIPLICATION";
  token_class_string_map[TokenClassEnum::DIVISION] = "DIVISION";
  token_class_string_map[TokenClassEnum::ASSIGNMENT] = "ASSIGNMENT";
  token_class_string_map[TokenClassEnum::L_PAREN] = "L_PAREN";
  token_class_string_map[TokenClassEnum::R_PAREN] = "R_PAREN";
  token_class_string_map[TokenClassEnum::L_BRACE] = "L_BRACE";
  token_class_string_map[TokenClassEnum::R_BRACE] = "R_BRACE";
  token_class_string_map[TokenClassEnum::COMMA] = "COMMA";
  token_class_string_map[TokenClassEnum::SEMICOLON] = "SEMICOLON";
  token_class_string_map[TokenClassEnum::RESERVED_WORD] = "RESERVED_WORD";
  token_class_string_map[TokenClassEnum::INTEGER] = "INTEGER";
  token_class_string_map[TokenClassEnum::FLOAT] = "FLOAT";
  token_class_string_map[TokenClassEnum::CHAR] = "CHAR";
}


Token::~Token()
{
}
