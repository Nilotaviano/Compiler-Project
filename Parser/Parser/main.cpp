#include <iostream>
#include <memory>

#include "Parser.h"

using std::cout;

int main(int argc, char* argv[])
{
  if (argc == 2) {
    char *file_name = argv[1];
    FILE *fp = fopen(file_name, "r");
    
    if (fp != nullptr) {
      Parser parser(fp);

      parser.Begin();
    }
    else {
      cout << "Falha na abertura do arquivo.\n";
    }
  }
  else {
    cout << "Numero de argumentos invalido: Esperado 1 argumento, " << (argc - 1) << " argumento(s) recebido(s).\n";
  }

  cout << "\nPressione enter para continuar..";
  getchar();
  return 0;
}