// Header para as funções do disparador/carregador
#ifndef DISPARADOR_H
#define DISPARADOR_H

#include <stdio.h>
#include "../pilha/pilha.h"

/*
 * Disparador
 * ----------
 * Objeto que contém uma pilha interna de 'disparo' (itens prontos para
 * serem disparados) e apontadores para dois carregadores (esquerdo e
 * direito). Cada carregador mantém uma pilha de munição (strings contendo
 * formas SVG lidas do arquivo .geo).
 *
 * A implementação esconde os detalhes das structs (tipos opacos).
 */
typedef struct Disparador Disparador;

/*
 * Carregador
 * ----------
 * Representa um recipiente de munição (pilha de strings). Também é
 * mantido como tipo opaco para o usuário da API.
 */
typedef struct Carregador Carregador;

/**
 * @brief Cria um carregador (aloca e inicializa a pilha interna)
 * @param id identificador numérico do carregador
 * @return ponteiro para Carregador alocado, ou NULL em caso de erro
 */
Carregador *criar_carredor(int id);

/**
 * @brief Cria um Disparador com coordenadas e carregadores internos
 * @param id identificador numérico do disparador
 * @param x coordenada x do disparador
 * @param y coordenada y do disparador
 * @return ponteiro para Disparador alocado, ou NULL em caso de erro
 */
Disparador *criar_disparador(int id, int x, int y);

/**
 * @brief Lê de um FILE (.geo) e empilha linhas no Carregador
 * @param arq ponteiro para FILE já aberto para leitura
 * @param c ponteiro para o Carregador alvo
 * @param n número de linhas/formas a serem lidas (se aplicável)
 */
void carregar_carregador(FILE *arq, Carregador *c, int n);

/**
 * @brief Executa ações de carregamento do Disparador, movendo itens entre
 *        pilha de disparo e carregadores conforme o comando
 * @param d ponteiro para o Disparador
 * @param n número de iterações a executar
 * @param comando "e" (esquerdo) ou "d" (direito)
 */
void carregar_disparador(Disparador *d, int n, char *comando);

/**
 * @brief Retorna o identificador do Disparador
 * @param d ponteiro para Disparador
 * @return id do Disparador, ou -1 se d for NULL
 */
int disparador_get_id(Disparador *d);

/* ---------------- Acessores ---------------- */

/** @brief Retorna o id do carregador (ou -1 se NULL) */
int carregador_get_id(Carregador *c);

/** @brief Retorna 1 se o carregador está vazio (0 caso contrário) */
int carregador_vazia(Carregador *c);

/**
 * @brief Remove (pop) um item do carregador
 * @param c ponteiro para o Carregador
 * @param out ponteiro onde será armazenado o item removido (void*)
 * @return 1 em caso de sucesso, 0 se vazio/erro
 */
int carregador_pop(Carregador *c, void **out);

/** @brief Retorna 1 se a pilha de disparo do Disparador está vazia */
int disparador_disp_vazia(Disparador *d);

/**
 * @brief Remove (pop) um item da pilha de disparo (disp)
 * @param d ponteiro para Disparador
 * @param out ponteiro onde será armazenado o item removido (void*)
 * @return 1 em caso de sucesso, 0 se vazio/erro
 */
int disparador_pop_disp(Disparador *d, void **out);

/** @brief Retorna o carregador esquerdo associado (ou NULL) */
Carregador *disparador_get_carregador_esq(Disparador *d);
/** @brief Retorna o carregador direito associado (ou NULL) */
Carregador *disparador_get_carregador_dir(Disparador *d);

/** @brief Retorna a coordenada X do disparador */
float disparador_get_x(Disparador *d);
/** @brief Retorna a coordenada Y do disparador */
float disparador_get_y(Disparador *d);

/**
 * @brief Associa um Carregador ao lado esquerdo do Disparador
 * @param d ponteiro para Disparador (alvo)
 * @param c ponteiro para Carregador a ser associado
 */
void disparador_set_carregador_esq(Disparador *d, Carregador *c);

/**
 * @brief Associa um Carregador ao lado direito do Disparador
 * @param d ponteiro para Disparador (alvo)
 * @param c ponteiro para Carregador a ser associado
 */
void disparador_set_carregador_dir(Disparador *d, Carregador *c);

/* -------- Relatórios (usados por qry.c) -------- */

/**
 * @brief Reporta no arquivo TXT a forma que está no topo da pilha de
 *        disparo do Disparador (sem consumir o item)
 */
void disparador_reportar_topo(Disparador *d, FILE *txt);

/**
 * @brief Reporta no arquivo TXT todas as figuras presentes no Carregador
 *        (do topo para a base), sem consumir os itens.
 */
void carregador_reportar_figuras(Carregador *c, FILE *txt);

/* -------- Destruição / limpeza -------- */

/**
 * @brief Destroi um Disparador liberando suas pilhas e (se apropriado)
 *        os carregadores associados.
 * @param d ponteiro para Disparador a ser destruído
 */
void destruir_disparador(Disparador *d);

/**
 * @brief Destroi um Carregador e libera sua munição
 * @param c ponteiro para Carregador a ser destruído
 */
void destruir_carregador(Carregador *c);

/**
 * @brief Remove e retorna a próxima forma a ser disparada.
 *        Prioridade: pilha interna do disparador, carregador esquerdo,
 *        carregador direito.
 * @param d Ponteiro para o disparador.
 * @return Ponteiro genérico (void*) para a forma (string SVG) ou NULL
 *         se não houver nada para disparar.
 */
void* disparar(Disparador *d);

#endif
