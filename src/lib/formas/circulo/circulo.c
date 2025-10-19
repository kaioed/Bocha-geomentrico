#include "circulo.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    int id;
    char * tipo;
    float x;
    float y;
    float r;
    char corPreenchimento[30];
    char corBorda[30];
} CirculoStruct;

// Cria um novo círculo com centro (x, y), raio r, cores especificadas e id
Circulo* criar_circulo(float x, float y, float r, char* corPreenchimento, char* corBorda, int id) {
    CirculoStruct* c = (CirculoStruct*) malloc(sizeof(CirculoStruct));
    if (!c) return NULL;
    c->tipo = "Circulo";
    c->id = id;
    c->x = x;
    c->y = y;
    c->r = r;

    if(corPreenchimento == NULL|| corBorda == NULL) {
        free(c);
        return NULL;
    }

    strncpy(c->corPreenchimento, corPreenchimento, sizeof(c->corPreenchimento)-1);
    c->corPreenchimento[sizeof(c->corPreenchimento)-1] = '\0';
    strncpy(c->corBorda, corBorda, sizeof(c->corBorda)-1);
    c->corBorda[sizeof(c->corBorda)-1] = '\0';
    return (Circulo*)c;
}

// Calcula e retorna a área do círculo
float area_circulo(const Circulo* c) {
    const CirculoStruct* circ = (const CirculoStruct*)c;
    return (float)(M_PI * circ->r * circ->r);
}

// Libera a memória alocada para o círculo
void liberar_circulo(Circulo* c) {
    free(c);
}

// Retorna o raio do círculo
float get_raio(const Circulo* c) {
    const CirculoStruct* circ = (const CirculoStruct*)c;
    return circ->r;
}

// Retorna a coordenada x do centro do círculo
float get_x(const Circulo* c) {
    const CirculoStruct* circ = (const CirculoStruct*)c;
    return circ->x;
}

// Retorna a coordenada y do centro do círculo
float get_y(const Circulo* c) {
    const CirculoStruct* circ = (const CirculoStruct*)c;
    return circ->y;
}

// Retorna o id do círculo
int get_id_circulo(const Circulo* c) {
    const CirculoStruct* circ = (const CirculoStruct*)c;
    return circ->id;
}

// Retorna a cor de preenchimento do círculo
const char* get_corPreenchimento_circulo(const Circulo* c) {
    const CirculoStruct* circ = (const CirculoStruct*)c;
    return circ->corPreenchimento;
}

// Retorna a cor da borda do círculo
const char* get_corBorda_circulo(const Circulo* c) {
    const CirculoStruct* circ = (const CirculoStruct*)c;
    return circ->corBorda;
}

void set_corPreenchimento_circulo(Circulo* c, const char* corPreenchimento) {
    CirculoStruct* circ = (CirculoStruct*)c;
    if (circ && corPreenchimento) {
        strncpy(circ->corPreenchimento, corPreenchimento, sizeof(circ->corPreenchimento)-1);
        circ->corPreenchimento[sizeof(circ->corPreenchimento)-1] = '\0';
    }
}

void set_corBorda_circulo(Circulo* c, const char* corBorda) {
    CirculoStruct* circ = (CirculoStruct*)c;
    if (circ && corBorda) {
        strncpy(circ->corBorda, corBorda, sizeof(circ->corBorda)-1);
        circ->corBorda[sizeof(circ->corBorda)-1] = '\0';
    }
}
