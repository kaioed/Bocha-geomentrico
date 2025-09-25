#include "arquivo.h"
#include <stdlib.h>

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

void escreverTexto(FILE *arq, const char *texto) {
    fprintf(arq, "%s\n", texto);
}

int lerLinha(FILE *arq, char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, arq) == NULL)
        return 0; // fim do arquivo
    return 1;
}

