#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "qry.h"
#include "../geo/geo.h" 
#include "../disparador/disparador.h"
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"

static int proximo_id_clone = 10000;

static float calcular_area_forma(FormaStruct* f) {
    if (!f) return 0.0f;
    switch (f->tipo) {
        case TIPO_CIRCULO:   return area_circulo(f->dados_forma);
        case TIPO_RETANGULO: return area_retangulo(f->dados_forma);
        default: return 0.0f;
    }
}

static FormaStruct* clonar_forma(FormaStruct* original, float x, float y, const char* nova_cor_borda, bool trocar_cores) {
    if (!original) return NULL;

    int id_clone = proximo_id_clone++;
    void* dados_clonados = NULL;

    switch(original->tipo) {
        case TIPO_CIRCULO: {
            Circulo* orig_c = (Circulo*)original->dados_forma;
            const char* cor_b = trocar_cores ? get_corPreenchimento_circulo(orig_c) : (nova_cor_borda ? nova_cor_borda : get_corBorda_circulo(orig_c));
            const char* cor_p = trocar_cores ? get_corBorda_circulo(orig_c) : get_corPreenchimento_circulo(orig_c);
            dados_clonados = criar_circulo(x, y, get_raio(orig_c), (char*)cor_p, (char*)cor_b, id_clone);
            break;
        }
        case TIPO_RETANGULO: {
            Retangulo* orig_r = (Retangulo*)original->dados_forma;
            const char* cor_b = trocar_cores ? get_corPreenchimento_retangulo(orig_r) : (nova_cor_borda ? nova_cor_borda : get_corBorda_retangulo(orig_r));
            const char* cor_p = trocar_cores ? get_corBorda_retangulo(orig_r) : get_corPreenchimento_retangulo(orig_r);
            dados_clonados = criar_retangulo(x, y, get_largura(orig_r), get_altura(orig_r), (char*)cor_p, (char*)cor_b, id_clone);
            break;
        }
       
        case TIPO_LINHA: {
            Linha* orig_l = (Linha*)original->dados_forma;
           
            const char* cor_original = get_cor_linha(orig_l);
            
            const char* nova_cor = nova_cor_borda ? nova_cor_borda : cor_original;
            float x1 = get_x1_linha(orig_l);
            float y1 = get_y1_linha(orig_l);
            float x2 = get_x2_linha(orig_l);
            float y2 = get_y2_linha(orig_l);
           
            dados_clonados = criar_linha(x, y, x + (x2 - x1), y + (y2 - y1), (char*)nova_cor, id_clone);
            break;
        }
       
        case TIPO_TEXTO: {
            Texto* orig_t = (Texto*)original->dados_forma;
           
            const char* cor_original = get_cor_texto(orig_t);
            const char* nova_cor = nova_cor_borda ? nova_cor_borda : cor_original;
            
           
            dados_clonados = criar_texto(x, y, (char*)nova_cor, (char*)get_conteudo_texto(orig_t), NULL, id_clone);
            break;
        }
    }

    if (!dados_clonados) return NULL;

    FormaStruct* wrapper_clone = malloc(sizeof(FormaStruct));
    wrapper_clone->id_original = id_clone;
    wrapper_clone->tipo = original->tipo;
    wrapper_clone->dados_forma = dados_clonados;
    wrapper_clone->foi_clonada = true;
    wrapper_clone->foi_destruida = false;
    wrapper_clone->x_centro = x;
    // Para retângulos, o y é o canto superior, então o centro é diferente
    if(original->tipo == TIPO_RETANGULO) {
         wrapper_clone->y_centro = y + get_altura(dados_clonados) / 2.0f;
    } else {
         wrapper_clone->y_centro = y;
    }

    return wrapper_clone;
}


typedef struct {
    Fila formas_na_arena;
} ArenaStruct;



bool circulo_colide_circulo(Circulo* c1, Circulo* c2) {
    float dist_x = get_x(c1) - get_x(c2);
    float dist_y = get_y(c1) - get_y(c2);
    float dist_squared = dist_x * dist_x + dist_y * dist_y;
    float raios_sum = get_raio(c1) + get_raio(c2);
    return dist_squared <= raios_sum * raios_sum;
}

bool retangulo_colide_retangulo(Retangulo* r1, Retangulo* r2) {
    return (get_x_retangulo(r1) < get_x_retangulo(r2) + get_largura(r2) &&
            get_x_retangulo(r1) + get_largura(r1) > get_x_retangulo(r2) &&
            get_y_retangulo(r1) < get_y_retangulo(r2) + get_altura(r2) &&
            get_y_retangulo(r1) + get_altura(r1) > get_y_retangulo(r2));
}

bool circulo_colide_retangulo(Circulo* c, Retangulo* r) {
    float cx = get_x(c);
    float cy = get_y(c);
    float rx = get_x_retangulo(r);
    float ry = get_y_retangulo(r);
    float rw = get_largura(r);
    float rh = get_altura(r);

    float closestX = (cx < rx) ? rx : (cx > rx + rw) ? rx + rw : cx;
    float closestY = (cy < ry) ? ry : (cy > ry + rh) ? ry + rh : cy;
    float dx = cx - closestX;
    float dy = cy - closestY;
    return (dx * dx + dy * dy) <= (get_raio(c) * get_raio(c));
}

bool formas_colidem(FormaStruct* f1, FormaStruct* f2) {
    if (f1->tipo == TIPO_CIRCULO && f2->tipo == TIPO_CIRCULO)
        return circulo_colide_circulo(f1->dados_forma, f2->dados_forma);
    if (f1->tipo == TIPO_RETANGULO && f2->tipo == TIPO_RETANGULO)
        return retangulo_colide_retangulo(f1->dados_forma, f2->dados_forma);
    if (f1->tipo == TIPO_CIRCULO && f2->tipo == TIPO_RETANGULO)
        return circulo_colide_retangulo(f1->dados_forma, f2->dados_forma);
    if (f1->tipo == TIPO_RETANGULO && f2->tipo == TIPO_CIRCULO)
        return circulo_colide_retangulo(f2->dados_forma, f1->dados_forma);
    return false;
}

// --- Funções de gerenciamento da Arena ---
Arena criar_arena() {
    ArenaStruct* a = malloc(sizeof(ArenaStruct));
    a->formas_na_arena = iniciar_fila();
    return a;
}

void arena_adicionar_forma(Arena a, FormaStruct* f) {
    ArenaStruct* arena = (ArenaStruct*)a;
    adicionar_na_fila(arena->formas_na_arena, f);
}

void destruir_arena(Arena a) {

    ArenaStruct* arena = (ArenaStruct*)a;
    destruir_fila(arena->formas_na_arena);
    free(arena);
}


static void reportar_dados_forma(FormaStruct* forma, FILE *txt) {
    if (!forma || !txt) return;
    switch (forma->tipo) {
        case TIPO_CIRCULO: {
            Circulo *c = (Circulo *)forma->dados_forma;
            fprintf(txt, "Circulo - ID: %d, Centro: (%.1f,%.1f), Raio: %.1f\n", get_id_circulo(c), get_x(c), get_y(c), get_raio(c));
            break;
        }
        case TIPO_RETANGULO: {
            Retangulo *r = (Retangulo *)forma->dados_forma;
            fprintf(txt, "Retangulo - ID: %d, Posicao: (%.1f,%.1f), Dimensoes: %.1fx%.1f\n", get_id_retangulo(r), get_x_retangulo(r), get_y_retangulo(r), get_largura(r), get_altura(r));
            break;
        }
        case TIPO_LINHA: {
            Linha *l = (Linha *)forma->dados_forma;
            fprintf(txt, "Linha - ID: %d, Pontos: (%.1f,%.1f) a (%.1f,%.1f)\n", get_id_linha(l), get_x1_linha(l), get_y1_linha(l), get_x2_linha(l), get_y2_linha(l));
            break;
        }
        case TIPO_TEXTO: {
            Texto *t = (Texto *)forma->dados_forma;
            fprintf(txt, "Texto - ID: %d, Posicao: (%.1f,%.1f), Conteudo: \"%s\"\n", get_id_texto(t), get_x_texto(t), get_y_texto(t), get_conteudo_texto(t));
            break;
        }
    }
}


void process_qry(FILE *qry, FILE *svg, Ground ground, FILE *txt) {
    fprintf(svg, "<svg xmlns='http://www.w3.org/2000/svg'>\n");
    
    Arena arena = criar_arena();
    Disparador **d = NULL;
    Carregador **c = NULL;
    int disp_conter = 0, car_conter = 0;

    char comando[64];
    while (fscanf(qry, "%63s", comando) == 1) {
        if (strcmp(comando, "pd") == 0) {
            int id; float x, y;
            fscanf(qry, "%d %f %f", &id, &x, &y);
            Disparador *nd = criar_disparador(id, (int)x, (int)y);
            d = realloc(d, (disp_conter + 1) * sizeof(Disparador*));
            d[disp_conter++] = nd;
        } else if (strcmp(comando, "lc") == 0) {
            int id, n;
            fscanf(qry, "%d %d", &id, &n);
            Carregador *nc = criar_carredor(id);
            carregar_carregador(get_ground_fila(ground), nc, n, txt);
            c = realloc(c, (car_conter + 1) * sizeof(Carregador*));
            c[car_conter++] = nc;
        } else if (strcmp(comando, "atch") == 0) {
             int id_disp, id_ce, id_cd;
    fscanf(qry, "%d %d %d", &id_disp, &id_ce, &id_cd);
    Disparador *disp_alvo = NULL;
    for (int i = 0; i < disp_conter; i++) {
        if (disparador_get_id(d[i]) == id_disp) {
            disp_alvo = d[i];
            break;
        }
    }

    if (disp_alvo) {
        Carregador* carregador_esq = NULL;
        Carregador* carregador_dir = NULL;
        int idx_esq = -1, idx_dir = -1;

        
        for (int i = 0; i < car_conter; i++) {
            if (c[i] != NULL) {
                if (carregador_get_id(c[i]) == id_ce) {
                    carregador_esq = c[i];
                    idx_esq = i;
                }
                if (carregador_get_id(c[i]) == id_cd) {
                    carregador_dir = c[i];
                    idx_dir = i;
                }
            }
        }
        
        
        if (carregador_esq) {
            disparador_set_carregador_esq(disp_alvo, carregador_esq);
            if (idx_esq != -1) c[idx_esq] = NULL;
        }
        if (carregador_dir) {
            disparador_set_carregador_dir(disp_alvo, carregador_dir);
            // Se for o mesmo carregador, não anula o ponteiro duas vezes
            if (idx_dir != -1 && idx_dir != idx_esq) c[idx_dir] = NULL;
        }
            }
        } else if (strcmp(comando, "shft") == 0) {
            int id, n; char lado[2];
            fscanf(qry, "%d %1s %d", &id, lado, &n);
            for (int i = 0; i < disp_conter; i++) {
                if (disparador_get_id(d[i]) == id) {
                    carregar_disparador(d[i], n, lado);
                    break;
                }
            }
        }else if (strcmp(comando, "rjd") == 0) {
    int id;
    float dx_inicial, dy_inicial, ix, iy;
    char lado[2];
    fscanf(qry, "%d %1s %f %f %f %f", &id, lado, &dx_inicial, &dy_inicial, &ix, &iy);
    
    Disparador *disp_alvo = NULL;
    for (int i = 0; i < disp_conter; i++) {
        if (disparador_get_id(d[i]) == id) {
            disp_alvo = d[i];
            break;
        }
    }

    if (disp_alvo) {
        int contador_disparos = 0;
        fprintf(txt, "[rjd] Iniciando rajada para disparador ID %d, lado %s\n", id, lado);
        
        while (1) {
            // 1. Equivalente a "shft d [e|d] 1"
            carregar_disparador(disp_alvo, 1, lado);
            
            // 2. Tenta disparar (consome a forma da posição de disparo)
            FormaStruct *forma_disparada = (FormaStruct*)disparador_disparar_forma(disp_alvo);

            // Se não houver mais formas no carregador, para a rajada
            if (!forma_disparada) {
                 fprintf(txt, "\t-> Rajada concluída. Carregador esgotado.\n\n");
                 break;
            }
            
            // 3. Calcula o deslocamento para *este* disparo da rajada
            float dx_atual = dx_inicial + contador_disparos * ix;
            float dy_atual = dy_inicial + contador_disparos * iy;

            // 4. Calcula a posição final da forma
            float x_disp_val = disparador_get_x(disp_alvo);
            float y_disp_val = disparador_get_y(disp_alvo);
            float x_final = x_disp_val + dx_atual;
            float y_final = y_disp_val + dy_atual;

            // 5. Clona a forma disparada e coloca na posição final (Lógica do DSP)
            FormaStruct* forma_na_arena = clonar_forma(forma_disparada, x_final, y_final, NULL, false);
            
            if (forma_na_arena) {
                arena_adicionar_forma(arena, forma_na_arena);
                adicionar_na_fila(get_ground_fila(ground), forma_na_arena);
                
                
                if (txt) {
                    fprintf(txt, "\t- Rajada Disparo %d: Forma ID %d -> Posicao Final (%.1f, %.1f) | Deslocamento (dx:%.1f, dy:%.1f)\n",
                            contador_disparos + 1,
                            forma_disparada->id_original, // Reporta o ID original
                            x_final, y_final, dx_atual, dy_atual);
                   
                }
            } else {
                 if(txt) fprintf(txt, "\t- Rajada Disparo %d: Erro ao clonar forma ID %d\n", contador_disparos + 1, forma_disparada->id_original);
            }
            
            contador_disparos++;
        }
    } else {
        fprintf(txt, "[rjd] Erro: Disparador com ID %d não encontrado.\n", id);
    }
  }  else if (strcmp(comando, "dsp") == 0) {
           int id; float dx, dy; char flag[2];
        fscanf(qry, "%d %f %f %1s", &id, &dx, &dy, flag);
        for (int i = 0; i < disp_conter; i++) {
            if (disparador_get_id(d[i]) == id) {
                // USA A NOVA FUNÇÃO QUE CONSOME A FORMA
                FormaStruct *forma_disparada = (FormaStruct*)disparador_disparar_forma(d[i]);
                if (forma_disparada) {
                    // Cria uma cópia da forma na posição final do disparo
                    float x_final = disparador_get_x(d[i]) + dx;
                    float y_final = disparador_get_y(d[i]) + dy;
                    
                    FormaStruct* forma_na_arena = clonar_forma(forma_disparada, x_final, y_final, NULL, false);
                    
                    if (forma_na_arena) {
                        arena_adicionar_forma(arena, forma_na_arena);
                        adicionar_na_fila(get_ground_fila(ground), forma_na_arena);
                       
                    }
                    
                    if (txt) {
                       fprintf(txt, "Disparo: ID %d -> Posição (%.1f, %.1f)\n", forma_disparada->id_original, x_final, y_final);
                    }
                }
                break;
            }
        }
        } else if (strcmp(comando, "calc") == 0) {
            fprintf(txt, "\n--- Início do Processamento de Colisões (calc) ---\n");

    Fila fila_arena = ((ArenaStruct*)arena)->formas_na_arena;
    // Usa a própria arena como lista temporária de formas a processar
    Fila processados_nesta_rodada = iniciar_fila(); // Guarda formas já processadas nesta rodada

    void* forma_i_ptr;
    // Processa cada forma da arena contra as que já foram processadas nesta rodada
    while(remover_da_fila(fila_arena, &forma_i_ptr)) {
        FormaStruct* forma_i = (FormaStruct*) forma_i_ptr;
        // Se a forma já foi marcada como destruída numa colisão anterior DENTRO desta chamada 'calc', ignora
        if (forma_i->foi_destruida) {
            adicionar_na_fila(processados_nesta_rodada, forma_i); // Adiciona aos processados
            continue;
        }

        bool colidiu_nesta_iteracao = false;

        Fila temp_para_reconstruir_processados = iniciar_fila();
        void* forma_j_ptr;
        // Compara forma_i com todos os processados até agora
        while(remover_da_fila(processados_nesta_rodada, &forma_j_ptr)) {
            FormaStruct* forma_j = (FormaStruct*) forma_j_ptr;

            // Se forma_j também está destruída, não pode colidir
            if (forma_j->foi_destruida) {
                adicionar_na_fila(temp_para_reconstruir_processados, forma_j);
                continue;
            }

            // Se forma_i ainda não colidiu nesta iteração E as formas colidem
            if (!colidiu_nesta_iteracao && formas_colidem(forma_i, forma_j)) {
                colidiu_nesta_iteracao = true; // Marca que forma_i colidiu
                float area_i = calcular_area_forma(forma_i);
                float area_j = calcular_area_forma(forma_j);

                fprintf(txt, "\tColisao: ID %d (Area %.2f) vs ID %d (Area %.2f)\n", forma_i->id_original, area_i, forma_j->id_original, area_j);

                if (area_i >= area_j) {
                    fprintf(txt, "\t-> ID %d sobrevive, ID %d destruido. Clones gerados.\n", forma_i->id_original, forma_j->id_original);
                    forma_j->foi_destruida = true; // J é destruído

                    const char* cor_preenchimento_i = (forma_i->tipo == TIPO_CIRCULO) ? get_corPreenchimento_circulo(forma_i->dados_forma) : get_corPreenchimento_retangulo(forma_i->dados_forma);
                    FormaStruct* clone_j = clonar_forma(forma_j, forma_j->x_centro, forma_j->y_centro, cor_preenchimento_i, false);
                    FormaStruct* clone_i_cores_trocadas = clonar_forma(forma_i, forma_i->x_centro, forma_i->y_centro, NULL, true);

                    // Adiciona NOVOS clones à fila principal
                    if(clone_j) adicionar_na_fila(get_ground_fila(ground), clone_j);
                    if(clone_i_cores_trocadas) adicionar_na_fila(get_ground_fila(ground), clone_i_cores_trocadas);

                    // Adiciona I (sobrevivente) de volta à lista temporária
                    adicionar_na_fila(temp_para_reconstruir_processados, forma_i);
                    // NÃO adiciona J (destruído) de volta

                } else { // area_i < area_j
                    fprintf(txt, "\t-> ID %d sobrevive, ID %d destruido.\n", forma_j->id_original, forma_i->id_original);
                    forma_i->foi_destruida = true; // I é destruído

                    // Adiciona J (sobrevivente) de volta à lista temporária
                     adicionar_na_fila(temp_para_reconstruir_processados, forma_j);
                     // NÃO adiciona I (destruído) de volta
                }
            } else {
                // Se não colidiram, forma_j continua processado/sobrevivente
                 adicionar_na_fila(temp_para_reconstruir_processados, forma_j);
            }
        }
       
        while(remover_da_fila(temp_para_reconstruir_processados, &forma_j_ptr)) {
            adicionar_na_fila(processados_nesta_rodada, forma_j_ptr);
        }
        destruir_fila(temp_para_reconstruir_processados);

        adicionar_na_fila(processados_nesta_rodada, forma_i);
    }

    
    void* ptr_temp;
    while(remover_da_fila(processados_nesta_rodada, &ptr_temp));
    destruir_fila(processados_nesta_rodada);

    fprintf(txt, "--- Fim do Processamento de Colisões ---\n");
        }
    }   


    
    fprintf(svg, "</svg>\n");

   for (int i = 0; i < disp_conter; i++) {
        destruir_disparador(d[i]);
    }
    free(d);

    
    for (int i = 0; i < car_conter; i++) {
        if (c[i] != NULL) {
            destruir_carregador(c[i]);
        }
    }
    free(c);
    
    destruir_arena(arena);
}
