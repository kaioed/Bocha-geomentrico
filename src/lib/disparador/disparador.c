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
    if (!d) return NULL;
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
                    pop(&d->disp, &linha);
                    push(&d->carregadorDir->municao, linha);
                    pop(&d->carregadorEsq->municao, &linha);
                    push(&d->disp, linha);
                }
            }
            else if (strcmp(comando, "d") == 0)
            {
                if (!vazia(&d->carregadorDir->municao))
                {
                    void *linha;
                    pop(&d->disp, &linha);
                    push(&d->carregadorEsq->municao, linha);
                    pop(&d->carregadorDir->municao, &linha);
                    push(&d->disp, linha);
                }
                else
                {
                    break; // Carregador direito está vazio
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

int carregador_vazia(Carregador *c)
{
    if (!c) return 1;
    return vazia(&c->municao);
}

int carregador_pop(Carregador *c, void **out)
{
    if (!c || !out) return 0;
    return pop(&c->municao, out);
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

Carregador *disparador_get_carregador_esq(Disparador *d)
{
    if (!d) return NULL;
    return d->carregadorEsq;
}

Carregador *disparador_get_carregador_dir(Disparador *d)
{
    if (!d) return NULL;
    return d->carregadorDir;
}

int disparador_disp_vazia(Disparador *d)
{
    if (!d) return 1;
    return vazia(&d->disp);
}

int disparador_pop_disp(Disparador *d, void **out)
{
    if (!d || !out) return 0;
    return pop(&d->disp, out);
}

float disparador_get_x(Disparador *d)
{
    if (!d) return 0.0f;
    return d->x;
}

float disparador_get_y(Disparador *d)
{
    if (!d) return 0.0f;
    return d->y;
}

void destruir_disparador(Disparador *d)
{
    if (!d) return;
    void *item = NULL;

    /* Esvazia e libera a pilha de disparo */
    while (pop(&d->disp, &item)) {
        if (item) free(item);
        item = NULL;
    }
    liberar_pilha(&d->disp);

    /* Se carregadores foram alocados internamente, destruí-los.
       Aqui assumimos que, quando criados no criar_disparador, o Disparador
       é o dono e deve destruí-los. Caso carregadores sejam associados
       externamente via setters, a responsabilidade de liberação deve ser
       claramente documentada ou controlada via flags de ownership. */
    if (d->carregadorEsq) {
        destruir_carregador(d->carregadorEsq);
        d->carregadorEsq = NULL;
    }
    if (d->carregadorDir) {
        destruir_carregador(d->carregadorDir);
        d->carregadorDir = NULL;
    }

    free(d);
}

void destruir_carregador(Carregador *c)
{
    if (!c) return;
    void *item;
    while (!vazia(&c->municao)) {
        pop(&c->municao, &item);
        free(item); // Assumindo que os itens são alocados dinamicamente
    }
    free(c);
}

/*
 * Reporta no arquivo 'txt' a figura que está no topo da pilha de disparo
 * do Disparador (se existir). A função NÃO deve consumir o item; para isso
 * usamos uma pilha temporária para preservar a ordem/itens.
 */
void disparador_reportar_topo(Disparador *d, FILE *txt)
{
    if (!d || !txt) return;
    if (vazia(&d->disp)) {
        fprintf(txt, "Top do disparador %d: vazio\n", d->id);
        return;
    }

    Pilha temp;
    inicializar(&temp);
    void *item = NULL;

    // Desempilha apenas o topo (preservando-o em temp) e imprime
    if (pop(&d->disp, &item)) {
        if (item) {
            // Garante que a linha tem newline ao final no TXT
            fprintf(txt, "Top do disparador %d: %s", d->id, (char*)item);
        } else {
            fprintf(txt, "Top do disparador %d: (item nulo)\n", d->id);
        }
        push(&temp, item);
    }

    // Restaura os itens da pilha temporária de volta ao disp
    while (!vazia(&temp)) {
        void *t = NULL;
        pop(&temp, &t);
        push(&d->disp, t);
    }
    liberar_pilha(&temp);
}

/*
 * Reporta no arquivo 'txt' todas as figuras carregadas no Carregador,
 * do topo para a base, sem consumir os itens.
 */
void carregador_reportar_figuras(Carregador *c, FILE *txt)
{
    if (!c || !txt) return;

    Pilha temp;
    inicializar(&temp);
    void *item = NULL;

    // Se a pilha estiver vazia, reporta
    if (vazia(&c->municao)) {
        fprintf(txt, "Carregador %d: vazio\n", c->id);
        liberar_pilha(&temp);
        return;
    }

    // 1) Transfere todos os itens para 'temp' apenas para contar
    int count = 0;
    while (pop(&c->municao, &item)) {
        push(&temp, item);
        count++;
        item = NULL;
    }

    // 2) Imprime cabeçalho com número de itens
    fprintf(txt, "Carregador %d: %d itens\n", c->id, count);

    // 3) Restaura da 'temp' para a pilha original e imprime cada item
    while (!vazia(&temp)) {
        void *t = NULL;
        pop(&temp, &t); // t contém o próximo item do topo->base
        if (t) fprintf(txt, "  - %s", (char*)t);
        push(&c->municao, t);
    }
    liberar_pilha(&temp);
}

void* disparar(Disparador *d) {
    if (d == NULL) {
        return NULL; // Validação de segurança
    }

    void *item = NULL;

    // 1. Tenta obter da pilha de disparo principal
    if (!disparador_disp_vazia(d)) {
        disparador_pop_disp(d, &item);
    }

    // 2. Se não conseguiu (item ainda é NULL), tenta obter do carregador esquerdo
    if (item == NULL) {
        Carregador *carregador_esq = disparador_get_carregador_esq(d);
        if (carregador_esq != NULL && !carregador_vazia(carregador_esq)) {
            carregador_pop(carregador_esq, &item);
        }
    }

    // 3. Se ainda não conseguiu, tenta obter do carregador direito
    if (item == NULL) {
        Carregador *carregador_dir = disparador_get_carregador_dir(d);
        if (carregador_dir != NULL && !carregador_vazia(carregador_dir)) {
            carregador_pop(carregador_dir, &item);
        }
    }
    
    // 4. Retorna o item encontrado, ou NULL se nada foi encontrado
    return item;
}