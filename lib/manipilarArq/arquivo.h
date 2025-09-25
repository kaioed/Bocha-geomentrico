#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stdio.h>

// Abre um arquivo no modo especificado
FILE* abrirArquivo(const char *nome, const char *modo);

// Fecha um arquivo
void fecharArquivo(FILE *arq);

// Escreve texto no arquivo
void escreverTexto(FILE *arq, const char *texto);

// Lê uma linha do arquivo
int lerLinha(FILE *arq, char *buffer, int tamanho);


#endif
