#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

// Estruturas internas (visíveis apenas neste arquivo)
typedef struct elemento {
    void *dado;
    struct elemento *prox;
} Elemento;

typedef struct {
    Elemento *topo;
} PilhaStruct;

// Inicializa a pilha: aloca a estrutura interna e define topo como NULL
void inicializar(Pilha *p) {
    if (!p) return;
    PilhaStruct *ps = (PilhaStruct*) malloc(sizeof(PilhaStruct));
    if (!ps) return;
    ps->topo = NULL;
    *p = (Pilha)ps;
}

// Retorna 1 se a pilha está vazia, 0 caso contrário
int vazia(Pilha *p) {
    if (!p || !*p) return 1;
    PilhaStruct *ps = (PilhaStruct*) *p;
    return (ps->topo == NULL);
}

// Insere um elemento no topo da pilha (push)
// Retorna 1 em caso de sucesso ou 0 se falhar
int push(Pilha *p, void *valor) {
    if (!p || !*p) return 0;
    PilhaStruct *ps = (PilhaStruct*) *p;
    Elemento *novo = (Elemento*) malloc(sizeof(Elemento));
    if (!novo) return 0;
    novo->dado = valor;
    novo->prox = ps->topo;
    ps->topo = novo;
    return 1;
}

// Remove o elemento do topo da pilha (pop) e armazena em 'removido'
// Retorna 1 em caso de sucesso ou 0 se a pilha estiver vazia
int pop(Pilha *p, void *removido) {
    if (!p || !*p) return 0;
    PilhaStruct *ps = (PilhaStruct*) *p;
    if (ps->topo == NULL) return 0;
    Elemento *temp = ps->topo;
    ps->topo = temp->prox;
    if (removido != NULL) {
        *(void**)removido = temp->dado;
    }
    free(temp);
    return 1;
}

// Libera toda a memória da pilha e zera o ponteiro
void liberar_pilha(Pilha *p) {
    if (!p || !*p) return;
    PilhaStruct *ps = (PilhaStruct*) *p;
    Elemento *atual = ps->topo;
    while (atual) {
        Elemento *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(ps);
    *p = NULL;
}