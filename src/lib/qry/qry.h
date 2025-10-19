#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include <stdbool.h>
#include "../fila/fila.h"
#include "../formas/texto/texto.h"

// Tipos opacos para esconder a implementação da struct
typedef void *Forma;
typedef void *Arena;

// Enum para identificar o tipo da forma de maneira segura
typedef enum {
    TIPO_CIRCULO,
    TIPO_RETANGULO,
    TIPO_LINHA,
    TIPO_TEXTO
} TipoForma;

/**
 * @brief Processa os comandos de um arquivo .qry, manipula as formas na arena e gera saídas.
 * @param qry Ponteiro para o arquivo .qry.
 * @param svg Ponteiro para o arquivo SVG de saída.
 * @param geo Ponteiro para o arquivo .geo de entrada.
 * @param txt Ponteiro para o arquivo de texto de saída.
 */
void process_qry(FILE *qry, FILE *svg, FILE *geo, FILE *txt);

#endif