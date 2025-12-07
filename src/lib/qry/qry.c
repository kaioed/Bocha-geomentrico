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
#include "../formas/circulo/circulo.h" // Necessário para criar o marcador circular

// Funções auxiliares
void* creating_retangulo(float x, float y, float w, float h, char* fill, char* stroke, int id);
void* creating_texto(float x, float y, char* stroke, char* fill, char anchor, char* content, char* font, int id);
void* creating_linha(float x1, float y1, float x2, float y2, char* color, bool dashed, int id);
void* creating_circulo(float x, float y, float r, char* fill, char* stroke, int id); // Nova auxiliar

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
                        arena_adicionar_elemento(arena, clone);
                    }
                    if (txt) fprintf(txt, "DSP: ID %d -> (%.1f, %.1f)\n", elemento_get_id_original(base), x_final, y_final);

                    // --- INICIO DA CORREÇÃO VISUAL ---
                    if (campos_lidos == 4 && strcmp(flag_visual, "v") == 0) {
                        // 1. Linha Tracejada do Disparador até o Destino (Vermelha)
                        void* linha_tiro = creating_linha(x_inicial, y_inicial, x_final, y_final, "red", true, -1);
                        Elemento e_linha = elemento_criar_wrapper(-1, TIPO_LINHA, linha_tiro, x_inicial, y_inicial);
                        adicionar_na_fila(get_ground_fila(ground), e_linha);

                        // 2. Pequeno círculo marcador no destino (Vermelho, sem preenchimento)
                        void* marcador = creating_circulo(x_final, y_final, 3.0, "none", "red", -1);
                        Elemento e_marcador = elemento_criar_wrapper(-1, TIPO_CIRCULO, marcador, x_final, y_final);
                        adicionar_na_fila(get_ground_fila(ground), e_marcador);

                        // 3. Guias e Textos (Roxo/Purple)
                        char str_dx[32], str_dy[32];
                        sprintf(str_dx, "%.2f", dx);
                        sprintf(str_dy, "%.2f", dy);

                        // Linha Guia Horizontal (x_shooter -> x_final, mantendo y_shooter)
                        void* guia_h = creating_linha(x_inicial, y_inicial, x_final, y_inicial, "purple", true, -1);
                        Elemento e_guia_h = elemento_criar_wrapper(-1, TIPO_LINHA, guia_h, x_inicial, y_inicial);
                        adicionar_na_fila(get_ground_fila(ground), e_guia_h);

                        // Linha Guia Vertical (x_final, y_shooter -> y_final)
                        void* guia_v = creating_linha(x_final, y_inicial, x_final, y_final, "purple", true, -1);
                        Elemento e_guia_v = elemento_criar_wrapper(-1, TIPO_LINHA, guia_v, x_final, y_inicial);
                        adicionar_na_fila(get_ground_fila(ground), e_guia_v);

                        // Texto DX (No meio da linha horizontal)
                        float mid_h_x = x_inicial + dx * 0.5;
                        void* txt_dx = creating_texto(mid_h_x, y_inicial - 5.0, "none", "purple", 'm', str_dx, "sans-serif", -1);
                        Elemento e_txt_dx = elemento_criar_wrapper(-1, TIPO_TEXTO, txt_dx, mid_h_x, y_inicial - 5.0);
                        adicionar_na_fila(get_ground_fila(ground), e_txt_dx);

                        // Texto DY (No meio da linha vertical)
                        float mid_v_y = y_inicial + dy * 0.5;
                        // Nota: O projeto original rotaciona este texto. O seu sistema de Texto atual não suporta rotação facilmente.
                        // O texto aparecerá na horizontal, mas na posição e cor corretas.
                        void* txt_dy = creating_texto(x_final + 15.0, mid_v_y, "none", "purple", 'm', str_dy, "sans-serif", -1);
                        Elemento e_txt_dy = elemento_criar_wrapper(-1, TIPO_TEXTO, txt_dy, x_final + 10.0, mid_v_y);
                        adicionar_na_fila(get_ground_fila(ground), e_txt_dy);
                    }
                    // --- FIM DA CORREÇÃO VISUAL ---

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
void* creating_circulo(float x, float y, float r, char* fill, char* stroke, int id) {
    return criar_circulo(x, y, r, fill, stroke, id);
}