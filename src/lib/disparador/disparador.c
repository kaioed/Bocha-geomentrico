#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "disparador.h"
#include "../pilha/pilha.h"
#include "../manipilarArq/arquivo.h"
#include "../fila/fila.h"
#include "../qry/qry.h"

typedef struct CarregadorInterno
{
    int id;
    Pilha municao;
} CarregadorInterno;

typedef struct DisparadorInterno
{
    int id;
    Pilha disp;
    CarregadorInterno *carregadorEsq;
    CarregadorInterno *carregadorDir;
    float x;
    float y;
    float area_esmagada;
} DisparadorInterno;

Carregador *criar_carredor(int id)
{
    CarregadorInterno *c = (CarregadorInterno *)malloc(sizeof(CarregadorInterno));
    if (!c) return NULL;
    c->id = id;
    inicializar((Pilha *)&c->municao);
    if (!c->municao) {
        free(c);
        return NULL;
    }
    return (Carregador *)c;
}

Disparador *criar_disparador(int id, int x, int y)
{
    DisparadorInterno *d = (DisparadorInterno *)malloc(sizeof(DisparadorInterno));
    if (!d) return NULL;
    d->id = id;
    d->x = (float)x;
    d->y = (float)y;
    d->area_esmagada = 0.0f;
    d->carregadorEsq = NULL;
    d->carregadorDir = NULL;
    inicializar((Pilha *)&d->disp);
    if (!d->disp) {
        free(d);
        return NULL;
    }
    return (Disparador *)d;
}

void carregar_carregador(Fila fonte_ground, Carregador *c, int n, FILE *log)
{
    CarregadorInterno *carreg = (CarregadorInterno *)(*c);
    if (!carreg || !carreg->municao || !fonte_ground) return;

    if (log)
        fprintf(log, "[lc] Carregando %d formas no Carregador ID %d\n", n, carreg->id);

    void **buffer_temp = (void**)malloc(n * sizeof(void*));
    int count = 0;

    for (int i = 0; i < n; i++)
    {
        void* forma_ptr = NULL;
        if (remover_da_fila(fonte_ground, &forma_ptr)) {
            if (forma_ptr) {
                buffer_temp[count++] = forma_ptr;
                
                if (log) {
                    fprintf(log, "\t- Carregado e removido do Ground: Forma ID %d\n", forma_get_id_original(forma_ptr));
                }
            }
        } else {
            break;
        }
    }

    for (int i = count - 1; i >= 0; i--) {
        push(carreg->municao, buffer_temp[i]);
    }

    free(buffer_temp);
    if (log) fprintf(log, "\n");
}

float disparador_get_area_esmagada(Disparador d) 
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    return disp ? disp->area_esmagada : 0.0f;
}

void carregar_disparador(Disparador *d_ptr, int n, char *comando) 
{
    if (!d_ptr || !*d_ptr) return;
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);

    char lado = comando[0]; 
    CarregadorInterno *prioritario = (lado == 'e') ? disp->carregadorDir : disp->carregadorEsq;
    CarregadorInterno *secundario  = (lado == 'e') ? disp->carregadorEsq : disp->carregadorDir;

    for (int i = 0; i < n; i++)
    {
        Forma forma = NULL;
        
        if (prioritario && pop((Pilha *)&prioritario->municao, &forma)) {
            push(disp->disp, forma);
        }
        else if (secundario && pop((Pilha *)&secundario->municao, &forma)) {
            push(disp->disp, forma);
        }
        else {
            break; 
        }
    }
}

float disparador_get_x(Disparador *d_ptr) 
{
    if (!d_ptr || !*d_ptr) return 0.0f;
    return ((DisparadorInterno *)(*d_ptr))->x;
}

float disparador_get_y(Disparador *d_ptr) 
{
    if (!d_ptr || !*d_ptr) return 0.0f;
    return ((DisparadorInterno *)(*d_ptr))->y;
}

int disparador_get_id(Disparador *d_ptr) 
{
    if (!d_ptr || !*d_ptr) return -1;
    return ((DisparadorInterno *)(*d_ptr))->id;
}

void destruir_carregador(Carregador c) 
{
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    if (!carreg) return;
    if (carreg->municao) liberar_pilha((Pilha *)&carreg->municao);
    free(carreg);
}

void destruir_disparador(Disparador *d_ptr) 
{
    if (!d_ptr || !*d_ptr) return;
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);

    if (disp->carregadorEsq) destruir_carregador(disp->carregadorEsq);
    if (disp->carregadorDir && disp->carregadorDir != disp->carregadorEsq) {
        destruir_carregador(disp->carregadorDir);
    }
    if (disp->disp) liberar_pilha((Pilha*)&disp->disp);
    free(disp);
    *d_ptr = NULL;
}

void disparador_set_carregador_dir(Disparador *d_ptr, Carregador *c_ptr) 
{
    if (!d_ptr || !*d_ptr || !c_ptr) return;
    ((DisparadorInterno *)(*d_ptr))->carregadorDir = (CarregadorInterno *)(*c_ptr);
}

void disparador_set_carregador_esq(Disparador *d_ptr, Carregador *c_ptr) 
{
    if (!d_ptr || !*d_ptr || !c_ptr) return;
    ((DisparadorInterno *)(*d_ptr))->carregadorEsq = (CarregadorInterno *)(*c_ptr);
}

int carregador_get_id(Carregador *c_ptr) 
{
    if (!c_ptr || !*c_ptr) return -1;
    return ((CarregadorInterno *)(*c_ptr))->id;
}

int carregador_vazio(Carregador *c_ptr) 
{
    if (!c_ptr || !*c_ptr) return 1;
    CarregadorInterno *c = (CarregadorInterno *)(*c_ptr);
    return vazia((Pilha *)&c->municao);
}

Forma disparador_disparar_forma(Disparador *d_ptr)
{
    if (!d_ptr || !*d_ptr) return NULL;
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);
    
    Forma forma_disparada = NULL;
    if (pop((Pilha *)&disp->disp, &forma_disparada)) {
        return forma_disparada;
    }
    return NULL;
}

void disparador_adicionar_area_esmagada(Disparador d, float area)
{
    if (!d) return;
    ((DisparadorInterno *)d)->area_esmagada += area;
}