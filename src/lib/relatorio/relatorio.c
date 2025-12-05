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
    
    fprintf(svg, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    fprintf(svg, "<svg xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns=\"http://www.w3.org/2000/svg\" width=\"1000\" height=\"1000\">\n");
    fprintf(svg, "<g>\n");

    Fila f = get_ground_fila(ground);
    Fila temp = iniciar_fila();
    void* ptr;

    while(remover_da_fila(f, &ptr)) {
        Elemento e = (Elemento)ptr;
        if (!elemento_foi_destruido(e)) {
            void* d = elemento_get_dados(e);
            int id = elemento_get_id_original(e);
            
            switch (elemento_get_tipo(e)) {
                case TIPO_CIRCULO:
                    fprintf(svg, " <circle id=\"%d\" cx=\"%lf\" cy=\"%lf\" r=\"%lf\" stroke=\"%s\" fill=\"%s\" opacity=\"0.500000\" stroke-width=\"1.500000\" />\n",
                        id, (double)get_x(d), (double)get_y(d), (double)get_raio(d), get_corBorda_circulo(d), get_corPreenchimento_circulo(d));
                    break;
                case TIPO_RETANGULO:
                    fprintf(svg, "\t<rect id=\"%d\" x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" stroke=\"%s\" fill=\"%s\" opacity=\"0.500000\" stroke-width=\"1.500000\" />\n",
                        id, (double)get_x_retangulo(d), (double)get_y_retangulo(d), (double)get_largura(d), (double)get_altura(d), get_corBorda_retangulo(d), get_corPreenchimento_retangulo(d));
                    break;
                case TIPO_LINHA:
                    if (is_dashed_linha(d)) {
                        fprintf(svg, "\t<line id=\"%d\" x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"%s\" stroke-dasharray=\"1, 1\" stroke-width=\"1.500000\" />\n",
                            id, (double)get_x1_linha(d), (double)get_y1_linha(d), (double)get_x2_linha(d), (double)get_y2_linha(d), get_cor_linha(d));
                    } else {
                        fprintf(svg, "\t<line id=\"%d\" x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"%s\" stroke-width=\"1.500000\" />\n",
                            id, (double)get_x1_linha(d), (double)get_y1_linha(d), (double)get_x2_linha(d), (double)get_y2_linha(d), get_cor_linha(d));
                    }
                    break;
                case TIPO_TEXTO: {
                    char anc = get_anchor_texto(d);
                    const char* s_anc = "start";
                    if (anc=='m'||anc=='M') s_anc="middle";
                    else if (anc=='e'||anc=='E'||anc=='f'||anc=='F') s_anc="end";
                    
                    const char* fp = get_fonte_texto(d);
                    char fam[64]="sans-serif", weight[16]="normal", size[16]="12";
                    if(fp && strlen(fp)>0) sscanf(fp, "%[^,],%[^,],%s", fam, weight, size);

                    fprintf(svg, "\t<text id=\"%d\" x=\"%lf\" y=\"%lf\" stroke=\"%s\" fill=\"%s\" font-family=\"%s\" font-weight=\"%s\" font-size=\"%spt\" text-anchor=\"%s\">%s</text>\n",
                        id, (double)get_x_texto(d), (double)get_y_texto(d), get_corBorda_texto(d), get_corPreenchimento_texto(d),
                        fam, weight, size, s_anc, get_conteudo_texto(d));
                    break;
                }
            }
        }
        adicionar_na_fila(temp, ptr);
    }
    
    while(remover_da_fila(temp, &ptr)) adicionar_na_fila(f, ptr);
    destruir_fila(temp);

    fprintf(svg, "</g>\n</svg>\n");
}