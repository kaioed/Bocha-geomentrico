#include "qry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disparador/disparador.h"
#include "../elemento/elemento.h"
#include "../contexto/contexto.h"
#include "../fisica/fisica.h"
#include "../relatorio/relatorio.h"
#include "../geo/geo.h"
#include "../campo/arena.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/texto/texto.h"
#include "../formas/linha/linha.h"

void* creating_retangulo(float x, float y, float w, float h, char* fill, char* stroke, int id);
void* creating_texto(float x, float y, char* stroke, char* fill, char anchor, char* content, char* font, int id);
void* creating_linha(float x1, float y1, float x2, float y2, char* color, bool dashed, int id);

TipoForma forma_get_tipo(void* forma) {
    return elemento_get_tipo((Elemento)forma);
}

int forma_get_id_original(void* forma) {
    return elemento_get_id_original((Elemento)forma);
}

void forma_set_destruida(void* forma, bool status) {
    elemento_set_destruido((Elemento)forma, status);
}

void process_qry(FILE *qry, FILE *svg, Ground ground, FILE *txt) {
    if (!qry || !svg || !ground || !txt) return;

    Arena arena = arena_criar();
    Contexto ctx = contexto_criar();

    int total_disparos = 0;
    int total_esmagadas = 0;
    int total_clonadas = 0;
    double pontuacao = 0.0;
    int comandos_executados = 0;

    char buffer[512];
    char cmd[64];

    while (fgets(buffer, sizeof(buffer), qry)) {
        cmd[0] = '\0';
        sscanf(buffer, "%s", cmd);
        if (strlen(cmd) == 0) continue;
        comandos_executados++;

        if (strncmp(cmd, "pd", 2) == 0) {
            int id; float x, y;
            sscanf(buffer, "pd %d %f %f", &id, &x, &y);
            contexto_adicionar_disparador(ctx, id, x, y);
        }
        else if (strncmp(cmd, "lc", 2) == 0) {
            int id, n;
            sscanf(buffer, "lc %d %d", &id, &n);
            if (n > 0) contexto_adicionar_carregador(ctx, id, get_ground_fila(ground), n, txt);
        }
        else if (strncmp(cmd, "atch", 4) == 0) {
            int i, e, d;
            sscanf(buffer, "atch %d %d %d", &i, &e, &d);
            contexto_vincular_equipamentos(ctx, i, e, d, txt);
        }
        else if (strncmp(cmd, "shft", 4) == 0) {
            int id, n; char l[2];
            sscanf(buffer, "shft %d %1s %d", &id, l, &n);
            Disparador d = contexto_buscar_disparador(ctx, id);
            if (d) carregar_disparador(&d, n, l);
            else if (txt) fprintf(txt, "SHFT: ID %d nao encontrado.\n", id);
        }
        else if (strncmp(cmd, "rjd", 3) == 0) {
            int id; float dx, dy, ix, iy; char l[2];
            sscanf(buffer, "rjd %d %1s %f %f %f %f", &id, l, &dx, &dy, &ix, &iy);
            Disparador d = contexto_buscar_disparador(ctx, id);
            
            if (d) {
                if(txt) fprintf(txt, "[rjd] Rajada ID %d lado %s\n", id, l);
                int k=0;
                while(1) {
                    carregar_disparador(&d, 1, l);
                    Elemento base = (Elemento)disparador_disparar_forma(&d);
                    if (!base) break;
                    total_disparos++;

                    float x = disparador_get_x(&d) + dx + k*ix;
                    float y = disparador_get_y(&d) + dy + k*iy;
                    
                    Elemento clone = elemento_clonar(base, x, y, NULL, false);
                    if (clone) {
                        arena_adicionar_elemento(arena, clone);
                        
                        if (txt) fprintf(txt, "\tRajada %d: ID %d -> (%.1f, %.1f)\n", k+1, elemento_get_id_original(base), x, y);
                    }
                    k++;
                }
            } else if (txt) fprintf(txt, "RJD: ID %d nao encontrado.\n", id);
        }
        else if (strncmp(cmd, "dsp", 3) == 0) {
            int id; float dx, dy;
            char flag_visual[4] = "";
            int campos_lidos = sscanf(buffer, "dsp %d %f %f %3s", &id, &dx, &dy, flag_visual);
            Disparador d = contexto_buscar_disparador(ctx, id);
            
            if (d) {
                float x_inicial = disparador_get_x(&d);
                float y_inicial = disparador_get_y(&d);
                Elemento base = (Elemento)disparador_disparar_forma(&d);
                
                if (base) {
                    total_disparos++;
                    float x_final = x_inicial + dx;
                    float y_final = y_inicial + dy;
                    Elemento clone = elemento_clonar(base, x_final, y_final, NULL, false);
                    
                    if (clone) {
                        // CORREÇÃO: Adiciona APENAS na arena.
                        arena_adicionar_elemento(arena, clone);
                    }
                    if (txt) fprintf(txt, "DSP: ID %d -> (%.1f, %.1f)\n", elemento_get_id_original(base), x_final, y_final);

                    // Desenho auxiliar do comando 'v' (visual) continua direto no ground pois é decorativo
                    if (campos_lidos == 4 && strcmp(flag_visual, "v") == 0) {
                        double tam_caixa = 20.0;
                        void* caixa = creating_retangulo(x_inicial - (tam_caixa/2.0), y_inicial + (tam_caixa/2.0), tam_caixa, tam_caixa, "none", "red", -1); 
                        Elemento e_caixa = elemento_criar_wrapper(-1, TIPO_RETANGULO, caixa, x_inicial - (tam_caixa/2.0), y_inicial + (tam_caixa/2.0));
                        adicionar_na_fila(get_ground_fila(ground), e_caixa);

                        char id_str[16];
                        sprintf(id_str, "%d", id);
                        void* texto_id = creating_texto(x_inicial, y_inicial, "red", "red", 'm', id_str, "sans-serif,bold,12px", -1);
                        Elemento e_texto = elemento_criar_wrapper(-1, TIPO_TEXTO, texto_id, x_inicial, y_inicial);
                        adicionar_na_fila(get_ground_fila(ground), e_texto);

                        void* proj_y = creating_linha(x_final, y_inicial, x_final, y_final, "#FF0000", true, -1);
                        Elemento e_proj_y = elemento_criar_wrapper(-1, TIPO_LINHA, proj_y, x_final, y_inicial);
                        adicionar_na_fila(get_ground_fila(ground), e_proj_y);

                        void* proj_x = creating_linha(x_inicial, y_final, x_final, y_final, "#FF0000", true, -1);
                        Elemento e_proj_x = elemento_criar_wrapper(-1, TIPO_LINHA, proj_x, x_inicial, y_final);
                        adicionar_na_fila(get_ground_fila(ground), e_proj_x);
                    }

                } else if (txt) fprintf(txt, "DSP: Disparador %d vazio.\n", id);
            } else if (txt) fprintf(txt, "DSP: ID %d nao encontrado.\n", id);
        }
        else if (strncmp(cmd, "calc", 4) == 0) {
            fisica_processar_colisoes(arena, ground, txt, &pontuacao, &total_esmagadas, &total_clonadas);
        }
    }

    if (txt) {
        fprintf(txt, "\n--- RELATORIO FINAL ---\n");
        fprintf(txt, "Pontuacao: %.2f\nInstrucoes: %d\nDisparos: %d\nEsmagadas: %d\nClonadas: %d\n", 
            pontuacao, comandos_executados, total_disparos, total_esmagadas, total_clonadas);
    }

    relatorio_gerar_svg(ground, svg);

    arena_destruir(arena);
    contexto_destruir(ctx);
}

void* creating_retangulo(float x, float y, float w, float h, char* fill, char* stroke, int id) {
    return criar_retangulo(x, y, w, h, fill, stroke, id);
}
void* creating_texto(float x, float y, char* stroke, char* fill, char anchor, char* content, char* font, int id) {
    return criar_texto(x, y, stroke, fill, anchor, content, font, id);
}
void* creating_linha(float x1, float y1, float x2, float y2, char* color, bool dashed, int id) {
    return criar_linha(x1, y1, x2, y2, color, dashed, id);
}