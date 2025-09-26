#include "linha.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Estrutura interna da linha
typedef struct {
    int id;
    float x1;
    float y1;
    float x2;
    float y2;
    char cor[30];
} LinhaStruct;

// Cria uma nova linha entre os pontos (x1, y1) e (x2, y2), com cor e id
Linha* criar_linha(float x1, float y1, float x2, float y2, char* cor, int id) {
    LinhaStruct* l = (LinhaStruct*) malloc(sizeof(LinhaStruct));
    if (!l) return NULL;
    l->id = id;
    l->x1 = x1;
    l->y1 = y1;
    l->x2 = x2;
    l->y2 = y2;
    strncpy(l->cor, cor, sizeof(l->cor)-1);
    l->cor[sizeof(l->cor)-1] = '\0';
    return (Linha*)l;
}

// Calcula e retorna o comprimento da linha
float comprimento_linha(const Linha* l) {
    const LinhaStruct* linha = (const LinhaStruct*)l;
    float dx = linha->x2 - linha->x1;
    float dy = linha->y2 - linha->y1;
    return sqrtf(dx*dx + dy*dy);
}

// Calcula e retorna a "área" da linha: 2.0 * comprimento da linha
float area_linha(const Linha* l) {
    return 2.0f * comprimento_linha(l);
}

// Libera a memória alocada para a linha
void liberar_linha(Linha* l) {
    free(l);
}

// Retorna a coordenada x do ponto inicial da linha
float get_x1_linha(const Linha* l) {
    const LinhaStruct* linha = (const LinhaStruct*)l;
    return linha->x1;
}

// Retorna a coordenada y do ponto inicial da linha
float get_y1_linha(const Linha* l) {
    const LinhaStruct* linha = (const LinhaStruct*)l;
    return linha->y1;
}

// Retorna a coordenada x do ponto final da linha
float get_x2_linha(const Linha* l) {
    const LinhaStruct* linha = (const LinhaStruct*)l;
    return linha->x2;
}

// Retorna a coordenada y do ponto final da linha
float get_y2_linha(const Linha* l) {
    const LinhaStruct* linha = (const LinhaStruct*)l;
    return linha->y2;
}

// Retorna o id da linha
int get_id_linha(const Linha* l) {
    const LinhaStruct* linha = (const LinhaStruct*)l;
    return linha->id;
}

// Retorna a cor da linha
const char* get_cor_linha(const Linha* l) {
    const LinhaStruct* linha = (const LinhaStruct*)l;
    return linha->cor;
}
