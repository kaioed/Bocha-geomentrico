#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // Incluído para bool
#include "disparador.h"
#include "../pilha/pilha.h"
#include "../manipilarArq/arquivo.h"
#include "../fila/fila.h"
#include "../qry/qry.h" // Inclui forma_get_tipo, forma_get_id_original, forma_set_destruida

// Estruturas internas (permanecem iguais)
typedef struct CarregadorInterno
{
    int id;
    Pilha municao; // Agora é void*
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
    if (!c)
        return NULL;
    c->id = id;
    inicializar((Pilha *)&c->municao);
    if (!c->municao)
    {
        free(c);
        return NULL;
    }
    return (Carregador *)c;
}

Disparador *criar_disparador(int id, int x, int y)
{
    DisparadorInterno *d = (DisparadorInterno *)malloc(sizeof(DisparadorInterno));
    if (!d)
        return NULL;
    d->id = id;
    d->x = (float)x;
    d->y = (float)y;
    d->area_esmagada = 0.0f;
    d->carregadorEsq = NULL;
    d->carregadorDir = NULL;
    inicializar((Pilha *)&d->disp);
    if (!d->disp)
    {
        free(d);
        return NULL;
    }
    return (Disparador *)d;
}

void carregar_carregador(Fila fonte_ground, Carregador *c, int n, FILE *log)
{
    CarregadorInterno *carreg = (CarregadorInterno *)(*c);
   
    if (!carreg || !carreg->municao || !fonte_ground)
        return;

    if (log)
        fprintf(log, "[lc] Carregando %d formas no Carregador ID %d\n", n, carreg->id);

    for (int i = 0; i < n; i++)
    {
        Forma forma_wrapper_ptr = NULL;

        if (remover_da_fila(fonte_ground, &forma_wrapper_ptr))
        {
            if (forma_wrapper_ptr)
            {

                push(carreg->municao, forma_wrapper_ptr);

                forma_set_destruida(forma_wrapper_ptr, true);

                if (log)
                {
                    const char *tipo_str = "Desconhecido";
                    switch (forma_get_tipo(forma_wrapper_ptr))
                    {
                    case TIPO_CIRCULO:
                        tipo_str = "Circulo";
                        break;
                    case TIPO_RETANGULO:
                        tipo_str = "Retangulo";
                        break;
                    case TIPO_LINHA:
                        tipo_str = "Linha";
                        break;
                    case TIPO_TEXTO:
                        tipo_str = "Texto";
                        break;
                    }
                    fprintf(log, "\t- Carregado: Forma ID %d (Tipo: %s)\n", forma_get_id_original(forma_wrapper_ptr), tipo_str);
                }
            }
        }
        else
        {
            if (log)
                fprintf(log, "\t- Fila de formas do Ground esgotada.\n");
            break; // Sai do loop se a fila estiver vazia
        }
    }
    if (log)
        fprintf(log, "\n");
}

float disparador_get_area_esmagada(Disparador d) 
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    return disp ? disp->area_esmagada : 0.0f;
}

// Função carregar_disparador (lógica P1 adaptada)
void carregar_disparador(Disparador *d_ptr, int n, char *comando) // Recebe Disparador* (void**)
{
    if (!d_ptr || !*d_ptr)
    {
        fprintf(stderr, "Erro: Tentativa de carregar disparador NULL.\n");
        return;
    }
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);

    char lado = comando[0];
    if (lado != 'e' && lado != 'd')
        return;

    int i = 0;
    while (i < n)
    {
        Forma forma_topo_disp = NULL;   // void*
        Forma forma_topo_loader = NULL; // void*
        bool movido_do_loader_para_disp = false;
        bool movido_do_disp_para_loader_oposto = false;

        // Identifica carregadores de origem e destino
        CarregadorInterno *loader_origem = (lado == 'e') ? disp->carregadorEsq : disp->carregadorDir;
        CarregadorInterno *loader_destino = (lado == 'e') ? disp->carregadorDir : disp->carregadorEsq;

        
        if (vazia((Pilha *)&disp->disp))
        {
            if (loader_origem && pop((Pilha *)&loader_origem->municao, &forma_topo_loader))
            {
                push(disp->disp, forma_topo_loader);
                movido_do_loader_para_disp = true;
            }
            else
            {
                break; // Origem vazia ou inexistente
            }
        }
        // Se o disparador NÃO está vazio
        else
        {
          
            if (pop((Pilha *)&disp->disp, &forma_topo_disp))
            {
                if (loader_destino)
                {
                    push(loader_destino->municao, forma_topo_disp);
                    movido_do_disp_para_loader_oposto = true;
                }
                else
                {
                    
                }

                
                if (loader_origem && pop((Pilha *)&loader_origem->municao, &forma_topo_loader))
                {
                    push(disp->disp, forma_topo_loader);
                    movido_do_loader_para_disp = true;
                } // Se não conseguir puxar, o disparador fica vazio, o que está correto.
            }
            else
            {
                break; // Falha ao remover do disparador (improvável)
            }
        }

        // Incrementa o contador do loop se alguma movimentação útil ocorreu
        if (movido_do_loader_para_disp || movido_do_disp_para_loader_oposto)
        {
            i++;
        }
        else
        {
            break; // Nenhuma movimentação (loaders vazios/inexistentes)
        }
    }
}

float disparador_get_x(Disparador *d_ptr) // Recebe Disparador* (void**)
{
    if (!d_ptr || !*d_ptr)
        return 0.0f;
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);
    return disp->x;
}

float disparador_get_y(Disparador *d_ptr) // Recebe Disparador* (void**)
{
    if (!d_ptr || !*d_ptr)
        return 0.0f;
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);
    return disp->y;
}

int disparador_get_id(Disparador *d_ptr) // Recebe Disparador* (void**)
{
    if (!d_ptr || !*d_ptr)
        return -1;
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);
    return disp->id;
}

void destruir_carregador(Carregador c) // Recebe Carregador (void*)
{
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    if (!carreg)
        return;
    // Libera a estrutura da pilha (sem liberar os dados FormaStruct*)
    if (carreg->municao)
        liberar_pilha((Pilha *)&carreg->municao);
    free(carreg);
}

void destruir_disparador(Disparador *d_ptr) // Recebe Disparador* (void**)
{
    if (!d_ptr || !*d_ptr) return; // Verifica container e ponteiro do objeto
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr); // Obtém o objeto

    Carregador carregador_esquerdo_a_liberar = NULL;
    Carregador carregador_direito_a_liberar = NULL;

    // Guarda os ponteiros dos carregadores antes de anular
    if (disp->carregadorEsq != NULL) {
        carregador_esquerdo_a_liberar = (Carregador)disp->carregadorEsq;
    }
    if (disp->carregadorDir != NULL) {
        // Só guarda o direito se for DIFERENTE do esquerdo
        if (disp->carregadorDir != disp->carregadorEsq) {
            carregador_direito_a_liberar = (Carregador)disp->carregadorDir;
        }

    }

    disp->carregadorEsq = NULL;
    disp->carregadorDir = NULL;


    
    if (carregador_esquerdo_a_liberar != NULL) {
        destruir_carregador(carregador_esquerdo_a_liberar); 
    }
    if (carregador_direito_a_liberar != NULL) {
       
        destruir_carregador(carregador_direito_a_liberar); 
    }


    
    if (disp->disp) liberar_pilha((Pilha*)&disp->disp); // Passa Pilha* (void**)

    // Libera a memória do próprio DisparadorInterno
    free(disp);

    
    *d_ptr = NULL;
}

void disparador_set_carregador_dir(Disparador *d_ptr, Carregador *c_ptr) // Recebe Disparador*, Carregador*
{
    if (!d_ptr || !*d_ptr || !c_ptr)
        return; // Adicionado verificação para c_ptr
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);
    disp->carregadorDir = (CarregadorInterno *)(*c_ptr);
}

void disparador_set_carregador_esq(Disparador *d_ptr, Carregador *c_ptr) // Recebe Disparador*, Carregador*
{
    if (!d_ptr || !*d_ptr || !c_ptr)
        return; // Adicionado verificação para c_ptr
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);
    disp->carregadorEsq = (CarregadorInterno *)(*c_ptr);
}

int carregador_get_id(Carregador *c_ptr) // Recebe Carregador* (void**)
{
    if (!c_ptr || !*c_ptr)
        return -1;
    CarregadorInterno *carreg = (CarregadorInterno *)(*c_ptr);
    return carreg->id;
}

int carregador_vazio(Carregador *c_ptr) // Recebe Carregador* (void**)
{
    if (!c_ptr || !*c_ptr)
        return 1;
    CarregadorInterno *carreg = (CarregadorInterno *)(*c_ptr);
    if (!carreg->municao)
        return 1;
    return vazia((Pilha *)&carreg->municao);
}

Forma disparador_disparar_forma(Disparador *d_ptr)
{
    if (!d_ptr || !*d_ptr)
        return NULL;
    DisparadorInterno *disp = (DisparadorInterno *)(*d_ptr);
    if (!disp->disp || vazia((Pilha *)&disp->disp))
        return NULL;

    Forma forma_disparada = NULL; // Usa typedef Forma (void*)
    if (pop((Pilha *)&disp->disp, &forma_disparada))
    {
        return forma_disparada;
    }
    return NULL;
}

// Funções de área esmagada (não usadas pela lógica P1, mas mantidas)
void disparador_adicionar_area_esmagada(Disparador d, float area)
{ // Recebe Disparador (void*)
    if (!d)
        return;
    ((DisparadorInterno *)d)->area_esmagada += area;
}