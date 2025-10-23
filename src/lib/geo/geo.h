#ifndef GEO_H
#define GEO_H

#include <stdio.h>
#include "../fila/fila.h"
#include "../pilha/pilha.h" // Adicionado

// Estrutura para manter o estado do "terreno" com todas as formas.
typedef struct {
    Fila todas_as_formas;      // Fila com todas as formas ativas na simulação
    Pilha pilha_clones_para_libertar; // Pilha para gerir a memória dos clones
} GroundStruct;

typedef void* Ground;

/**
 * @brief Processa um arquivo .geo, carrega todas as formas na estrutura Ground
 * e desenha o estado inicial no arquivo SVG.
 */
Ground process_geo(FILE *geo, FILE *svg);

/**
 * @brief Libera toda a memória associada à estrutura Ground, incluindo formas originais e clones.
 */
void destruir_ground(Ground g);

/**
 * @brief Obtém a fila de formas da estrutura Ground.
 */
Fila get_ground_fila(Ground g);

/**
 * @brief Obtém a pilha de clones da estrutura Ground.
 */
Pilha get_ground_pilha_clones(Ground g);

#endif