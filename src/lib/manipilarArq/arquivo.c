#include "arquivo.h"
#include <stdlib.h>
#include <string.h>

FILE* abrirArquivo(const char *nome, const char *modo) {
    FILE *arq = fopen(nome, modo);
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo %s!\n", nome);
        exit(1);
    }
    return arq;
}

void fecharArquivo(FILE *arq) {
    fclose(arq);
}


int lerLinha(FILE *arq, char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, arq) == NULL)
        return 0; // fim do arquivo
    return 1;
}

static char *duplicate_string(const char *s) {
  if (s == NULL)
    return NULL;

  size_t len = strlen(s) + 1;
  char *dup = malloc(len);
  if (dup != NULL) {
    strcpy(dup, s);
  }
  return dup;
}

void lerArquivoParaFila(FILE *arq, Fila *fila) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), arq) != NULL) {
        char *linha = duplicate_string(buffer);
        adicionar_na_fila(fila, linha);
    }
}

void lerArquivoParaPilha(FILE *arq, Pilha *pilha) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), arq) != NULL) {
        char *linha = duplicate_string(buffer);
        push(pilha, linha);
    }
}
