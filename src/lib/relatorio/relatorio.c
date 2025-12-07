#include "relatorio.h"
#include "../elemento/elemento.h"
#include "../fila/fila.h"
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include <stdlib.h>
#include <string.h>

void relatorio_gerar_svg(Ground ground, FILE* svg) {
    if (!ground || !svg) return;
    
    fprintf(svg, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(svg, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 1000\">\n");

    Fila f = get_ground_fila(ground);
    Fila temp = iniciar_fila();
    void* ptr;

    while(remover_da_fila(f, &ptr)) {
        Elemento e = (Elemento)ptr;
        if (!elemento_foi_destruido(e)) {
            void* d = elemento_get_dados(e);
            
            switch (elemento_get_tipo(e)) {
                case TIPO_CIRCULO:
                    fprintf(svg, "<circle cx='%.2f' cy='%.2f' r='%.2f' fill='%s' stroke='%s' fill-opacity='0.5'/>\n",
                        (double)get_x(d), (double)get_y(d), (double)get_raio(d), 
                        get_corPreenchimento_circulo(d), get_corBorda_circulo(d));
                    break;

                case TIPO_RETANGULO:
                    fprintf(svg, "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' stroke='%s' fill-opacity='0.5'/>\n",
                        (double)get_x_retangulo(d), (double)get_y_retangulo(d), 
                        (double)get_largura(d), (double)get_altura(d), 
                        get_corPreenchimento_retangulo(d), get_corBorda_retangulo(d));
                    break;

                case TIPO_LINHA:
                    fprintf(svg, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'",
                        (double)get_x1_linha(d), (double)get_y1_linha(d), 
                        (double)get_x2_linha(d), (double)get_y2_linha(d), 
                        get_cor_linha(d));
                    
                    if (is_dashed_linha(d)) {
                        fprintf(svg, " stroke-dasharray='4,2'");
                    }
                    fprintf(svg, "/>\n");
                    break;

                case TIPO_TEXTO: {
                    char anc = get_anchor_texto(d);
                    const char* text_anchor = "start";
                    if (anc == 'm' || anc == 'M') text_anchor = "middle";
                    else if (anc == 'e' || anc == 'E') text_anchor = "end";
                    else if (anc == 's' || anc == 'S') text_anchor = "start";
                    
                    fprintf(svg, "<text x='%.2f' y='%.2f' fill='%s' stroke='%s' text-anchor='%s' fill-opacity='0.5'>%s</text>\n",
                        (double)get_x_texto(d), (double)get_y_texto(d), 
                        get_corPreenchimento_texto(d), get_corBorda_texto(d),
                        text_anchor, get_conteudo_texto(d));
                    break;
                }
            }
        }
        adicionar_na_fila(temp, ptr);
    }
    
    while(remover_da_fila(temp, &ptr)) adicionar_na_fila(f, ptr);
    destruir_fila(temp);

    fprintf(svg, "</svg>\n");
}