#ifndef FILA_H
#define FILA_H

/*
 * @brief Fila é uma estrutura de dados do tipo FIFO (First In, First Out),
 * onde o primeiro elemento inserido é o primeiro a ser removido.
 */

typedef void *Fila;

/**
 * @brief Inicializa a fila, alocando a estrutura interna e definindo o início e fim como vazios
 * @param fila ponteiro para a fila
 */
Fila iniciar_fila();

/**
 * @brief Adiciona um elemento ao final da fila
 * @param fila ponteiro para a fila
 * @param dado ponteiro para o elemento a ser inserido
 */
void adicionar_na_fila(Fila fila, void *dado);

/**
 * @brief Remove o elemento do início da fila e armazena em 'removido'
 * @param fila ponteiro para a fila
 * @param removido ponteiro para onde o dado removido será armazenado
 */
int remover_da_fila(Fila fila, void *removido);

/**
 * @brief Destroi a fila e libera a memória alocada
 * @param fila a fila a ser destruída
 */
void destruir_fila(Fila fila);

/**
 * @brief Verifica se a fila está vazia
 * @param fila ponteiro para a fila
 * @return 1 se vazia, 0 caso contrário
 */ 
int checar_fila_vazia(Fila fila);

#endif