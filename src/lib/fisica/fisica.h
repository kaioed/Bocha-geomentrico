#ifndef FISICA_H
#define FISICA_H

#include <stdio.h>
#include <stdbool.h>
// Forward declaration para evitar dependência circular
typedef void* Arena;
typedef void* Ground;

/**
 * @brief Processa colisões entre elementos da arena e formas do ground, calculando pontuações e danos
 * @param arena estrutura contendo os elementos disparados
 * @param ground estrutura contendo as formas originais do jogo
 * @param txt ponteiro para arquivo de texto para logging de eventos (pode ser NULL)
 * @param pontuacao ponteiro para acumular pontuação obtida nas colisões
 * @param esmagadas ponteiro para contar número de formas esmagadas
 * @param clonadas ponteiro para contar número de formas clonadas durante as colisões
 */
void fisica_processar_colisoes(Arena arena, Ground ground, FILE* txt, double* pontuacao, int* esmagadas, int* clonadas);

#endif