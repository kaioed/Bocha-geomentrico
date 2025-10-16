#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disparador.h"
#include "../pilha/pilha.h"
#include "../manipilarArq/arquivo.h"

typedef struct Carregador {
    int id;
    Pilha municao;
} Carregador;

// Estrutura definida no header; aqui apenas inclusão de helpers locais se necessário
typedef struct Disparador
{
    int id;
    Pilha disp;
    Carregador* carregadorEsq;
    Carregador* carregadorDir;
    float x;
    float y;
} Disparador;

 Carregador *criar_carredor(int id)
 {
     Carregador *c = (Carregador *)malloc(sizeof(Carregador));
     if (!c) return NULL;
     c->id = id;
     inicializar(&c->municao);
     return c;
 }

Disparador *criar_disparador(int id, int x, int y)
{
    Disparador *d = (Disparador *)malloc(sizeof(Disparador));
    if (!d) NULL;
    d->id = id;
    d->x = x;
    d->y = y;
    /* aloca e inicializa carregadores */
    d->carregadorEsq = (Carregador*) malloc(sizeof(Carregador));
    d->carregadorDir = (Carregador*) malloc(sizeof(Carregador));
    if (!d->carregadorEsq || !d->carregadorDir) {
        /* falha na alocação: liberar e sair */
        free(d->carregadorEsq);
        free(d->carregadorDir);
        free(d);
        return NULL;
    }
    inicializar(&d->carregadorEsq->municao);
    inicializar(&d->disp);
    inicializar(&d->carregadorDir->municao);
    return d;
}

void carregar_carregador(FILE *arq, Carregador *c, int n)
{
    if (!c)
        return;
    for (int i = 0; i < n; i++)
    {
        lerArquivoParaPilha(arq, &c->municao);
    }
}

void carregar_disparador(Disparador *d, int n, char *comando)
{
    int i = 0;
    while (i < n)
    {
        if (vazia(&d->disp))
        {
            if (strcmp(comando, "e") == 0)
            {
                if (!vazia(&d->carregadorEsq->municao))
                {
                    void *linha;
                    pop(&d->carregadorEsq->municao, &linha);
                    push(&d->disp, linha);
                }
                else
                {
                    break; // Carregador esquerdo está vazio
                }
            }
            else if (strcmp(comando, "d") == 0)
            {
                if (!vazia(&d->carregadorDir->municao))
                {
                    void *linha;
                    pop(&d->carregadorDir->municao, &linha);
                    push(&d->disp, linha);
                }
                else
                {
                    break; // Carregador direito está vazio
                }
            }
            i++;
        }
        else
        {
            if (strcmp(comando, "e") == 0)
            {
                if (!vazia(&d->carregadorEsq->municao))
                {
                    void *linha;
                    pop(&d->carregadorEsq->municao, &linha);
                    push(&d->carregadorDir->municao, linha);
                }
            }
            else if (strcmp(comando, "d") == 0)
            {
                if (!vazia(&d->carregadorDir->municao))
                {
                    void *linha;
                    pop(&d->carregadorDir->municao, &linha);
                    push(&d->carregadorEsq->municao, linha);
                }
            }
        }
    }
}

int disparador_get_id(Disparador *d)
{
    if (!d) return -1;
    return d->id;
}

int carregador_get_id(Carregador *c)
{
    if (!c) return -1;
    return c->id;
}

void disparador_set_carregador_esq(Disparador *d, Carregador *c)
{
    if (!d) return;
    d->carregadorEsq = c;
}

void disparador_set_carregador_dir(Disparador *d, Carregador *c)
{
    if (!d) return;
    d->carregadorDir = c;
}