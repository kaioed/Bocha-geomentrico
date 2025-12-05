#include "elemento.h"
#include <stdlib.h>
#include <string.h>
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"

typedef struct {
    int id_original;
    TipoForma tipo;
    void *dados_forma;
    bool foi_destruida;
    bool foi_clonada;
    float x_landed;
    float y_landed;
} ElementoStruct;

static int proximo_id_clone = 10000;

Elemento elemento_criar_wrapper(int id, TipoForma tipo, void* dados, float x, float y) {
    ElementoStruct* e = malloc(sizeof(ElementoStruct));
    if (!e) return NULL;
    e->id_original = id;
    e->tipo = tipo;
    e->dados_forma = dados;
    e->foi_destruida = false;
    e->foi_clonada = false;
    e->x_landed = x;
    e->y_landed = y;
    return (Elemento)e;
}

TipoForma elemento_get_tipo(Elemento e) {
    return ((ElementoStruct*)e)->tipo;
}

int elemento_get_id_original(Elemento e) {
    return ((ElementoStruct*)e)->id_original;
}

void* elemento_get_dados(Elemento e) {
    return ((ElementoStruct*)e)->dados_forma;
}

float elemento_get_x(Elemento e) {
    return ((ElementoStruct*)e)->x_landed;
}

float elemento_get_y(Elemento e) {
    return ((ElementoStruct*)e)->y_landed;
}

bool elemento_foi_destruido(Elemento e) {
    return ((ElementoStruct*)e)->foi_destruida;
}

void elemento_set_destruido(Elemento e, bool status) {
    ((ElementoStruct*)e)->foi_destruida = status;
}

float elemento_calcular_area(Elemento e) {
    ElementoStruct* el = (ElementoStruct*)e;
    if (!el || !el->dados_forma) return 0.0f;
    switch (el->tipo) {
        case TIPO_CIRCULO: return area_circulo(el->dados_forma);
        case TIPO_RETANGULO: return area_retangulo(el->dados_forma);
        case TIPO_LINHA: return area_linha(el->dados_forma);
        case TIPO_TEXTO: return area_texto(el->dados_forma);
        default: return 0.0f;
    }
}

Elemento elemento_clonar(Elemento original, float x, float y, const char* nova_cor_borda, bool trocar_cores) {
    ElementoStruct* orig = (ElementoStruct*)original;
    if (!orig || !orig->dados_forma) return NULL;

    int id_clone = proximo_id_clone++;
    void *dados_clonados = NULL;

    switch (orig->tipo) {
        case TIPO_CIRCULO: {
            Circulo *c = (Circulo *)orig->dados_forma;
            const char *cb = trocar_cores ? get_corPreenchimento_circulo(c) : (nova_cor_borda ? nova_cor_borda : get_corBorda_circulo(c));
            const char *cp = trocar_cores ? get_corBorda_circulo(c) : get_corPreenchimento_circulo(c);
            dados_clonados = criar_circulo(x, y, get_raio(c), (char*)cp, (char*)cb, id_clone);
            break;
        }
        case TIPO_RETANGULO: {
            Retangulo *r = (Retangulo *)orig->dados_forma;
            const char *cb = trocar_cores ? get_corPreenchimento_retangulo(r) : (nova_cor_borda ? nova_cor_borda : get_corBorda_retangulo(r));
            const char *cp = trocar_cores ? get_corBorda_retangulo(r) : get_corPreenchimento_retangulo(r);
            dados_clonados = criar_retangulo(x, y, get_largura(r), get_altura(r), (char*)cp, (char*)cb, id_clone);
            break;
        }
        case TIPO_LINHA: {
            Linha *l = (Linha *)orig->dados_forma;
            float dx = get_x2_linha(l) - get_x1_linha(l);
            float dy = get_y2_linha(l) - get_y1_linha(l);
            const char *cor = nova_cor_borda ? nova_cor_borda : get_cor_linha(l);
            dados_clonados = criar_linha(x, y, x+dx, y+dy, (char*)cor, is_dashed_linha(l), id_clone);
            break;
        }
        case TIPO_TEXTO: {
            Texto *t = (Texto *)orig->dados_forma;
            const char *cb = trocar_cores ? get_corPreenchimento_texto(t) : (nova_cor_borda ? nova_cor_borda : get_corBorda_texto(t));
            const char *cp = trocar_cores ? get_corBorda_texto(t) : get_corPreenchimento_texto(t);
            dados_clonados = criar_texto(x, y, (char*)cb, (char*)cp, get_anchor_texto(t), (char*)get_conteudo_texto(t), get_fonte_texto(t), id_clone);
            break;
        }
    }

    if (!dados_clonados) return NULL;

    ElementoStruct* clone = malloc(sizeof(ElementoStruct));
    clone->id_original = id_clone;
    clone->tipo = orig->tipo;
    clone->dados_forma = dados_clonados;
    clone->foi_clonada = true;
    clone->foi_destruida = false;
    clone->x_landed = x;
    clone->y_landed = y;
    return (Elemento)clone;
}