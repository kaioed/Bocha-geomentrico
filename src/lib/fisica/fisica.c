#include "fisica.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../elemento/elemento.h" 
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../elemento/elemento.h"
#include "../campo/arena.h"
#include "../fila/fila.h"

typedef struct { float minX, minY, maxX, maxY; } Aabb;


static char* duplicar_str(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* copia = malloc(len);
    if (copia) {
        strcpy(copia, s);
    }
    return copia;
}


static char* get_cor_complementar(const char* cor_hexa) {
    if (!cor_hexa) return duplicar_str("#FFFFFF");
    
    int has_hash = (cor_hexa[0] == '#');
    const char* hex = has_hash ? cor_hexa + 1 : cor_hexa;
    unsigned int r, g, b;
    
    if (strlen(hex) == 3) {
        unsigned int rh, gh, bh;
        sscanf(hex, "%1x%1x%1x", &rh, &gh, &bh);
        r = rh * 17; g = gh * 17; b = bh * 17;
    } else {
        sscanf(hex, "%2x%2x%2x", &r, &g, &b);
    }
    
    int cr = 255 - r; if(cr<0) cr=0; if(cr>255) cr=255;
    int cg = 255 - g; if(cg<0) cg=0; if(cg>255) cg=255;
    int cb = 255 - b; if(cb<0) cb=0; if(cb>255) cb=255;
    
    char* out = malloc(10);
    if (out) {
        sprintf(out, "#%02x%02x%02x", cr, cg, cb);
    }
    return out;
}

static Aabb make_aabb(Elemento e) {
    Aabb box = {0, 0, 0, 0};
    float x = elemento_get_x(e);
    float y = elemento_get_y(e);
    void* dados = elemento_get_dados(e);
    
    switch (elemento_get_tipo(e)) {
        case TIPO_CIRCULO: {
            float r = get_raio((Circulo*)dados);
            box.minX = x - r; box.maxX = x + r;
            box.minY = y - r; box.maxY = y + r;
            break;
        }
        case TIPO_RETANGULO: {
            float w = get_largura((Retangulo*)dados);
            float h = get_altura((Retangulo*)dados);
            box.minX = x; box.maxX = x + w;
            box.minY = y; box.maxY = y + h;
            break;
        }
        case TIPO_TEXTO: {
            const char* c = get_conteudo_texto((Texto*)dados);
            float l = c ? (float)strlen(c)*10.0f : 0.0f;
            char anc = get_anchor_texto((Texto*)dados);
            float x1=x, x2=x;
            if (anc=='i'||anc=='s') x2=x+l;
            else if (anc=='f'||anc=='e') x1=x-l;
            else { x1=x-l/2; x2=x+l/2; }
            box.minX = x1-1; box.maxX = x2+1;
            box.minY = y-1; box.maxY = y+1;
            break;
        }
        case TIPO_LINHA: {
            float x1 = get_x1_linha((Linha*)dados);
            float y1 = get_y1_linha((Linha*)dados);
            float x2 = get_x2_linha((Linha*)dados);
            float y2 = get_y2_linha((Linha*)dados);
            box.minX = (x1<x2?x1:x2)-1; box.maxX = (x1>x2?x1:x2)+1;
            box.minY = (y1<y2?y1:y2)-1; box.maxY = (y1>y2?y1:y2)+1;
            break;
        }
    }
    return box;
}

static bool overlap(Aabb a, Aabb b) {
    return !(a.maxX < b.minX || b.maxX < a.minX || a.maxY < b.minY || b.maxY < a.minY);
}

void fisica_processar_colisoes(Arena arena, Ground ground, FILE* txt, double* pontuacao, int* esmagadas, int* clonadas) {
    if (txt) fprintf(txt, "\n--- Inicio do Processamento de Colisoes (calc) ---\n");

    Fila f = arena_get_fila(arena);
    Fila fg = get_ground_fila(ground);
    void *I_void, *J_void;

    while (remover_da_fila(f, &I_void)) {
        Elemento I = (Elemento)I_void;
        elemento_set_destruido(I, true);

        if (!remover_da_fila(f, &J_void)) {
            Elemento Ipos = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), NULL, false);
            if (Ipos) adicionar_na_fila(fg, Ipos);
            continue;
        }
        Elemento J = (Elemento)J_void;
        elemento_set_destruido(J, true);

        Aabb aI = make_aabb(I);
        Aabb aJ = make_aabb(J);

        if (overlap(aI, aJ)) {
            float areaI = elemento_calcular_area(I);
            float areaJ = elemento_calcular_area(J);

            if (txt) fprintf(txt, "\tColisao: ID %d (Area %.2f) vs ID %d (Area %.2f)\n", elemento_get_id_original(I), areaI, elemento_get_id_original(J), areaJ);

            if (areaI < areaJ) {
                *pontuacao += areaI;
                if (txt) fprintf(txt, "\t-> ID %d sobrevive, ID %d destruido.\n", elemento_get_id_original(J), elemento_get_id_original(I));
                (*esmagadas)++;
                
                
                float x_esm = elemento_get_x(I);
                float y_esm = elemento_get_y(I);
                void* ast = criar_texto(x_esm, y_esm, "red", "black", 'm', "*", "sans-serif,bold,30px", -1);
                Elemento e_ast = elemento_criar_wrapper(-1, TIPO_TEXTO, ast, x_esm, y_esm);
                adicionar_na_fila(fg, e_ast);

                Elemento Jpos = elemento_clonar(J, elemento_get_x(J), elemento_get_y(J), NULL, false);
                if (Jpos) adicionar_na_fila(fg, Jpos);
            } else {
                *pontuacao += areaJ;
                if (txt) fprintf(txt, "\t-> ID %d sobrevive, ID %d destruido. Clones gerados.\n", elemento_get_id_original(I), elemento_get_id_original(J));
                (*esmagadas)++;
                
               
                if (elemento_get_tipo(I) == TIPO_LINHA) {
                    const char* corLinha = get_cor_linha((Linha*)elemento_get_dados(I));
                    char* comp = get_cor_complementar(corLinha);
                    
                    Elemento Jprime = elemento_clonar(J, elemento_get_x(J), elemento_get_y(J), comp, false);
                    
                    
                    Elemento Iclone = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), comp, false); 
                    
                   
                    Elemento Ipos = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), NULL, false);

                    if (Jprime) adicionar_na_fila(fg, Jprime);
                    if (Ipos) adicionar_na_fila(fg, Ipos);
                    if (Iclone) { adicionar_na_fila(fg, Iclone); (*clonadas)++; }
                    free(comp);
                } 
                else {
                   
                    const char* cor_i = NULL;
                    TipoForma t = elemento_get_tipo(I);
                    void* d = elemento_get_dados(I);
                    if(t==TIPO_CIRCULO) cor_i=get_corPreenchimento_circulo(d);
                    else if(t==TIPO_RETANGULO) cor_i=get_corPreenchimento_retangulo(d);
                    else if(t==TIPO_TEXTO) cor_i=get_corPreenchimento_texto(d);

                    Elemento Jprime = elemento_clonar(J, elemento_get_x(J), elemento_get_y(J), cor_i, false);
                    Elemento Ipos = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), NULL, false);
                    Elemento Iclone = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), NULL, true);
                    
                    if (Jprime) adicionar_na_fila(fg, Jprime);
                    if (Ipos) adicionar_na_fila(fg, Ipos);
                    if (Iclone) { adicionar_na_fila(fg, Iclone); (*clonadas)++; }
                }
            }
        } else {
            Elemento Ipos = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), NULL, false);
            Elemento Jpos = elemento_clonar(J, elemento_get_x(J), elemento_get_y(J), NULL, false);
            if (Ipos) adicionar_na_fila(fg, Ipos);
            if (Jpos) adicionar_na_fila(fg, Jpos);
        }
    }
    
    if (txt) {
        fprintf(txt, "\n[calc]\n\tResult: %.2f\n\n", *pontuacao);
    }
    arena_limpar(arena);
}