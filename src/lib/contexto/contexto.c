#include "contexto.h"
#include <stdlib.h>

typedef struct {
    Disparador **disparadores;
    int qtd_disp;
    Carregador **carregadores;
    int qtd_carr;
} ContextoStruct;

Contexto contexto_criar() {
    ContextoStruct* ctx = malloc(sizeof(ContextoStruct));
    if (ctx) {
        ctx->disparadores = NULL;
        ctx->qtd_disp = 0;
        ctx->carregadores = NULL;
        ctx->qtd_carr = 0;
    }
    return (Contexto)ctx;
}

void contexto_destruir(Contexto ctx) {
    if (!ctx) return;
    ContextoStruct* c = (ContextoStruct*)ctx;
    
    for (int i = 0; i < c->qtd_disp; i++) {
        if (c->disparadores[i]) {
            destruir_disparador(c->disparadores[i]);
            free(c->disparadores[i]);
        }
    }
    free(c->disparadores);

    for (int i = 0; i < c->qtd_carr; i++) {
        if (c->carregadores[i]) {
            destruir_carregador(*(c->carregadores[i])); 
            free(c->carregadores[i]);
        }
    }
    free(c->carregadores);
    free(c);
}

void contexto_adicionar_disparador(Contexto ctx, int id, float x, float y) {
    ContextoStruct* c = (ContextoStruct*)ctx;
    Disparador d = criar_disparador(id, (int)x, (int)y);
    c->disparadores = realloc(c->disparadores, (c->qtd_disp + 1) * sizeof(Disparador*));
    c->disparadores[c->qtd_disp] = malloc(sizeof(Disparador));
    *(c->disparadores[c->qtd_disp]) = d;
    c->qtd_disp++;
}

Disparador contexto_buscar_disparador(Contexto ctx, int id) {
    ContextoStruct* c = (ContextoStruct*)ctx;
    for (int i = 0; i < c->qtd_disp; i++) {
        if (c->disparadores[i] && disparador_get_id(c->disparadores[i]) == id) {
            return *(c->disparadores[i]);
        }
    }
    return NULL;
}

void contexto_adicionar_carregador(Contexto ctx, int id, Fila ground, int n, FILE* log) {
    ContextoStruct* c = (ContextoStruct*)ctx;
    Carregador carg = criar_carredor(id);
    carregar_carregador(ground, &carg, n, log);
    c->carregadores = realloc(c->carregadores, (c->qtd_carr + 1) * sizeof(Carregador*));
    c->carregadores[c->qtd_carr] = malloc(sizeof(Carregador));
    *(c->carregadores[c->qtd_carr]) = carg;
    c->qtd_carr++;
}

Carregador contexto_buscar_carregador(Contexto ctx, int id) {
    ContextoStruct* c = (ContextoStruct*)ctx;
    for (int i = 0; i < c->qtd_carr; i++) {
        if (c->carregadores[i] && carregador_get_id(c->carregadores[i]) == id) {
            return *(c->carregadores[i]);
        }
    }
    return NULL;
}

void contexto_vincular_equipamentos(Contexto ctx, int id_disp, int id_ce, int id_cd, FILE* log) {
    ContextoStruct* c = (ContextoStruct*)ctx;
    Disparador* d_ptr = NULL;
    
    for (int i=0; i < c->qtd_disp; i++) {
        if (disparador_get_id(c->disparadores[i]) == id_disp) {
            d_ptr = c->disparadores[i];
            break;
        }
    }

    if (d_ptr) {
        Carregador* ce_ptr = NULL;
        Carregador* cd_ptr = NULL;
        int idx_e = -1, idx_d = -1;

        for (int i=0; i < c->qtd_carr; i++) {
            if (carregador_get_id(c->carregadores[i]) == id_ce) {
                ce_ptr = c->carregadores[i];
                idx_e = i;
            }
            if (carregador_get_id(c->carregadores[i]) == id_cd) {
                cd_ptr = c->carregadores[i];
                idx_d = i;
            }
        }
        
        if (ce_ptr) {
            disparador_set_carregador_esq(d_ptr, ce_ptr);
            if (idx_e != -1) { free(c->carregadores[idx_e]); c->carregadores[idx_e] = NULL; }
        }
        if (cd_ptr) {
            disparador_set_carregador_dir(d_ptr, cd_ptr);
            if (idx_d != -1 && idx_d != idx_e) { free(c->carregadores[idx_d]); c->carregadores[idx_d] = NULL; }
        }
    } else if (log) {
        fprintf(log, "ATCH: Disparador ID %d nao encontrado.\n", id_disp);
    }
}