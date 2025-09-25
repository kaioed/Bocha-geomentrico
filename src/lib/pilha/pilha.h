#ifndef PILHA_H
#define PILHA_H

#define MAX 100  // tamanho máximo da pilha

typedef struct pilha Pilha;

// Inicializa a pilha
void inicializar(Pilha *p);

// Verifica se a pilha está vazia
int vazia(Pilha *p);

// Verifica se a pilha está cheia
void liberar_pilha(Pilha *p);

// Insere um elemento (push)
int push(Pilha *p, int valor);

// Remove um elemento (pop)
int pop(Pilha *p, int *removido);

#endif