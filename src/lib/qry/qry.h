#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include <stdbool.h>
#include "../fila/fila.h"
#include "../geo/geo.h" 


typedef enum {
    TIPO_CIRCULO,
    TIPO_RETANGULO,
    TIPO_LINHA,
    TIPO_TEXTO
} TipoForma;


typedef void* Forma;

typedef void *Arena;

/**
 * @brief Obtém o tipo da forma encapsulada (e.g., TIPO_CIRCULO, TIPO_RETANGULO).
 * @param forma Ponteiro opaco (FormaStruct) para a estrutura da forma.
 * @return O enumerador TipoForma que representa o tipo da forma.
 */
TipoForma forma_get_tipo(void* forma);

/**
 * @brief Obtém o ID original da forma contida na estrutura (FormaStruct).
 * Este ID pode ser o ID lido do .geo ou um ID de clone.
 * @param forma Ponteiro opaco (FormaStruct) para a estrutura da forma.
 * @return O ID inteiro da forma.
 */
int forma_get_id_original(void* forma);


/**
 * @brief Define o status de destruição da forma.
 * @param forma Ponteiro opaco (FormaStruct) para a estrutura da forma.
 * @param status Booleano indicando se a forma foi destruída (true) ou não (false).
 */
void forma_set_destruida(Forma forma, bool status);

/**
 * @brief Processa os comandos de um arquivo .qry, manipula as formas na arena e gera saídas.
 * @param qry Ponteiro para o arquivo .qry.
 * @param svg Ponteiro para o arquivo SVG de saída.
 * @param ground A estrutura que contém todas as formas lidas do .geo.
 * @param txt Ponteiro para o arquivo de texto de saída.
 */
void process_qry(FILE *qry, FILE *svg, Ground ground, FILE *txt);

#endif