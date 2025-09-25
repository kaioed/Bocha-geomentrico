#ifndef FILA_H
#define FILA_H



typedef struct fila Fila;

// Inicializa um fila
void iniciar_fila(Fila *fila);

// Adiciona um elemento a fila;
void adicionar_na_fila(Fila *fila,void *dado);

// Retira um elemento da fila
void retirar_da_fila(Fila *fila,void *removido);

// Limpa a fila dada
void limpar_fila(Fila*fila);



#endif