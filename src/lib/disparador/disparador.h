// Header para as funções do disparador/carregador
#ifndef DISPARADOR_H
#define DISPARADOR_H

#include <stdio.h>
#include "../pilha/pilha.h"
#include "../fila/fila.h"

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
typedef void *Disparador;

/*
 * Carregador
 * ----------
 * Representa um recipiente de munição (pilha de strings). Também é
 * mantido como tipo opaco para o usuário da API.
 */
typedef void *Carregador;

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
 * @param log ponteiro para FILE de log (pode ser NULL)
 */
void carregar_carregador(Fila fonte, Carregador *c, int n,FILE *log);

/**
 * @brief Executa ações de carregamento do Disparador, movendo itens entre
 *        pilha de disparo e carregadores conforme o comando
 * @param d ponteiro para o Disparador
 * @param n número de iterações a executar
 * @param comando "e" (esquerdo) ou "d" (direito)
 */
void carregar_disparador(Disparador *d, int n, char *comando);

/**
 * @brief Obtém a coordenada X do disparador
 * @param d ponteiro para o Disparador
 * @return coordenada X do disparador
 */
float disparador_get_x(Disparador *d);

/**
 * @brief Obtém a coordenada Y do disparador
 * @param d ponteiro para o Disparador
 * @return coordenada Y do disparador
 */
float disparador_get_y(Disparador *d);

/**
 * @brief Obtém o ID do disparador
 * @param d ponteiro para o Disparador
 * @return ID do disparador
 */
int disparador_get_id(Disparador *d);

/**
 * @brief Destroi um disparador e libera toda a memória alocada
 * @param d ponteiro para o Disparador a ser destruído
 */
void destruir_disparador(Disparador *d);

/**
 * @brief Destroi um carregador e libera toda a memória alocada
 * @param c ponteiro para o Carregador a ser destruído
 */
void destruir_carregador(Carregador *c);

/**
 * @brief Define o carregador direito do disparador
 * @param d ponteiro para o Disparador
 * @param c ponteiro para o Carregador a ser definido como direito
 */
void disparador_set_carregador_dir(Disparador *d, Carregador *c);

/**
 * @brief Define o carregador esquerdo do disparador
 * @param d ponteiro para o Disparador
 * @param c ponteiro para o Carregador a ser definido como esquerdo
 */
void disparador_set_carregador_esq(Disparador *d, Carregador *c);

/**
 * @brief Obtém o ID do carregador
 * @param c ponteiro para o Carregador
 * @return ID do carregador
 */
int carregador_get_id(Carregador *c);

/**
 * @brief Verifica se o carregador está vazio
 * @param c ponteiro para o Carregador
 * @return 1 se vazio, 0 caso contrário
 */
int carregador_vazio(Carregador *c);


/**
 * @brief Destroi um carregador e libera toda a memória alocada
 * @param c ponteiro para o Carregador a ser destruído
 */
void carregador_destruir(Carregador *c);

/**
 * @brief Obtém a forma no topo da pilha de disparo sem removê-la
 * @param d ponteiro para o Disparador
 * @return ponteiro para a forma no topo da pilha, ou NULL se vazia
 */
void *disparador_obter_forma_disparo(Disparador *d);

#endif // DISPARADOR_H