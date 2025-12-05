#include "arena.h"
#include <stdlib.h>

typedef struct {
    Fila formas;
} ArenaStruct;

Arena arena_criar() {
    ArenaStruct* a = malloc(sizeof(ArenaStruct));
    if (a) {
        a->formas = iniciar_fila();
    }
    return (Arena)a;
}

void arena_destruir(Arena a) {
    if (a) {
        ArenaStruct* ar = (ArenaStruct*)a;
        destruir_fila(ar->formas);
        free(ar);
    }
}

void arena_adicionar_elemento(Arena a, Elemento e) {
    if (a && e) {
        adicionar_na_fila(((ArenaStruct*)a)->formas, e);
    }
}

Fila arena_get_fila(Arena a) {
    return a ? ((ArenaStruct*)a)->formas : NULL;
}

void arena_limpar(Arena a) {
    if (a) {
        ArenaStruct* ar = (ArenaStruct*)a;
        destruir_fila(ar->formas);
        ar->formas = iniciar_fila();
    }
}