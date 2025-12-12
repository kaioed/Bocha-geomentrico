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
#include "../formas/circulo/circulo.h"

void* creating_retangulo(float x, float y, float w, float h, char* fill, char* stroke, int id);
void* creating_texto(float x, float y, char* stroke, char* fill, char anchor, char* content, char* font, int id);
void* creating_linha(float x1, float y1, float x2, float y2, char* color, bool dashed, int id);
void* creating_circulo(float x, float y, float r, char* fill, char* stroke, int id);

Elemento mover_forma_preservando_id(Elemento original, float novo_x, float novo_y) {
    if (!original) return NULL;
    
    int id = elemento_get_id_original(original);
    void* dados = elemento_get_dados(original);
    TipoForma tipo = elemento_get_tipo(original);
    void* nova_forma_dados = NULL;

    switch(tipo) {
        case TIPO_CIRCULO: {
            float r = get_raio(dados);
            const char* cp = get_corPreenchimento_circulo(dados);
            const char* cb = get_corBorda_circulo(dados);
            nova_forma_dados = creating_circulo(novo_x, novo_y, r, (char*)cp, (char*)cb, id);
            break;
        }
        case TIPO_RETANGULO: {
            float w = get_largura(dados);
            float h = get_altura(dados);
            const char* cp = get_corPreenchimento_retangulo(dados);
            const char* cb = get_corBorda_retangulo(dados);
            nova_forma_dados = creating_retangulo(novo_x, novo_y, w, h, (char*)cp, (char*)cb, id);
            break;
        }
        case TIPO_LINHA: {
            float x1_ant = get_x1_linha(dados);
            float y1_ant = get_y1_linha(dados);
            float dx = novo_x - x1_ant;
            float dy = novo_y - y1_ant;
            float x2 = get_x2_linha(dados) + dx;
            float y2 = get_y2_linha(dados) + dy;
            const char* cor = get_cor_linha(dados);
            bool dashed = is_dashed_linha(dados);
            nova_forma_dados = creating_linha(novo_x, novo_y, x2, y2, (char*)cor, dashed, id);
            break;
        }
        case TIPO_TEXTO: {
            const char* cp = get_corPreenchimento_texto(dados);
            const char* cb = get_corBorda_texto(dados);
            char anc = get_anchor_texto(dados);
            const char* txt = get_conteudo_texto(dados);
            const char* font = get_fonte_texto(dados);
            nova_forma_dados = creating_texto(novo_x, novo_y, (char*)cb, (char*)cp, anc, (char*)txt, (char*)font, id);
            break;
        }
    }

    if (nova_forma_dados) {
        Elemento novo_el = elemento_criar_wrapper(id, tipo, nova_forma_dados, novo_x, novo_y);
        switch(tipo) {
            case TIPO_CIRCULO: liberar_circulo(dados); break;
            case TIPO_RETANGULO: liberar_retangulo(dados); break;
            case TIPO_LINHA: liberar_linha(dados); break;
            case TIPO_TEXTO: liberar_texto(dados); break;
        }
        free(original); 
        return novo_el;
    }
    return NULL;
}

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
                
                Carregador alvo = NULL;
                char lado_selecionado = ' ';

                if (l[0] == 'e' || l[0] == 'E') {
                    alvo = disparador_get_carregador_esq(&d);
                    if (!alvo || carregador_vazio(&alvo)) {
                        alvo = disparador_get_carregador_dir(&d);
                        lado_selecionado = 'd';
                    } else {
                        lado_selecionado = 'e';
                    }
                } else {
                    alvo = disparador_get_carregador_dir(&d);
                    if (!alvo || carregador_vazio(&alvo)) {
                        alvo = disparador_get_carregador_esq(&d);
                        lado_selecionado = 'e';
                    } else {
                        lado_selecionado = 'd';
                    }
                }

                if (!alvo || carregador_vazio(&alvo)) {
                    if (txt) fprintf(txt, "\tRajada cancelada: sem municao.\n");
                    continue;
                }

                char comando_lado[2] = {lado_selecionado, '\0'};
                
                Elemento formas_buffer[100];
                int qtd_formas = 0;

                for (int k = 0; k < 100; k++) {
                    if (carregador_vazio(&alvo)) break; 
                    carregar_disparador(&d, 1, comando_lado);
                    Elemento e = (Elemento)disparador_disparar_forma(&d);
                    if (!e) break;
                    formas_buffer[qtd_formas++] = e;
                    total_disparos++;
                }

                float curr_dx = dx;
                float curr_dy = dy;

                for (int k = 0; k < qtd_formas; k += 2) {
                    if (k + 1 >= qtd_formas) {
                        Elemento I = formas_buffer[k];
                        float x = disparador_get_x(&d) + curr_dx;
                        float y = disparador_get_y(&d) + curr_dy;
                        
                        Elemento I_movido = mover_forma_preservando_id(I, x, y);
                        if (I_movido) {
                            adicionar_na_fila(get_ground_fila(ground), I_movido);
                            if (txt) fprintf(txt, "\tRajada Impar: ID %d -> (%.1f, %.1f)\n", elemento_get_id_original(I_movido), x, y);
                        }
                        break;
                    }

                    Elemento I = formas_buffer[k];
                    Elemento J = formas_buffer[k+1];

                    float ix_pos = disparador_get_x(&d) + curr_dx;
                    float iy_pos = disparador_get_y(&d) + curr_dy;
                    curr_dx += ix;
                    curr_dy += iy;

                    float jx_pos = disparador_get_x(&d) + curr_dx;
                    float jy_pos = disparador_get_y(&d) + curr_dy;
                    curr_dx += ix;
                    curr_dy += iy;

                    Elemento I_movido = mover_forma_preservando_id(I, ix_pos, iy_pos);
                    Elemento J_movido = mover_forma_preservando_id(J, jx_pos, jy_pos);

                    float areaI = elemento_calcular_area(I_movido);
                    float areaJ = elemento_calcular_area(J_movido);

                    if (areaI < areaJ) {
                        pontuacao += areaI;
                        total_esmagadas++;
                        if (txt) fprintf(txt, "SOBREPOSICAO (RJD PAR): Forma I id:%d (area %.2f) esmagada pela Forma J id:%d (area %.2f).\n",
                            elemento_get_id_original(I_movido), areaI, elemento_get_id_original(J_movido), areaJ);
                        
                        if (J_movido) adicionar_na_fila(get_ground_fila(ground), J_movido);
                        
                        if (I_movido) {
                            void* dados = elemento_get_dados(I_movido);
                            switch(elemento_get_tipo(I_movido)) {
                                case TIPO_CIRCULO: liberar_circulo(dados); break;
                                case TIPO_RETANGULO: liberar_retangulo(dados); break;
                                case TIPO_LINHA: liberar_linha(dados); break;
                                case TIPO_TEXTO: liberar_texto(dados); break;
                            }
                            free(I_movido);
                        }
                    } else {
                        pontuacao += areaJ;
                        total_esmagadas++;
                        if (txt) fprintf(txt, "SOBREPOSICAO (RJD PAR): Forma J id:%d (area %.2f) esmagada pela Forma I id:%d (area %.2f).\n",
                            elemento_get_id_original(J_movido), areaJ, elemento_get_id_original(I_movido), areaI);
                        
                        if (I_movido) adicionar_na_fila(get_ground_fila(ground), I_movido);
                        
                        if (J_movido) {
                            void* dados = elemento_get_dados(J_movido);
                            switch(elemento_get_tipo(J_movido)) {
                                case TIPO_CIRCULO: liberar_circulo(dados); break;
                                case TIPO_RETANGULO: liberar_retangulo(dados); break;
                                case TIPO_LINHA: liberar_linha(dados); break;
                                case TIPO_TEXTO: liberar_texto(dados); break;
                            }
                            free(J_movido);
                        }
                    }
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

                    if (campos_lidos == 4 && strcmp(flag_visual, "v") == 0) {
                        void* linha_tiro = creating_linha(x_inicial, y_inicial, x_final, y_final, "red", true, -1);
                        Elemento e_linha = elemento_criar_wrapper(-1, TIPO_LINHA, linha_tiro, x_inicial, y_inicial);
                        adicionar_na_fila(get_ground_fila(ground), e_linha);

                        void* marcador = creating_circulo(x_final, y_final, 3.0, "none", "red", -1);
                        Elemento e_marcador = elemento_criar_wrapper(-1, TIPO_CIRCULO, marcador, x_final, y_final);
                        adicionar_na_fila(get_ground_fila(ground), e_marcador);

                        char str_dx[32], str_dy[32];
                        sprintf(str_dx, "%.2f", dx);
                        sprintf(str_dy, "%.2f", dy);

                        void* guia_h = creating_linha(x_inicial, y_inicial, x_final, y_inicial, "purple", true, -1);
                        Elemento e_guia_h = elemento_criar_wrapper(-1, TIPO_LINHA, guia_h, x_inicial, y_inicial);
                        adicionar_na_fila(get_ground_fila(ground), e_guia_h);

                        void* guia_v = creating_linha(x_final, y_inicial, x_final, y_final, "purple", true, -1);
                        Elemento e_guia_v = elemento_criar_wrapper(-1, TIPO_LINHA, guia_v, x_final, y_inicial);
                        adicionar_na_fila(get_ground_fila(ground), e_guia_v);

                        float mid_h_x = x_inicial + dx * 0.5;
                        void* txt_dx = creating_texto(mid_h_x, y_inicial - 5.0, "none", "purple", 'm', str_dx, "sans-serif", -1);
                        Elemento e_txt_dx = elemento_criar_wrapper(-1, TIPO_TEXTO, txt_dx, mid_h_x, y_inicial - 5.0);
                        adicionar_na_fila(get_ground_fila(ground), e_txt_dx);

                        float mid_v_y = y_inicial + dy * 0.5;
                        void* txt_dy = creating_texto(x_final + 15.0, mid_v_y, "none", "purple", 'm', str_dy, "sans-serif", -1);
                        Elemento e_txt_dy = elemento_criar_wrapper(-1, TIPO_TEXTO, txt_dy, x_final + 10.0, mid_v_y);
                        adicionar_na_fila(get_ground_fila(ground), e_txt_dy);
                    }

                    void* dadosBase = elemento_get_dados(base);
                    switch(elemento_get_tipo(base)) {
                        case TIPO_CIRCULO: liberar_circulo(dadosBase); break;
                        case TIPO_RETANGULO: liberar_retangulo(dadosBase); break;
                        case TIPO_LINHA: liberar_linha(dadosBase); break;
                        case TIPO_TEXTO: liberar_texto(dadosBase); break;
                    }
                    free(base);

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