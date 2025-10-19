#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disparador.h"
#include "../pilha/pilha.h"
#include "../manipilarArq/arquivo.h"
#include "../fila/fila.h"
#include "../qry/qry.h"

// Estruturas internas - não expostas no header
typedef struct CarregadorInterno {
    int id;
    Pilha municao;
} CarregadorInterno;

typedef struct DisparadorInterno
{
    int id;
    Pilha disp;
    CarregadorInterno* carregadorEsq;
    CarregadorInterno* carregadorDir;
    float x;
    float y;
} DisparadorInterno;

Carregador *criar_carredor(int id)
{
    CarregadorInterno *c = (CarregadorInterno *)malloc(sizeof(CarregadorInterno));
    if (!c) return NULL;
    c->id = id;
    inicializar(&c->municao);
    return (Carregador *)c;
}

Disparador *criar_disparador(int id, int x, int y)
{
    DisparadorInterno *d = (DisparadorInterno *)malloc(sizeof(DisparadorInterno));
    if (!d) return NULL;
    d->id = id;
    d->x = x;
    d->y = y;
    /* aloca e inicializa carregadores */
    d->carregadorEsq = (CarregadorInterno*) malloc(sizeof(CarregadorInterno));
    d->carregadorDir = (CarregadorInterno*) malloc(sizeof(CarregadorInterno));
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
    return (Disparador *)d;
}

TipoForma forma_get_tipo(Forma f);
void* forma_get_dados(Forma f);
int forma_get_id(Forma f);

void carregar_carregador(Fila fonte, Carregador *c, int n,FILE *log)
{
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    if (!carreg) return;

    for (int i = 0; i < n; i++) {
        void *item_wrapper = NULL;
        if (remover_da_fila(fonte, &item_wrapper)) {
            if (item_wrapper) {
                push(&carreg->municao, item_wrapper);
                
                // Lógica de Log (agora segura)
                if (log) {
                    Forma f = (Forma)item_wrapper;
                    const char* tipo_str = "Desconhecido";
                    switch(forma_get_tipo(f)) {
                        case TIPO_CIRCULO:   tipo_str = "Circulo";   break;
                        case TIPO_RETANGULO: tipo_str = "Retangulo"; break;
                        case TIPO_LINHA:     tipo_str = "Linha";     break;
                        case TIPO_TEXTO:     tipo_str = "Texto";     break;
                    }
                    fprintf(log, "Carregador %d: inserindo forma ID %d (tipo %s)\n", carreg->id, forma_get_id(f), tipo_str);
                }
            }
        } else {
            break; 
        }
    }

}

void carregar_disparador(Disparador *d, int n, char *comando)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    int i = 0;
    while (i < n)
    {
        if (vazia(&disp->disp))
        {
            if (strcmp(comando, "e") == 0)
            {
                if (!vazia(&disp->carregadorEsq->municao))
                {
                    void *linha;
                    pop(&disp->carregadorEsq->municao, &linha);
                    push(&disp->disp, linha);
                }
                else
                {
                    break; // Carregador esquerdo está vazio
                }
            }
            else if (strcmp(comando, "d") == 0)
            {
                if (!vazia(&disp->carregadorDir->municao))
                {
                    void *linha;
                    pop(&disp->carregadorDir->municao, &linha);
                    push(&disp->disp, linha);
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
                if (!vazia(&disp->carregadorEsq->municao))
                {
                    void *linha;
                    pop(&disp->disp, &linha);
                    push(&disp->carregadorDir->municao, linha);
                    pop(&disp->carregadorEsq->municao, &linha);
                    push(&disp->disp, linha);
                }
            }
            else if (strcmp(comando, "d") == 0)
            {
                if (!vazia(&disp->carregadorDir->municao))
                {
                    void *linha;
                    pop(&disp->disp, &linha);
                    push(&disp->carregadorEsq->municao, linha);
                    pop(&disp->carregadorDir->municao, &linha);
                    push(&disp->disp, linha);
                }
            }
        }
    }
}

float disparador_get_x(Disparador *d)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    if (!disp) return 0.0f;
    return disp->x;
}

float disparador_get_y(Disparador *d)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    if (!disp) return 0.0f;
    return disp->y;
}

int disparador_get_id(Disparador *d)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    if (!disp) return -1;
    return disp->id;
}

void destruir_carregador(Carregador *c)
{
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    if (!carreg) return;
    
    // Libera a pilha de munição
    liberar_pilha(&carreg->municao);
    
    // Libera a estrutura do carregador
    free(carreg);
}

void destruir_disparador(Disparador *d)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    if (!disp) return;
    
    // Libera as pilhas dos carregadores
    if (disp->carregadorEsq) {
        liberar_pilha(&disp->carregadorEsq->municao);
        free(disp->carregadorEsq);
    }
    
    if (disp->carregadorDir) {
        liberar_pilha(&disp->carregadorDir->municao);
        free(disp->carregadorDir);
    }
    
    // Libera a pilha de disparo
    liberar_pilha(&disp->disp);
    
    // Libera a estrutura do disparador
    free(disp);
}

void disparador_set_carregador_dir(Disparador *d, Carregador *c)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    
    if (!disp) return;
    
    // Se já existe um carregador direito, libera ele primeiro
    if (disp->carregadorDir) {
        liberar_pilha(&disp->carregadorDir->municao);
        free(disp->carregadorDir);
    }
    
    // Define o novo carregador direito
    disp->carregadorDir = carreg;
}

void disparador_set_carregador_esq(Disparador *d, Carregador *c)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    
    if (!disp) return;
    
    // Se já existe um carregador esquerdo, libera ele primeiro
    if (disp->carregadorEsq) {
        liberar_pilha(&disp->carregadorEsq->municao);
        free(disp->carregadorEsq);
    }
    
    // Define o novo carregador esquerdo
    disp->carregadorEsq = carreg;
}

int carregador_get_id(Carregador *c)
{
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    if (!carreg) return -1;
    return carreg->id;
}

int carregador_vazio(Carregador *c)
{
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    if (!carreg) return 1;
    return vazia(&carreg->municao);
}


void carregador_destruir(Carregador *c)
{
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    if (!carreg) return;
    
    // Libera a pilha de munição
    liberar_pilha(&carreg->municao);
    
    // Libera a estrutura do carregador
    free(carreg);
}

void *disparador_obter_forma_disparo(Disparador *d)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    if (!disp) return NULL;
    
    // Verifica se há algo na pilha de disparo
    if (vazia(&disp->disp)) return NULL;
    
    // Obtém o item do topo sem removê-lo
    void *forma = NULL;
    // Como não temos uma função peek, vamos fazer um pop temporário e push de volta
    if (pop(&disp->disp, &forma)) {
        push(&disp->disp, forma); // Coloca de volta
        return forma;
    }
    
    return NULL;
}