#ifndef PILHA_H
#define PILHA_H

/*
    Pilha e uma estrutura de dados do tipo LIFO (Last In, First Out),
    onde o último elemento inserido é o primeiro a ser removido.
*/

typedef void *Pilha;

// Inicializa a pilha, alocando a estrutura interna e definindo o topo como vazio
void inicializar(Pilha *p);

// Retorna 1 se a pilha está vazia, 0 caso contrário
int vazia(Pilha *p);

// Libera toda a memória alocada para a pilha
void liberar_pilha(Pilha *p);

// Insere um elemento no topo da pilha (push)
// Retorna 1 em caso de sucesso ou 0 se a pilha estiver cheia
int push(Pilha *p, int valor);

// Remove o elemento do topo da pilha (pop) e armazena em 'removido'
// Retorna 1 em caso de sucesso ou 0 se a pilha estiver vazia
int pop(Pilha *p, int *removido);

#endif