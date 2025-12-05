#ifndef ARENA_H
#define ARENA_H

#include "../fila/fila.h"
// Forward declaration para evitar dependência circular
typedef void* Elemento;

/*
 * @brief Arena é o espaço onde os elementos (formas disparadas e clonadas) interagem e sofrem colisões.
 */
typedef void* Arena;

/**
 * @brief Cria uma nova arena (campo de batalha) vazia
 * @return ponteiro opaco para a arena criada
 */
Arena arena_criar();

/**
 * @brief Destrói uma arena e libera toda a memória alocada
 * @param a ponteiro para a arena a ser destruída
 */
void arena_destruir(Arena a);

/**
 * @brief Adiciona um elemento (forma) à arena
 * @param a ponteiro para a arena
 * @param e elemento a ser adicionado
 */
void arena_adicionar_elemento(Arena a, Elemento e);

/**
 * @brief Obtém a fila de elementos da arena
 * @param a ponteiro para a arena
 * @return fila contendo todos os elementos da arena
 */
Fila arena_get_fila(Arena a);

/**
 * @brief Limpa a arena removendo todos os elementos
 * @param a ponteiro para a arena
 */
void arena_limpar(Arena a);

#endif