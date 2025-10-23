#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include <stdbool.h>
#include "../fila/fila.h"
#include "../geo/geo.h" // Inclui para ter acesso a Ground

// Enum para identificar o tipo da forma de maneira segura
typedef enum {
    TIPO_CIRCULO,
    TIPO_RETANGULO,
    TIPO_LINHA,
    TIPO_TEXTO
} TipoForma;

// ESTRUTURA DECLARADA AQUI
// Esta struct serve como um "invólucro" genérico para qualquer forma geométrica,
// adicionando metadados para a simulação.
typedef struct {
    int id_original;
    TipoForma tipo;
    void* dados_forma;      // Ponteiro para o círculo, retângulo, etc.
    bool foi_destruida;
    bool foi_clonada;
    float x_centro, y_centro; // Posição central para anotações
} FormaStruct;


// Tipos opacos para esconder a implementação
typedef void *Arena;

/**
 * @brief Processa os comandos de um arquivo .qry, manipula as formas na arena e gera saídas.
 * @param qry Ponteiro para o arquivo .qry.
 * @param svg Ponteiro para o arquivo SVG de saída.
 * @param ground A estrutura que contém todas as formas lidas do .geo.
 * @param txt Ponteiro para o arquivo de texto de saída.
 */
void process_qry(FILE *qry, FILE *svg, Ground ground, FILE *txt);

#endif