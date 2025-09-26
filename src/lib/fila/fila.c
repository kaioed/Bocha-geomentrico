#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

typedef struct elemento{
    void *dado;                    
    struct elemento *prox;
}Elemento;

typedef struct fila
{
  Elemento *inicio;
  Elemento *final;
}Fila;

// Inicializa a fila, definindo início e final como NULL
Fila *iniciar_fila(){
    Fila *f = (Fila *)malloc(sizeof(Fila));
    f->final = NULL;
    f->inicio = NULL;
}

// Adiciona um elemento ao final da fila
void adicionar_na_fila(Fila *fila, void *dado){ 
    Elemento *temp = (Elemento *)malloc(sizeof(Elemento));
    if(temp == NULL){
        exit(1); // Encerra o programa se não houver memória
    }
    temp->dado = dado;
    temp->prox = NULL;

    if(fila->final == NULL && fila->inicio == NULL){
        // Se a fila está vazia, o novo elemento é início e final
        fila->inicio = temp;
        fila->final = temp;
    }else{
        // Caso contrário, adiciona ao final
        fila->final->prox = temp;
        fila->final = temp;
    }
}

// Libera todos os elementos da fila e imprime seus valores
void limpar_fila(Fila *f){ // Função de impressão genérica
    Elemento *atual = f->inicio;
    while (atual != NULL) {
        Elemento *temp = atual;
        atual = atual->prox;
        free(temp);
    }
    f->final = NULL;
    f->inicio = NULL;
}


// Remove o primeiro elemento da fila (dequeue) e retorna seu valor em 'removido'
void retirar_da_fila(Fila *fila, void *removido) {
    if (fila->inicio == NULL) {
        // Fila vazia, não remove nada
        return;
    }
    Elemento *temp = fila->inicio;
    removido = temp->dado;
    fila->inicio = temp->prox;
    if (fila->inicio == NULL) {
        // Se a fila ficou vazia, atualiza o final também
        fila->final = NULL;
    }
    free(temp);
    return;
}