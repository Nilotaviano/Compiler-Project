#include <iostream>
#include <memory>
#include <fstream>

#include "Parser.h"

using std::cout;

int main(int argc, char* argv[])
{
  if (argc >= 2) {
    char *in_file_name = argv[1];
    FILE *in_fp = fopen(in_file_name, "r");
    char *out_file_name = argc > 2 ? argv[2] : nullptr;

    if (in_fp != nullptr) {
      Parser parser(in_fp, out_file_name);

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