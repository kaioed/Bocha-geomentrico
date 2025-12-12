#ifndef DISPARADOR_H
#define DISPARADOR_H

#include <stdio.h>
#include "../pilha/pilha.h"
#include "../fila/fila.h"

typedef void *Disparador;
typedef void *Carregador;

Carregador *criar_carredor(int id);
Disparador *criar_disparador(int id, int x, int y);
void carregar_carregador(Fila fonte, Carregador *c, int n, FILE *log);
void carregar_disparador(Disparador *d, int n, char *comando);
float disparador_get_x(Disparador *d);
float disparador_get_y(Disparador *d);
int disparador_get_id(Disparador *d);
void destruir_disparador(Disparador *d);
void destruir_carregador(Carregador c);
void disparador_set_carregador_dir(Disparador *d, Carregador *c);
void disparador_set_carregador_esq(Disparador *d, Carregador *c);
int carregador_get_id(Carregador *c);
int carregador_vazio(Carregador *c);
void carregador_destruir(Carregador *c);
void *disparador_disparar_forma(Disparador *d);
void disparador_adicionar_area_esmagada(Disparador d, float area);
float disparador_get_area_esmagada(Disparador d);
Carregador disparador_get_carregador_esq(Disparador *d);
Carregador disparador_get_carregador_dir(Disparador *d);

#endif