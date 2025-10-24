#include <stdlib.h>
#include <string.h>
#include "texto.h"

// Estrutura interna do texto
typedef struct {
    int id;
    char * tipo;
    float x;
    float y;
    char corPreenchimento[30]; // Modificado
    char corBorda[30];       // Adicionado
    char anchor;             // Adicionado
    char conteudo[256];
    char fonte[64];
} TextoStruct;

// Cria um novo texto na posição (x, y), com cor, conteúdo e id
Texto* criar_texto(float x, float y, char* corBorda, char* corPreenchimento, char anchor, char* conteudo, const char* fonte, int id) {
    TextoStruct* t = (TextoStruct*) malloc(sizeof(TextoStruct));
    if (!t) return NULL;
    t->tipo = "Texto";
    t->id = id;
    t->x = x;
    t->y = y;
    t->anchor = anchor; // Salva a âncora

    // Salva as cores
    strncpy(t->corPreenchimento, corPreenchimento, sizeof(t->corPreenchimento)-1);
    t->corPreenchimento[sizeof(t->corPreenchimento)-1] = '\0';
    strncpy(t->corBorda, corBorda, sizeof(t->corBorda)-1);
    t->corBorda[sizeof(t->corBorda)-1] = '\0';

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

// Retorna a cor de PREENCHIMENTO do texto
const char* get_corPreenchimento_texto(const Texto* t) {
    if (!t) return NULL;
    const TextoStruct* txt = (const TextoStruct*)t;
    return txt->corPreenchimento;
}

// Retorna a cor da BORDA do texto
const char* get_corBorda_texto(const Texto* t) {
    if (!t) return NULL;
    const TextoStruct* txt = (const TextoStruct*)t;
    return txt->corBorda;
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

// Retorna a âncora do texto
char get_anchor_texto(const Texto* t) {
  if (!t) return '\0';
  const TextoStruct* txt = (const TextoStruct*)t;
  return txt->anchor;
}