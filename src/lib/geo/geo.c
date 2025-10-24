#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geo.h"
#include "../qry/qry.h" 
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"


typedef struct {
    Fila todas_as_formas;     
    Pilha pilha_clones_para_libertar; 
} GroundStruct;


typedef struct {
    int id_original;
    TipoForma tipo;
    void* dados_forma;     
    bool foi_destruida;
    bool foi_clonada;
    float x_centro, y_centro; 
    int id_disparador_origem;
} FormaStruct;


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
            wrapper->x_centro = get_x_retangulo((Retangulo*)dados_forma);
            wrapper->y_centro = get_y_retangulo((Retangulo*)dados_forma);
            break;
        case TIPO_LINHA:
            wrapper->x_centro = get_x1_linha((Linha*)dados_forma);
            wrapper->y_centro = get_y1_linha((Linha*)dados_forma);
            break;
        case TIPO_TEXTO:
            wrapper->x_centro = get_x_texto((Texto*)dados_forma);
            wrapper->y_centro = get_y_texto((Texto*)dados_forma);
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
   

    if (!svg || !geo) { 
         fprintf(stderr, "Erro: Arquivo geo ou svg inválido em process_geo.\n");
         if (ground) destruir_fila(ground->todas_as_formas);
         free(ground);
         return NULL;
    }

    fprintf(svg, "<svg xmlns='http://www.w3.org/2000/svg'>\n");

    char linha_buffer[1024];
    char comando[16]; 
    char* args_ptr;   

    while (fgets(linha_buffer, sizeof(linha_buffer), geo) != NULL) {
        
        if (sscanf(linha_buffer, " %15s", comando) != 1) continue; 

        char* cmd_end_ptr = strstr(linha_buffer, comando);
        if (cmd_end_ptr) {
            cmd_end_ptr += strlen(comando);
            while (*cmd_end_ptr == ' ' || *cmd_end_ptr == '\t') cmd_end_ptr++;
            args_ptr = cmd_end_ptr; 
        } else {
             continue; 
        }

        FormaStruct* nova_forma = NULL; 

        if (strcmp(comando, "c") == 0) {
            int id; float x, y, r; char corb[32], corp[32];
            // *** CORREÇÃO: Removido '&' de corb e corp ***
            if (sscanf(args_ptr, "%d %f %f %f %31s %31s", &id, &x, &y, &r, corb, corp) == 6) {
                 Circulo* c = criar_circulo(x, y, r, corp, corb, id);
                 if (c) {
                     nova_forma = criar_forma_wrapper(id, TIPO_CIRCULO, c);
                     fprintf(svg, "<circle id='%d' cx='%.1f' cy='%.1f' r='%.1f' stroke='%s' fill='%s'/>\n",
                             id, x, y, r, corb, corp);
                 }
            } else { fprintf(stderr, "Erro lendo 'c': %s", linha_buffer); }
        }
        else if (strcmp(comando, "r") == 0) {
            int id; float x, y, w, h; char corb[32], corp[32];
            // *** CORREÇÃO: Removido '&' de corb e corp ***
            if (sscanf(args_ptr, "%d %f %f %f %f %31s %31s", &id, &x, &y, &w, &h, corb, corp) == 7) {
                 Retangulo* r = criar_retangulo(x, y, w, h, corp, corb, id);
                 if (r) {
                     nova_forma = criar_forma_wrapper(id, TIPO_RETANGULO, r);
                     fprintf(svg, "<rect id='%d' x='%.1f' y='%.1f' width='%.1f' height='%.1f' stroke='%s' fill='%s'/>\n",
                             id, x, y, w, h, corb, corp);
                 }
            } else { fprintf(stderr, "Erro lendo 'r': %s", linha_buffer); }
        }
        else if (strcmp(comando, "l") == 0) {
            int id; float x1, y1, x2, y2; char cor[32];
            // *** CORREÇÃO: Removido '&' de cor *** (Embora não estivesse dando warning, é o correto)
            if (sscanf(args_ptr, "%d %f %f %f %f %31s", &id, &x1, &y1, &x2, &y2, cor) == 6) {
                 Linha* l = criar_linha(x1, y1, x2, y2, cor, id);
                 if (l) {
                     nova_forma = criar_forma_wrapper(id, TIPO_LINHA, l);
                     fprintf(svg, "<line id='%d' x1='%.1f' y1='%.1f' x2='%.1f' y2='%.1f' stroke='%s'/>\n",
                             id, x1, y1, x2, y2, cor);
                 }
            } else { fprintf(stderr, "Erro lendo 'l': %s", linha_buffer); }
        }
        else if (strcmp(comando, "t") == 0) {
            int id; float x, y; char corb[32], corp[32], anchor = ' ', texto[256] = ""; 
            // *** CORREÇÃO: Removido '&' de corb e corp ***
            int scan_result = sscanf(args_ptr, "%d %f %f %31s %31s %c %[^\n]", &id, &x, &y, corb, corp, &anchor, texto);
            
            if (scan_result >= 5) {
                if (scan_result < 6 || anchor == ' ' || anchor == '\n') anchor = 'i'; 
                if (scan_result < 7) texto[0] = '\0';

                Texto* t = criar_texto(x, y, corb, corp, anchor, texto, NULL, id);
                if (t) {
                     nova_forma = criar_forma_wrapper(id, TIPO_TEXTO, t);

                     const char *ancora_svg = "start"; 
                     if (anchor == 'm' || anchor == 'M') ancora_svg = "middle";
                     else if (anchor == 'e' || anchor == 'E' || anchor == 'f' || anchor == 'F') ancora_svg = "end";
                     else if (anchor == 's' || anchor == 'S' || anchor == 'i' || anchor == 'I') ancora_svg = "start";

                     fprintf(svg, "<text id='%d' x='%.1f' y='%.1f' stroke='%s' fill='%s' text-anchor='%s'>%s</text>\n",
                             id, x, y, corb, corp, ancora_svg, texto);
                 }
            } else { 
                 fprintf(stderr, "Erro ao ler comando 't': Formato inválido na linha: %s", linha_buffer);
            }
        }
        else if (strcmp(comando, "ts") == 0) {
            // Ignora
        } else {
             fprintf(stderr, "Comando desconhecido no .geo: '%s'\n", comando);
        }

        if (nova_forma) {
            adicionar_na_fila(ground->todas_as_formas, (Forma)nova_forma); 
        }
    }

    fprintf(svg, "</svg>\n"); 
    return (Ground)ground; 
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