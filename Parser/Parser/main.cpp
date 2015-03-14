#include <stdio.h>
#include <iostream>
#include "Scanner.h"
#include "Token.h"
#include <memory>


int main(int argc, char* argv[])
{
  typedef std::shared_ptr<Token> TokenPtr;
  FILE *fp = fopen("teste.c", "r");

  if (fp != NULL) {
    Scanner scanner(fp);

    while (!scanner.get_end_of_file_()) {
      TokenPtr token = scanner.GetNextToken();
      if (token != nullptr) {
        cout << token->getTokenClassString() << ": " << token->get_lexeme() << "\n";
      }
      else {
        cout << scanner.GetError() << "\n";
      }
    }
  }
  else {
    printf("Arquivo não encontrado");
  }

  printf("Pressione enter para continuar..");
  getchar();
  return 0;
}