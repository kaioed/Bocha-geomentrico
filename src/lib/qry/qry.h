#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include "../fila/fila.h"

/**
 * @brief Processa os comandos em uma fila gerada a partir de um .qry e escreve em um SVG
 * @param qry ponteiro para o arquivo .qry (pode ser NULL se a fila já estiver preenchida)
 * @param svg ponteiro para o arquivo SVG de saída
 */
void process_qry(FILE *qry, FILE *svg, FILE *geo, FILE *txt);

#endif
