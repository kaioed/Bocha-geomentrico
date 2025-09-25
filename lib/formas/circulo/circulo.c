#include "circulo.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float x;
    float y;
    float r;
    char corPreenchimento[32];
    char corBorda[32];
} CirculoStruct;

// Cria um novo círculo com centro (x, y), raio r e cores especificadas
Circulo* criar_circulo(float x, float y, float r, char* corPreenchimento, char* corBorda) {
    CirculoStruct* c = (CirculoStruct*) malloc(sizeof(CirculoStruct));
    if (!c) return NULL;
    c->x = x;
    c->y = y;
    c->r = r;
    strncpy(c->corPreenchimento, corPreenchimento, sizeof(c->corPreenchimento)-1);
    c->corPreenchimento[sizeof(c->corPreenchimento)-1] = '\0';
    strncpy(c->corBorda, corBorda, sizeof(c->corBorda)-1);
    c->corBorda[sizeof(c->corBorda)-1] = '\0';
    return (Circulo*)c;
}

// Calcula e retorna a área do círculo
float area_circulo(Circulo* c) {
    CirculoStruct* circ = (CirculoStruct*)c;
    return (float)(M_PI * circ->r * circ->r);
}

// Libera a memória alocada para o círculo
void liberar_circulo(Circulo* c) {
    free(c);
}
