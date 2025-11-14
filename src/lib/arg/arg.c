#include "arg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para lidar com argumentos no formato -opt valorDaOpcao
// argc e argv são os argumentos da função main
// opt_name é o nome da opção (sem o '-'), por exemplo: "opt"
// Retorna um ponteiro para o valor da opção ou NULL se não for encontrado
char *obter_valor_opcao(int argc, char *argv[], char *opt_name) {
  char formato_opt[64];
  snprintf(formato_opt, sizeof(formato_opt), "-%s", opt_name);

  for (int i = 1; i < argc - 1; ++i) {
    if (strcmp(argv[i], formato_opt) == 0) {
      if (argv[i + 1] == NULL)
        return NULL;
      return argv[i + 1];
    }
  }
  return NULL;
}

char *obter_sufixo_comando(int argc, char *argv[]) {
  // Duplica argv para trabalhar com uma cópia
  char **argv_copia = malloc(argc * sizeof(char *));
  if (argv_copia == NULL)
    return NULL;

  // Copia todos os argumentos
  for (int i = 0; i < argc; i++) {
    argv_copia[i] = argv[i];
  }

  int novo_argc = argc;

  // Remove todos os pares -opt valor
  for (int i = 1; i < novo_argc - 1; i++) {
    // Verifica se o argumento atual é uma opção (começa com '-')
    if (argv_copia[i][0] == '-') {
      // Verifica se o próximo argumento não é uma opção (valor dessa opção)
      if (argv_copia[i + 1][0] != '-') {
        // Remove o par deslocando os argumentos restantes
        for (int j = i; j < novo_argc - 2; j++) {
          argv_copia[j] = argv_copia[j + 2];
        }
        novo_argc -= 2;
        i--; // Reavalia essa posição
      }
    }
  }

  // Obtém o último argumento, se existir
  char *sufixo = NULL;
  if (novo_argc == 2) {
    sufixo = argv_copia[novo_argc - 1];
  } else if (novo_argc > 2) {
    printf("Erro: Mais de um sufixo encontrado\n");
    free(argv_copia);
    exit(1);
  }

  free(argv_copia);
  return sufixo;
}
