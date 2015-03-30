#include <iostream>
#include <memory>

#include "Parser.h"

int main(int argc, char* argv[])
{
  Parser parser("teste.c");

  parser.Begin();

  cout << "Pressione enter para continuar..";
  getchar();
  return 0;
}