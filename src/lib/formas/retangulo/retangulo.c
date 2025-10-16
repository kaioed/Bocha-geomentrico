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
    if (corPreenchimento == NULL || corBorda == NULL) {
        free(r);
        return NULL;
    }
    strncpy(r->corPreenchimento, corPreenchimento, sizeof(r->corPreenchimento)-1);
    r->corPreenchimento[sizeof(r->corPreenchimento)-1] = '\0';
    strncpy(r->corBorda, corBorda, sizeof(r->corBorda)-1);
    r->corBorda[sizeof(r->corBorda)-1] = '\0';
    return (Retangulo*)r;
}

// Calcula e retorna a área do retângulo
float area_retangulo(const Retangulo* r) {
    RetanguloStruct* ret = (RetanguloStruct*)r;
    return ret->w * ret->h;
}

// Libera a memória alocada para o retângulo
void liberar_retangulo(Retangulo* r) {
    free(r);
}

// Retorna a largura do retângulo
float get_largura(const Retangulo* r) {
    const RetanguloStruct* ret = (const RetanguloStruct*)r;
    return ret->w;
}

// Retorna a altura do retângulo
float get_altura(const Retangulo* r) {
    const RetanguloStruct* ret = (const RetanguloStruct*)r;
    return ret->h;
}

// Retorna a coordenada x do retângulo
float get_x_retangulo(const Retangulo* r) {
    const RetanguloStruct* ret = (const RetanguloStruct*)r;
    return ret->x;
}

// Retorna a coordenada y do retângulo
float get_y_retangulo(const Retangulo* r) {
    const RetanguloStruct* ret = (const RetanguloStruct*)r;
    return ret->y;
}

// Retorna o id do retângulo
int get_id_retangulo(const Retangulo* r) {
    const RetanguloStruct* ret = (const RetanguloStruct*)r;
    return ret->id;
}

// Retorna a cor de preenchimento do retângulo
const char* get_corPreenchimento_retangulo(const Retangulo* r) {
    const RetanguloStruct* ret = (const RetanguloStruct*)r;
    return ret->corPreenchimento;
}

// Retorna a cor da borda do retângulo
const char* get_corBorda_retangulo(const Retangulo* r) {
    const RetanguloStruct* ret = (const RetanguloStruct*)r;
    return ret->corBorda;
}
