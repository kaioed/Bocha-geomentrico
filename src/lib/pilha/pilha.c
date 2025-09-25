#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

// Estrutura de um elemento da pilha
typedef struct elemento {
    void *dado;                
    struct elemento *prox;
} Elemento;

// Estrutura da pilha (ponteiro para o topo)
typedef struct {
    Elemento *topo;
} Pilha;

// Inicializa a pilha
void inicializar(Pilha *p) {
    p->topo = NULL;
}

// Verifica se a pilha está vazia
int vazia(Pilha *p) {
    return (p->topo == NULL);
}

// Insere um elemento (push)
int push(Pilha *p, void *valor) {    
    Elemento *novo = (Elemento *)malloc(sizeof(Elemento));
    if (novo == NULL) {
        // Falha na alocação
        return 0;
    }
    novo->dado = valor;
    novo->prox = p->topo;
    p->topo = novo;
    return 1;
}

// Remove um elemento (pop)
int pop(Pilha *p, void **removido) { 
    if (vazia(p)) {
        // Pilha vazia
        return 0;
    }
    Elemento *temp = p->topo;
    *removido = temp->dado;
    p->topo = temp->prox;
    free(temp);
    return 1;
}

// Libera toda a pilha
void liberar_pilha(Pilha *p) {
    Elemento *atual = p->topo;
    while (atual != NULL) {
        Elemento *temp = atual;
        atual = atual->prox;
        free(temp);
    }
    p->topo = NULL;
}