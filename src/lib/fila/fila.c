#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

// Estrutura interna (privada, visível só neste arquivo)
typedef struct no {
    void* dado;
    struct no* prox;
} No;

typedef struct {
    No* inicio;
    No* fim;
} FilaStruct;

Fila iniciar_fila() {
    FilaStruct* f = malloc(sizeof(FilaStruct));
    f->inicio = NULL;
    f->fim = NULL;
    return f;
}

void adicionar_na_fila(Fila fila, void* dado) {
    FilaStruct* f = (FilaStruct*)fila;
    No* novo = malloc(sizeof(No));
    novo->dado = dado;
    novo->prox = NULL;

    if (f->fim == NULL) {
        f->inicio = novo;
    } else {
        f->fim->prox = novo;
    }
    f->fim = novo;
}

int remover_da_fila(Fila fila, void* removido) {
    FilaStruct* f = (FilaStruct*)fila;
    if (f->inicio == NULL) return 0; // fila vazia

    No* temp = f->inicio;
    f->inicio = temp->prox;
    if (f->inicio == NULL) f->fim = NULL;

    if (removido != NULL)
        *(void**)removido = temp->dado;

    free(temp);
    return 1;
}

void destruir_fila(Fila fila) {
    FilaStruct* f = (FilaStruct*)fila;
    No* atual = f->inicio;
    while (atual != NULL) {
        No* prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(f);
}

int checar_fila_vazia(Fila fila) {
    FilaStruct* f = (FilaStruct*)fila;
    if(f->inicio == NULL) {
        return 1;
    }
    return 0;
}
