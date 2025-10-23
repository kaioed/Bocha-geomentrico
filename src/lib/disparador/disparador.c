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
    
    d->carregadorEsq = NULL;
    d->carregadorDir = NULL;
    

    inicializar(&d->disp);
    
    return (Disparador *)d;
}

void carregar_carregador(Fila fonte_ground, Carregador *c, int n, FILE *log)
{
    CarregadorInterno *carreg = (CarregadorInterno *)c;
    if (!carreg || !fonte_ground) return;

    fprintf(log, "[lc] Carregando %d formas no Carregador ID %d\n", n, carreg->id);

    for (int i = 0; i < n; i++) {
        void *forma_wrapper_ptr = NULL;

        // Tira uma forma do início da fila do ground
        if (remover_da_fila(fonte_ground, &forma_wrapper_ptr)) {
            if (forma_wrapper_ptr) {
                FormaStruct* forma = (FormaStruct*) forma_wrapper_ptr;

                // Coloca a forma na "munição" (pilha) do carregador
                push(carreg->municao, forma);

                // DEVOLVE a forma para o final da fila do ground, mantendo o ciclo
                adicionar_na_fila(fonte_ground, forma);

                // Log da operação
                if (log) {
                    const char* tipo_str = "Desconhecido";
                    switch(forma->tipo) {
                        case TIPO_CIRCULO:   tipo_str = "Circulo";   break;
                        case TIPO_RETANGULO: tipo_str = "Retangulo"; break;
                        case TIPO_LINHA:     tipo_str = "Linha";     break;
                        case TIPO_TEXTO:     tipo_str = "Texto";     break;
                    }
                    fprintf(log, "\t- Carregado: Forma ID %d (Tipo: %s)\n", forma->id_original, tipo_str);
                }
            }
        } else {
            // A fila do ground está vazia, não há mais nada a carregar.
            fprintf(log, "\t- Fila de formas do Ground esgotada.\n");
            break;
        }
    }
    fprintf(log, "\n");
}

void carregar_disparador(Disparador *d, int n, char *comando)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    // Adiciona verificação inicial para o ponteiro do disparador
    if (!disp) {
        fprintf(stderr, "Erro: Tentativa de carregar disparador NULL.\n");
        return;
    }

    int i = 0;
    while (i < n)
    {
        // Caso 1: Pilha de disparo está vazia -> Carregar da munição
        if (vazia(&disp->disp))
        {
            void *linha = NULL;
            int pop_sucesso = 0;

            if (strcmp(comando, "e") == 0)
            {
                // Verifica se carregador esquerdo existe E se o pop tem sucesso
                if (disp->carregadorEsq && pop(&disp->carregadorEsq->municao, &linha))
                {
                   pop_sucesso = 1;
                } else {
                    // fprintf(stderr, "Debug: Carregador esquerdo vazio ou inexistente.\n"); // Debug opcional
                    break; // Para o loop se não puder carregar
                }
            }
            else if (strcmp(comando, "d") == 0)
            {
                // Verifica se carregador direito existe E se o pop tem sucesso
                if (disp->carregadorDir && pop(&disp->carregadorDir->municao, &linha))
                {
                    pop_sucesso = 1;
                } else {
                    // fprintf(stderr, "Debug: Carregador direito vazio ou inexistente.\n"); // Debug opcional
                    break; // Para o loop se não puder carregar
                }
            }

            // Só faz push se o pop foi bem-sucedido e obteve um ponteiro não-NULL
            if (pop_sucesso && linha != NULL) {
                 push(disp->disp, linha);
                 i++; // Incrementa SÓ se carregou com sucesso
            } else {
                 // Se pop falhou ou retornou NULL, não há mais nada a fazer neste lado
                 break;
            }
        }
        // Caso 2: Pilha de disparo NÃO está vazia -> Ciclar entre carregadores
        else
        {
            void *linha_disp = NULL;
            void *linha_loader = NULL;
            int pop_disp_ok = 0;
            int pop_loader_ok = 0;
            bool operacao_realizada = false; // Flag para saber se algo foi movido

            if (strcmp(comando, "e") == 0)
            {
                // Verifica se carregador esquerdo existe E NÃO está vazio
                if (disp->carregadorEsq && !vazia(&disp->carregadorEsq->municao))
                {
                    pop_disp_ok = pop(&disp->disp, &linha_disp);
                    // Só faz push para o direito se o pop teve sucesso E o carregador direito existe
                    if(pop_disp_ok && disp->carregadorDir) {
                         push(disp->carregadorDir->municao, linha_disp);
                         operacao_realizada = true; // Algo foi movido
                    }
                    pop_loader_ok = pop(&disp->carregadorEsq->municao, &linha_loader);
                    // Só faz push para disp se o pop do carregador teve sucesso
                    if(pop_loader_ok) {
                         push(disp->disp, linha_loader);
                         operacao_realizada = true; // Algo foi movido
                    }
                } else {
                     // fprintf(stderr, "Debug: Carregador esquerdo vazio ou inexistente para ciclo.\n"); // Debug opcional
                     break; // Para o loop se não puder ciclar
                }
            }
            else if (strcmp(comando, "d") == 0)
            {
                // Verifica se carregador direito existe E NÃO está vazio
                if (disp->carregadorDir && !vazia(&disp->carregadorDir->municao))
                {
                    pop_disp_ok = pop(&disp->disp, &linha_disp);
                    // Só faz push para o esquerdo se o pop teve sucesso E o carregador esquerdo existe
                     if(pop_disp_ok && disp->carregadorEsq) {
                         push(disp->carregadorEsq->municao, linha_disp);
                         operacao_realizada = true;
                     }
                    pop_loader_ok = pop(&disp->carregadorDir->municao, &linha_loader);
                    // Só faz push para disp se o pop do carregador teve sucesso
                     if(pop_loader_ok) {
                         push(disp->disp, linha_loader);
                         operacao_realizada = true;
                     }
                } else {
                     // fprintf(stderr, "Debug: Carregador direito vazio ou inexistente para ciclo.\n"); // Debug opcional
                     break; // Para o loop se não puder ciclar
                }
            }

            // Incrementa o contador SÓ se alguma operação de pop/push foi realmente realizada
            if (operacao_realizada) {
                 i++;
            } else {
                 // Se nenhuma operação foi feita (ex: pop falhou), paramos para evitar loop infinito
                 break;
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

void destruir_carregador(Carregador c)
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

  
    if (disp->carregadorEsq && disp->carregadorEsq == disp->carregadorDir) {
        destruir_carregador(disp->carregadorEsq);
        disp->carregadorDir = NULL; 
    }

   
    if (disp->carregadorEsq) {
        destruir_carregador(disp->carregadorEsq);
    }
    
   
    if (disp->carregadorDir) {
        destruir_carregador(disp->carregadorDir);
    }
    
    liberar_pilha(&disp->disp);
    free(disp);
}

void disparador_set_carregador_dir(Disparador *d, Carregador *c)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    if (!disp) return;
    
   
    disp->carregadorDir = (CarregadorInterno *)c;
}

void disparador_set_carregador_esq(Disparador *d, Carregador *c)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    if (!disp) return;
    
   
    disp->carregadorEsq = (CarregadorInterno *)c;

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


void* disparador_disparar_forma(Disparador *d)
{
    DisparadorInterno *disp = (DisparadorInterno *)d;
    if (!disp || vazia(&disp->disp)) return NULL;
    
    void *forma_disparada = NULL;
    // Remove (pop) a forma da pilha de disparo
    if (pop(&disp->disp, &forma_disparada)) {
        return forma_disparada;
    }
    
    return NULL;
}