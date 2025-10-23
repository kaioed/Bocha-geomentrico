#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geo.h"
#include "../qry/qry.h" 
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"


static FormaStruct* criar_forma_wrapper(int id, TipoForma tipo, void* dados_forma) {
    FormaStruct* wrapper = (FormaStruct*) malloc(sizeof(FormaStruct));
    if (!wrapper) return NULL;

    wrapper->id_original = id;
    wrapper->tipo = tipo;
    wrapper->dados_forma = dados_forma;
    wrapper->foi_destruida = false;
    wrapper->foi_clonada = false;


    switch (tipo) {
        case TIPO_CIRCULO:
            wrapper->x_centro = get_x((Circulo*)dados_forma);
            wrapper->y_centro = get_y((Circulo*)dados_forma);
            break;
        case TIPO_RETANGULO:
            wrapper->x_centro = get_x_retangulo((Retangulo*)dados_forma) + get_largura((Retangulo*)dados_forma) / 2.0f;
            wrapper->y_centro = get_y_retangulo((Retangulo*)dados_forma) + get_altura((Retangulo*)dados_forma) / 2.0f;
            break;
        default: 
            wrapper->x_centro = 0;
            wrapper->y_centro = 0;
            break;
    }
    return wrapper;
}

Ground process_geo(FILE *geo, FILE *svg) {
    GroundStruct* ground = (GroundStruct*) malloc(sizeof(GroundStruct));
    if (!ground) {
        fprintf(stderr, "Erro ao alocar memoria para Ground.\n");
        return NULL;
    }
    ground->todas_as_formas = iniciar_fila();

    fprintf(svg, "<svg xmlns='http://www.w3.org/2000/svg'>\n");

    char linha_buffer[1024];
    char comando;
    char* resto_linha;

    while (fgets(linha_buffer, sizeof(linha_buffer), geo) != NULL) {
        if (sscanf(linha_buffer, " %c", &comando) != 1) continue;
        resto_linha = strchr(linha_buffer, comando) + 1;

        FormaStruct* nova_forma = NULL;

        switch (comando) {
            case 'c': {
                int id; float x, y, r; char corb[32], corp[32];
                sscanf(resto_linha, "%d %f %f %f %s %s", &id, &x, &y, &r, corb, corp);
                Circulo* c = criar_circulo(x, y, r, corp, corb, id);
                nova_forma = criar_forma_wrapper(id, TIPO_CIRCULO, c);
                fprintf(svg, "<circle id='%d' cx='%.1f' cy='%.1f' r='%.1f' stroke='%s' fill='%s'/>\n",
                        id, x, y, r, corb, corp);
                break;
            }
            case 'r': {
                int id; float x, y, w, h; char corb[32], corp[32];
                sscanf(resto_linha, "%d %f %f %f %f %s %s", &id, &x, &y, &w, &h, corb, corp);
                Retangulo* r = criar_retangulo(x, y, w, h, corp, corb, id);
                nova_forma = criar_forma_wrapper(id, TIPO_RETANGULO, r);
                fprintf(svg, "<rect id='%d' x='%.1f' y='%.1f' width='%.1f' height='%.1f' stroke='%s' fill='%s'/>\n",
                        id, x, y, w, h, corb, corp);
                break;
            }
            case 'l': {
                int id; float x1, y1, x2, y2; char cor[32];
                sscanf(resto_linha, "%d %f %f %f %f %s", &id, &x1, &y1, &x2, &y2, cor);
                Linha* l = criar_linha(x1, y1, x2, y2, cor, id);
                nova_forma = criar_forma_wrapper(id, TIPO_LINHA, l);
                 fprintf(svg, "<line id='%d' x1='%.1f' y1='%.1f' x2='%.1f' y2='%.1f' stroke='%s'/>\n",
                        id, x1, y1, x2, y2, cor);
                break;
            }
            case 't': {
                int id; float x, y; char corb[32], corp[32], anchor, texto[256];
                sscanf(resto_linha, "%d %f %f %s %s %c %[^\n]", &id, &x, &y, corb, corp, &anchor, texto);
                Texto* t = criar_texto(x, y, corp, texto, NULL, id);
                nova_forma = criar_forma_wrapper(id, TIPO_TEXTO, t);
                 char *ancora_svg = (anchor == 'm') ? "middle" : (anchor == 'f' ? "end" : "start");
                fprintf(svg, "<text id='%d' x='%.1f' y='%.1f' stroke='%s' fill='%s' text-anchor='%s'>%s</text>\n",
                        id, x, y, corb, corp, ancora_svg, texto);
                break;
            }
        }

        if (nova_forma) {
            adicionar_na_fila(ground->todas_as_formas, nova_forma);
        }
    }

    fprintf(svg, "</svg>\n"); // Fecha o SVG inicial
    return ground;
}

void destruir_ground(Ground g) {
    if (!g) return;
    GroundStruct* ground = (GroundStruct*)g;
    void* forma_wrapper_ptr;

    while(remover_da_fila(ground->todas_as_formas, &forma_wrapper_ptr)) {
        FormaStruct* forma = (FormaStruct*)forma_wrapper_ptr;
        if (forma) {
        
            switch(forma->tipo) {
                case TIPO_CIRCULO: liberar_circulo(forma->dados_forma); break;
                case TIPO_RETANGULO: liberar_retangulo(forma->dados_forma); break;
                case TIPO_LINHA: liberar_linha(forma->dados_forma); break;
                case TIPO_TEXTO: liberar_texto(forma->dados_forma); break;
            }
            
            free(forma);
        }
    }
    
    destruir_fila(ground->todas_as_formas);

    free(ground);
}

Fila get_ground_fila(Ground g) {
    if (!g) return NULL;
    return ((GroundStruct*)g)->todas_as_formas;
}

Pilha get_ground_pilha_clones(Ground g) {
    return g ? ((GroundStruct*)g)->pilha_clones_para_libertar : NULL;
}

