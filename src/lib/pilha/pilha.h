#ifndef PILHA_H
#define PILHA_H

/*
 * @brief Pilha é uma estrutura de dados do tipo LIFO (Last In, First Out),
 * onde o último elemento inserido é o primeiro a ser removido.
 */

typedef void *Pilha;

/**
 * @brief Inicializa a pilha, alocando a estrutura interna e definindo o topo como vazio
 * @param p ponteiro para a pilha
 */
void inicializar(Pilha *p);

/**
 * @brief Retorna 1 se a pilha está vazia, 0 caso contrário
 * @param p ponteiro para a pilha
 * @return 1 se vazia, 0 caso contrário
 */
int vazia(Pilha *p);

/**
 * @brief Libera toda a memória alocada para a pilha
 * @param p ponteiro para a pilha
 */
void liberar_pilha(Pilha *p);

/**
 * @brief Insere um elemento no topo da pilha (push)
 * @param p ponteiro para a pilha
 * @param valor valor inteiro a ser inserido
 * @return 1 em caso de sucesso ou 0 se a pilha estiver cheia
 */
int push(Pilha p, void *valor);

/**
 * @brief Remove o elemento do topo da pilha (pop) e armazena em 'removido'
 * @param p ponteiro para a pilha
 * @param removido ponteiro para onde o valor removido será armazenado
 * @return 1 em caso de sucesso ou 0 se a pilha estiver vazia
 */
int pop(Pilha *p, void *removido);

#endif