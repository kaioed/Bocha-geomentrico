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
float comprimento_linha(Linha* l) {
    LinhaStruct* linha = (LinhaStruct*)l;
    float dx = linha->x2 - linha->x1;
    float dy = linha->y2 - linha->y1;
    return sqrtf(dx*dx + dy*dy);
}

// Calcula e retorna a "área" da linha: 2.0 * comprimento da linha
float area_linha(Linha* l) {
    return 2.0f * comprimento_linha(l);
}

// Libera a memória alocada para a linha
void liberar_linha(Linha* l) {
    free(l);
}
