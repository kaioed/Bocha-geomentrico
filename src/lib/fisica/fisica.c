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
#include "../campo/arena.h"
#include "../fila/fila.h"
#include "../sobreposicao/sobreposicao.h"
#include "../geo/geo.h"

static const char* obter_cor_preenchimento(Elemento e) {
    void* dados = elemento_get_dados(e);
    switch(elemento_get_tipo(e)) {
        case TIPO_CIRCULO: return get_corPreenchimento_circulo((Circulo*)dados);
        case TIPO_RETANGULO: return get_corPreenchimento_retangulo((Retangulo*)dados);
        case TIPO_TEXTO: return get_corPreenchimento_texto((Texto*)dados);
        default: return NULL;
    }
}

void fisica_processar_colisoes(Arena arena, Ground ground, FILE* txt, double* pontuacao, int* esmagadas, int* clonadas) {
    if (txt) fprintf(txt, "\n--- Inicio do Processamento de Colisoes (calc) ---\n");

    Fila f = arena_get_fila(arena);
    Fila fg = get_ground_fila(ground);

    while (1) {
        void *I_void;
        if (!remover_da_fila(f, &I_void)) break; 
        Elemento I = (Elemento)I_void;

        void *J_void;
        if (!remover_da_fila(f, &J_void)) {
             Elemento Ipos = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), NULL, false);
             if (Ipos) adicionar_na_fila(fg, Ipos);
             break;
        }
        Elemento J = (Elemento)J_void;

        bool colide = formas_sobrepoem(elemento_get_dados(I), elemento_get_tipo(I), 
                                      elemento_get_dados(J), elemento_get_tipo(J));

        if (colide) {
            float areaI = elemento_calcular_area(I);
            float areaJ = elemento_calcular_area(J);

            if (txt) fprintf(txt, "\tColisao: ID %d (Area %.2f) vs ID %d (Area %.2f)\n", 
                             elemento_get_id_original(I), areaI, elemento_get_id_original(J), areaJ);

            *pontuacao += (areaI < areaJ) ? areaI : areaJ;

            if (areaI < areaJ) {
                if (txt) fprintf(txt, "\t-> ID %d sobrevive, ID %d destruido.\n", elemento_get_id_original(J), elemento_get_id_original(I));
                (*esmagadas)++;

                Elemento Jpos = elemento_clonar(J, elemento_get_x(J), elemento_get_y(J), NULL, false);
                if (Jpos) adicionar_na_fila(fg, Jpos);
            } 
            else {

                if (txt) fprintf(txt, "\t-> Colisao com modificacao de cores e clonagem.\n");
                
                Elemento Iorig = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), NULL, false);
                if (Iorig) adicionar_na_fila(fg, Iorig);

                const char* cor_ref = obter_cor_preenchimento(I);
                Elemento Jmod = elemento_clonar(J, elemento_get_x(J), elemento_get_y(J), cor_ref, false);
                if (Jmod) {
                    adicionar_na_fila(fg, Jmod);
                }

                Elemento Imod = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), NULL, true);
                if (Imod) {
                    adicionar_na_fila(fg, Imod);
                    (*clonadas)++;
                }
            }
        } else {
            Elemento Ipos = elemento_clonar(I, elemento_get_x(I), elemento_get_y(I), NULL, false);
            if (Ipos) adicionar_na_fila(fg, Ipos);

            Elemento Jpos = elemento_clonar(J, elemento_get_x(J), elemento_get_y(J), NULL, false);
            if (Jpos) adicionar_na_fila(fg, Jpos);
        }
    }

    if (txt) {
        fprintf(txt, "\n[calc]\n\tResult: %.2f\n\n", *pontuacao);
    }
    arena_limpar(arena);
}