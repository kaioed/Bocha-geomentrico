#include <stdlib.h>
#include <string.h>
#include "texto.h"

// Estrutura interna do texto
typedef struct {
    int id;
    float x;
    float y;
    char cor[30];
    char conteudo[256];
    char fonte[64];
} TextoStruct;

// Cria um novo texto na posição (x, y), com cor, conteúdo e id
Texto* criar_texto(float x, float y, char* cor, char* conteudo, const char* fonte, int id) {
    TextoStruct* t = (TextoStruct*) malloc(sizeof(TextoStruct));
    if (!t) return NULL;
    t->id = id;
    t->x = x;
    t->y = y;
    strncpy(t->cor, cor, sizeof(t->cor)-1);
    t->cor[sizeof(t->cor)-1] = '\0';
    strncpy(t->conteudo, conteudo, sizeof(t->conteudo)-1);
    t->conteudo[sizeof(t->conteudo)-1] = '\0';
    if (fonte) {
        strncpy(t->fonte, fonte, sizeof(t->fonte)-1);
        t->fonte[sizeof(t->fonte)-1] = '\0';
    } else {
        t->fonte[0] = '\0';
    }
    return (Texto*)t;
}

// Calcula e retorna a "área" do texto: 20.0 * número de caracteres
float area_texto(const Texto* t) {
    const TextoStruct* txt = (const TextoStruct*)t;
    return 20.0f * (float)strlen(txt->conteudo);
}

// Libera a memória alocada para o texto
void liberar_texto(Texto* t) {
    free(t);
}

// Ajusta a fonte do texto em tempo de execução
void set_fonte_texto(Texto* t, const char* fonte) {
    if (!t || !fonte) return;
    TextoStruct* txt = (TextoStruct*)t;
    strncpy(txt->fonte, fonte, sizeof(txt->fonte)-1);
    txt->fonte[sizeof(txt->fonte)-1] = '\0';
}

// Retorna a fonte atual do texto
const char* get_fonte_texto(const Texto* t) {
    if (!t) return NULL;
    const TextoStruct* txt = (const TextoStruct*)t;
    return txt->fonte;
}

// Retorna o id do texto
int get_id_texto(const Texto* t) {
    if (!t) return -1;
    const TextoStruct* txt = (const TextoStruct*)t;
    return txt->id;
}

// Retorna a cor do texto
const char* get_cor_texto(const Texto* t) {
    if (!t) return NULL;
    const TextoStruct* txt = (const TextoStruct*)t;
    return txt->cor;
}

// Retorna o conteúdo do texto
const char* get_conteudo_texto(const Texto* t) {
    if (!t) return NULL;
    const TextoStruct* txt = (const TextoStruct*)t;
    return txt->conteudo;
}

// Retorna a coordenada x do texto
float get_x_texto(const Texto* t) {
    if (!t) return 0.0f;
    const TextoStruct* txt = (const TextoStruct*)t;
    return txt->x;
}

// Retorna a coordenada y do texto
float get_y_texto(const Texto* t) {
    if (!t) return 0.0f;
    const TextoStruct* txt = (const TextoStruct*)t;
    return txt->y;
}
