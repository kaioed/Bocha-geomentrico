#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stdio.h>
#include "../fila/fila.h"
#include "../pilha/pilha.h"

/**
 * @brief Abre um arquivo no modo especificado
 * @param nome nome do arquivo
 * @param modo modo de abertura (ex: "r", "w", "a")
 * @return ponteiro para o arquivo aberto
 */
FILE* abrirArquivo(const char *nome, const char *modo);

/**
 * @brief Fecha um arquivo
 * @param arq ponteiro para o arquivo
 */
void fecharArquivo(FILE *arq);

/**
 * @brief Escreve texto no arquivo
 * @param arq ponteiro para o arquivo
 * @param texto string a ser escrita
 */
void escreverTexto(FILE *arq, const char *texto);

/**
 * @brief Lê uma linha do arquivo
 * @param arq ponteiro para o arquivo
 * @param buffer buffer onde a linha será armazenada
 * @param tamanho tamanho máximo do buffer
 * @return 1 se leu uma linha, 0 se fim do arquivo
 */
int lerLinha(FILE *arq, char *buffer, int tamanho);

/**
 * @brief Lê todas as linhas do arquivo e insere em uma fila
 * @param arq ponteiro para o arquivo
 * @param fila ponteiro para a fila onde as linhas serão inseridas
 */
void lerArquivoParaFila(FILE *arq, Fila *fila);

/**
 * @brief Lê todas as linhas do arquivo e insere em uma pilha
 * @param arq ponteiro para o arquivo
 * @param pilha ponteiro para a pilha onde as linhas serão inseridas
 */
void lerArquivoParaPilha(FILE *arq, Pilha *pilha);

#endif
