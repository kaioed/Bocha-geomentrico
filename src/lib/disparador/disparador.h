// Header para as funções do disparador/carregador
#ifndef DISPARADOR_H
#define DISPARADOR_H

#include <stdio.h>
#include "../pilha/pilha.h"
// Forward-declarações (implementações em disparador.c)
typedef struct Disparador Disparador;
// Estrutura Carregador usada pelo Disparador (definida em disparador.c)
typedef struct Carregador Carregador;

/**
 * @brief Cria um carregador (usado internamente pelo Disparador)
 * @param id identificador numérico do carregador
 * @return ponteiro para Carregador alocado (NULL em caso de erro)
 */
Carregador *criar_carredor(int id);

/**
 * @brief Cria um objeto Disparador
 * @param id identificador numérico do disparador
 * @return ponteiro para Disparador alocado (NULL em caso de erro)
 * @param x coordenada x do disparador
 * @param y coordenada y do disparador
 */
Disparador *criar_disparador(int id, int x, int y);

/**
 * @brief Carrega dados de um arquivo já aberto para um Carregador (p.ex. .geo)
 * @param arq ponteiro para FILE já aberto para leitura
 * @param c ponteiro para o Carregador alvo
 * @param n número de vezes que a leitura deve ser repetida (se aplicável)
 */
void carregar_carregador(FILE *arq, Carregador *c, int n);

/**
 * @brief Executa o ciclo de carregamento do disparador, movendo elementos entre pilhas
 * @param d ponteiro para o Disparador
 * @param n número de iterações a executar
 * @param comando string com o comando ('e' para esquerdo, 'd' para direito)
 */
void carregar_disparador(Disparador *d, int n, char *comando);

/**
 * @brief Retorna o identificador do Disparador
 * @param d ponteiro para Disparador
 * @return id do Disparador, ou -1 se d for NULL
 */
int disparador_get_id(Disparador *d);

/* Acessores para Carregador/Disparador (evitam expor struct internals)
 */
int carregador_get_id(Carregador *c);
/**
 * @brief Retorna o identificador do Carregador
 * @param c ponteiro para Carregador
 * @return id do Carregador, ou -1 se c for NULL
 */
int carregador_get_id(Carregador *c);

/**
 * @brief Associa um Carregador ao lado esquerdo de um Disparador
 * @param d ponteiro para Disparador (alvo)
 * @param c ponteiro para Carregador a ser associado
 *
 * Função utilitária para encapsular a atribuição do carregador esquerdo
 * sem expor a estrutura interna do Disparador no header.
 */
void disparador_set_carregador_esq(Disparador *d, Carregador *c);

/**
 * @brief Associa um Carregador ao lado direito de um Disparador
 * @param d ponteiro para Disparador (alvo)
 * @param c ponteiro para Carregador a ser associado
 *
 * Função utilitária para encapsular a atribuição do carregador direito
 * sem expor a estrutura interna do Disparador no header.
 */
void disparador_set_carregador_dir(Disparador *d, Carregador *c);

#endif
