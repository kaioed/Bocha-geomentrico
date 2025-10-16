#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include "../fila/fila.h"

/**
 * @brief Lê um arquivo .qry e insere cada linha em uma fila
 * @param arq ponteiro para o arquivo .qry aberto
 * @param fila fila onde as linhas serão inseridas
 */
void ler_qry(FILE *arq, Fila fila);

/**
 * @brief Processa os comandos em uma fila gerada a partir de um .qry e escreve em um SVG
 * @param qry ponteiro para o arquivo .qry (pode ser NULL se a fila já estiver preenchida)
 * @param svg ponteiro para o arquivo SVG de saída
 */
void process_qry(FILE *qry, FILE *svg, FILE *geo, FILE *txt);

#endif
