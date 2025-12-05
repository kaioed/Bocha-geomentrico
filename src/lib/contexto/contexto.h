#ifndef CONTEXTO_H
#define CONTEXTO_H

#include "../disparador/disparador.h"
#include "../fila/fila.h"
#include <stdio.h>

/*
 * @brief Contexto é o gerenciador de disparadores e carregadores, mantendo suas associações.
 */
typedef void* Contexto;

/**
 * @brief Cria um novo contexto vazio
 * @return ponteiro opaco para o contexto criado
 */
Contexto contexto_criar();

/**
 * @brief Destrói um contexto e libera toda a memória alocada
 * @param ctx ponteiro para o contexto a ser destruído
 */
void contexto_destruir(Contexto ctx);

/**
 * @brief Adiciona um disparador ao contexto com as coordenadas especificadas
 * @param ctx ponteiro para o contexto
 * @param id identificador numérico do disparador
 * @param x coordenada x do disparador
 * @param y coordenada y do disparador
 */
void contexto_adicionar_disparador(Contexto ctx, int id, float x, float y);

/**
 * @brief Busca um disparador no contexto pelo seu ID
 * @param ctx ponteiro para o contexto
 * @param id identificador do disparador a buscar
 * @return ponteiro para o disparador encontrado, ou NULL se não existir
 */
Disparador contexto_buscar_disparador(Contexto ctx, int id);

/**
 * @brief Adiciona um carregador ao contexto e o carrega com elementos da fila
 * @param ctx ponteiro para o contexto
 * @param id identificador numérico do carregador
 * @param ground fila de elementos para carregar no carregador
 * @param n número de elementos a carregar
 * @param log ponteiro para arquivo de log (pode ser NULL)
 */
void contexto_adicionar_carregador(Contexto ctx, int id, Fila ground, int n, FILE* log);

/**
 * @brief Busca um carregador no contexto pelo seu ID
 * @param ctx ponteiro para o contexto
 * @param id identificador do carregador a buscar
 * @return ponteiro para o carregador encontrado, ou NULL se não existir
 */
Carregador contexto_buscar_carregador(Contexto ctx, int id);

/**
 * @brief Vincula dois carregadores (esquerdo e direito) a um disparador
 * @param ctx ponteiro para o contexto
 * @param id_disp ID do disparador a receber os carregadores
 * @param id_ce ID do carregador esquerdo
 * @param id_cd ID do carregador direito
 * @param log ponteiro para arquivo de log (pode ser NULL)
 */
void contexto_vincular_equipamentos(Contexto ctx, int id_disp, int id_ce, int id_cd, FILE* log);

#endif