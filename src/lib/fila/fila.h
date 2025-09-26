#ifndef FILA_H
#define FILA_H

/*
    Fila é uma estrutura de dados do tipo FIFO (First In, First Out),
    onde o primeiro elemento inserido é o primeiro a ser removido.
*/

typedef void *Fila;

// Inicializa a fila, alocando a estrutura interna e definindo o início e fim como vazios
void iniciar_fila(Fila *fila);

// Adiciona um elemento ao final da fila
// 'dado' é um ponteiro para o elemento a ser inserido
void adicionar_na_fila(Fila *fila, void *dado);

// Remove o elemento do início da fila e armazena em 'removido'
// 'removido' deve ser um ponteiro para onde o dado removido será armazenado
void retirar_da_fila(Fila *fila, void *removido);

// Remove todos os elementos da fila e libera a memória alocada
void limpar_fila(Fila *fila);

#endif