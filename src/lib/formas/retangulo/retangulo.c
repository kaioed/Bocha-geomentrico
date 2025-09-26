#include "retangulo.h"
#include <stdlib.h>
#include <string.h>

// Estrutura interna do retângulo
typedef struct {
    int id;
    float x;
    float y;
    float w;
    float h;
    char corPreenchimento[30];
    char corBorda[30];
} RetanguloStruct;

// Cria um novo retângulo com origem (x, y), largura w, altura h, cores e id
Retangulo* criar_retangulo(float x, float y, float w, float h, char* corPreenchimento, char* corBorda, int id) {
    RetanguloStruct* r = (RetanguloStruct*) malloc(sizeof(RetanguloStruct));
    if (!r) return NULL;
    r->id = id;
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
    strncpy(r->corPreenchimento, corPreenchimento, sizeof(r->corPreenchimento)-1);
    r->corPreenchimento[sizeof(r->corPreenchimento)-1] = '\0';
    strncpy(r->corBorda, corBorda, sizeof(r->corBorda)-1);
    r->corBorda[sizeof(r->corBorda)-1] = '\0';
    return (Retangulo*)r;
}

// Calcula e retorna a área do retângulo
float area_retangulo(Retangulo* r) {
    RetanguloStruct* ret = (RetanguloStruct*)r;
    return ret->w * ret->h;
}

// Libera a memória alocada para o retângulo
void liberar_retangulo(Retangulo* r) {
    free(r);
}
