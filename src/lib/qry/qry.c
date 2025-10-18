#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qry.h"
#include "../manipilarArq/arquivo.h"
#include "../disparador/disparador.h"
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../fila/fila.h"

// Helper: safe realloc wrapper
static void *safe_realloc(void *ptr, size_t newSize) {
    void *p = realloc(ptr, newSize);
    if (!p && newSize != 0) {
        return NULL;
    }
    return p;
}

Fila ler_geo_armazenar(FILE *geo,
                       Circulo*** circulos, int *nCirculos,
                       Retangulo*** retangulos, int *nRetangulos,
                       Linha*** linhas, int *nLinhas,
                       Texto*** textos, int *nTextos) {
    if (!geo) return NULL;

    char line[1024];
    Fila f = iniciar_fila();

    // Inicializa retornos
    if (circulos) { *circulos = NULL; *nCirculos = 0; }
    if (retangulos) { *retangulos = NULL; *nRetangulos = 0; }
    if (linhas) { *linhas = NULL; *nLinhas = 0; }
    if (textos) { *textos = NULL; *nTextos = 0; }

    
    while (fgets(line, sizeof(line), geo) != NULL) {

        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\n' || *p == '#') continue; 

        char type = 0;
        int offset = 0;
        if (sscanf(p, " %c%n", &type, &offset) != 1) continue;
        char *rest = p + offset;

        if (type == 'c') {
            int id; float x, y, r;
            char corB[128], corP[128];
            int matched = sscanf(rest, " %d %f %f %f %127s %127s", &id, &x, &y, &r, corB, corP);
            if (matched >= 6 && circulos) {
                Circulo* obj = criar_circulo(x, y, r, corP, corB, id);
                if (obj) {
                    Circulo **arr = safe_realloc(*circulos, ((*nCirculos)+1) * sizeof(Circulo*));
                    if (arr) {
                        arr[*nCirculos] = obj;
                        *circulos = arr;
                        (*nCirculos)++;
                        adicionar_na_fila(f, obj); // <<-- CÓDIGO MOVIDO PARA CÁ
                    } else {
                        liberar_circulo(obj);
                    }
                }
            }

        } else if (type == 'r') {
            int id; float x, y, w, h;
            char corB[128], corP[128];
            int matched = sscanf(rest, " %d %f %f %f %f %127s %127s", &id, &x, &y, &w, &h, corB, corP);
            if (matched >= 7 && retangulos) {
                Retangulo* obj = criar_retangulo(x, y, w, h, corP, corB, id);
                if (obj) {
                    Retangulo **arr = safe_realloc(*retangulos, ((*nRetangulos)+1) * sizeof(Retangulo*));
                    if (arr) {
                        arr[*nRetangulos] = obj;
                        *retangulos = arr;
                        (*nRetangulos)++;
                        adicionar_na_fila(f, obj); // <<-- CÓDIGO MOVIDO PARA CÁ
                    } else {
                        liberar_retangulo(obj);
                    }
                }
            }
        } else if (type == 'l') {
            int id; float x1, y1, x2, y2; char cor[128];
            int matched = sscanf(rest, " %d %f %f %f %f %127s", &id, &x1, &y1, &x2, &y2, cor);
            if (matched >= 6 && linhas) {
                Linha* obj = criar_linha(x1, y1, x2, y2, cor, id);
                if (obj) {
                    Linha **arr = safe_realloc(*linhas, ((*nLinhas)+1) * sizeof(Linha*));
                    if (arr) {
                        arr[*nLinhas] = obj;
                        *linhas = arr;
                        (*nLinhas)++;
                        adicionar_na_fila(f, obj); // <<-- CÓDIGO MOVIDO PARA CÁ
                    } else {
                        liberar_linha(obj);
                    }
                }
            }
        } else if (type == 't') {
            int id; float x, y; char corB[128], corP[128], anchor[16];
            int consumed = 0;
            int matched = sscanf(rest, " %d %f %f %127s %127s %15s %n", &id, &x, &y, corB, corP, anchor, &consumed);
            if (matched >= 6 && textos) {
                char *texto = rest + consumed;
                while (*texto == ' ' || *texto == '\t') texto++;
                size_t len = strlen(texto);
                while (len > 0 && (texto[len-1] == '\n' || texto[len-1] == '\r')) texto[--len] = '\0';
                if (!texto) texto = "";
                Texto* obj = criar_texto(x, y, corP, texto, NULL, id);
                if (obj) {
                    Texto **arr = safe_realloc(*textos, ((*nTextos)+1) * sizeof(Texto*));
                    if (arr) {
                        arr[*nTextos] = obj;
                        *textos = arr;
                        (*nTextos)++;
                        adicionar_na_fila(f, obj); // <<-- CÓDIGO MOVIDO PARA CÁ
                    } else {
                        liberar_texto(obj);
                    }
                }
            }
        } else {
            continue;
        }
    }
    
    return f;
}


// Funções auxiliares (obter_coordenadas_segmento_texto, segmentos_se_sobrepoem, etc.)
// ... (mantenha as suas funções auxiliares como estavam) ...
static void obter_coordenadas_segmento_texto(Texto *t, float *x1, float *y1, float *x2, float *y2) {
    if (!t) {
        *x1 = *y1 = *x2 = *y2 = 0.0f;
        return;
    }
    float xt = get_x_texto(t);
    float yt = get_y_texto(t);
    const char *conteudo = get_conteudo_texto(t);
    int tamanho = conteudo ? strlen(conteudo) : 0;
    float comprimento = 10.0f * tamanho;
    *x1 = xt;
    *y1 = yt;
    *x2 = xt + comprimento;
    *y2 = yt;
}

static int segmentos_se_sobrepoem(float x1a, float y1a, float x2a, float y2a,
                                  float x1b, float y1b, float x2b, float y2b) {
    (void)y2a;
    (void)y2b;
    if (y1a != y1b) return 0;
    float max_x1 = (x1a > x1b) ? x1a : x1b;
    float min_x2 = (x2a < x2b) ? x2a : x2b;
    return max_x1 < min_x2;
}

static void reportar_dados_forma(void *forma, FILE *txt) {
    if (!forma || !txt) return;
    char *tipo = NULL;
    // AVISO: Este método para obter o tipo da forma é instável.
    // O ideal seria ter um campo 'tipo' na própria struct.
    memcpy(&tipo, (char *)forma + sizeof(int), sizeof(char *));
    
    if (tipo && strlen(tipo) > 0) {
        if (strcmp(tipo, "Circulo") == 0) {
            Circulo *c = (Circulo *)forma;
            fprintf(txt, "Círculo - ID: %d, Centro: (%.1f,%.1f), Raio: %.1f, Cor: %s\n",
                    get_id_circulo(c), get_x(c), get_y(c), get_raio(c), get_corBorda_circulo(c));
        } else if (strcmp(tipo, "Retangulo") == 0) {
            Retangulo *r = (Retangulo *)forma;
            fprintf(txt, "Retângulo - ID: %d, Posição: (%.1f,%.1f), Dimensões: %.1fx%.1f, Cor: %s\n",
                    get_id_retangulo(r), get_x_retangulo(r), get_y_retangulo(r), 
                    get_largura(r), get_altura(r), get_corBorda_retangulo(r));
        } else if (strcmp(tipo, "Linha") == 0) {
            Linha *l = (Linha *)forma;
            fprintf(txt, "Linha - ID: %d, Pontos: (%.1f,%.1f) a (%.1f,%.1f), Cor: %s\n",
                    get_id_linha(l), get_x1_linha(l), get_y1_linha(l), 
                    get_x2_linha(l), get_y2_linha(l), get_cor_linha(l));
        } else if (strcmp(tipo, "Texto") == 0) {
            Texto *t = (Texto *)forma;
            fprintf(txt, "Texto - ID: %d, Posição: (%.1f,%.1f), Conteúdo: \"%s\", Cor: %s\n",
                    get_id_texto(t), get_x_texto(t), get_y_texto(t), 
                    get_conteudo_texto(t), get_cor_texto(t));
        }
    }
}

void calcular_sobreps(Texto** textos, int nTextos, FILE *log) {
    if (!textos || nTextos <= 1) return;
    int sobreposicoes_encontradas = 0;
    for (int i = 0; i < nTextos; i++) {
        for (int j = i + 1; j < nTextos; j++) {
            if (!textos[i] || !textos[j]) continue;
            float x1a, y1a, x2a, y2a;
            float x1b, y1b, x2b, y2b;
            obter_coordenadas_segmento_texto(textos[i], &x1a, &y1a, &x2a, &y2a);
            obter_coordenadas_segmento_texto(textos[j], &x1b, &y1b, &x2b, &y2b);
            if (segmentos_se_sobrepoem(x1a, y1a, x2a, y2a, x1b, y1b, x2b, y2b)) {
                sobreposicoes_encontradas++;
                if (log) {
                    int id1 = get_id_texto(textos[i]);
                    int id2 = get_id_texto(textos[j]);
                    const char *conteudo1 = get_conteudo_texto(textos[i]);
                    const char *conteudo2 = get_conteudo_texto(textos[j]);
                    fprintf(log, "Sobreposição encontrada entre textos:\n");
                    fprintf(log, "  Texto %d (\"%s\") - segmento: (%.1f,%.1f) a (%.1f,%.1f)\n", 
                            id1, conteudo1 ? conteudo1 : "", x1a, y1a, x2a, y2a);
                    fprintf(log, "  Texto %d (\"%s\") - segmento: (%.1f,%.1f) a (%.1f,%.1f)\n", 
                            id2, conteudo2 ? conteudo2 : "", x1b, y1b, x2b, y2b);
                    fprintf(log, "\n");
                }
            }
        }
    }
    if (log) {
        fprintf(log, "Total de sobreposições encontradas: %d\n", sobreposicoes_encontradas);
    }
}


// =======================================================================
// FUNÇÃO PROCESS_QRY CORRIGIDA
// =======================================================================
void process_qry(FILE *qry, FILE *svg, FILE *geo, FILE *txt)
{
    // Início do SVG
    fprintf(svg, "<svg xmlns='http://www.w3.org/2000/svg'>\n");

    Disparador **d = NULL;
    Carregador **c = NULL;
    int id, n, disp_conter = 0, car_conter = 0, i, k, j;
    float x_disp, y_disp;
    char comando[64];
    Circulo** circulo = NULL;
    int nCirculos = 0;
    Retangulo** retangulos = NULL;
    int nRetangulos = 0;
    Linha** linhas = NULL;
    int nLinhas = 0;
    Texto** textos = NULL;
    int nTextos = 0;
    Fila ordem;

    ordem = ler_geo_armazenar(geo, &circulo, &nCirculos, &retangulos, &nRetangulos, &linhas, &nLinhas, &textos, &nTextos);

    // =======================================================================
    // INÍCIO DO CÓDIGO ADICIONADO: Desenhar todas as formas lidas do .geo
    // =======================================================================

    fprintf(svg, "\n");

    // Desenha todos os círculos
    for (i = 0; i < nCirculos; i++) {
        if (circulo[i]) {
            // NOTA: Assumi que existe uma função get_corPreenchimento_circulo. Se o nome for outro, ajuste-o.
            fprintf(svg, "<circle cx='%.1f' cy='%.1f' r='%.1f' stroke='%s' fill='%s' />\n",
                    get_x(circulo[i]),
                    get_y(circulo[i]),
                    get_raio(circulo[i]),
                    get_corBorda_circulo(circulo[i]),
                    get_corPreenchimento_circulo(circulo[i])); 
        }
    }

    // Desenha todos os retângulos
    for (i = 0; i < nRetangulos; i++) {
        if (retangulos[i]) {
            // NOTA: Assumi que existe uma função get_corPreenchimento_retangulo. Se o nome for outro, ajuste-o.
            fprintf(svg, "<rect x='%.1f' y='%.1f' width='%.1f' height='%.1f' stroke='%s' fill='%s' />\n",
                    get_x_retangulo(retangulos[i]),
                    get_y_retangulo(retangulos[i]),
                    get_largura(retangulos[i]),
                    get_altura(retangulos[i]),
                    get_corBorda_retangulo(retangulos[i]),
                    get_corPreenchimento_retangulo(retangulos[i]));
        }
    }

    // Desenha todas as linhas
    for (i = 0; i < nLinhas; i++) {
        if (linhas[i]) {
            fprintf(svg, "<line x1='%.1f' y1='%.1f' x2='%.1f' y2='%.1f' stroke='%s' stroke-width='2' />\n",
                    get_x1_linha(linhas[i]),
                    get_y1_linha(linhas[i]),
                    get_x2_linha(linhas[i]),
                    get_y2_linha(linhas[i]),
                    get_cor_linha(linhas[i]));
        }
    }

    // Desenha todos os textos
    for (i = 0; i < nTextos; i++) {
        if (textos[i]) {
            fprintf(svg, "<text x='%.1f' y='%.1f' fill='%s' >%s</text>\n",
                    get_x_texto(textos[i]),
                    get_y_texto(textos[i]),
                    get_cor_texto(textos[i]),
                    get_conteudo_texto(textos[i]));
        }
    }

    fprintf(svg, "\n\n");

    // =======================================================================
    // FIM DO CÓDIGO ADICIONADO
    // =======================================================================


    while (fscanf(qry, "%63s", comando) == 1)
    {
        if (strcmp(comando, "pd") == 0)
        {
            fscanf(qry, "%d %f %f", &id, &x_disp, &y_disp);
            Disparador *nd = criar_disparador(id, (int)x_disp, (int)y_disp);
            if (nd) {
                Disparador **tmp = (Disparador**) safe_realloc(d, (disp_conter+1) * sizeof(Disparador*));
                if (tmp) {
                    tmp[disp_conter] = nd;
                    d = tmp;
                    disp_conter++;
                } else {
                    destruir_disparador(nd);
                }
            }
        }
        else if (strcmp(comando, "lc") == 0)
        {
            fscanf(qry, "%d %d", &id, &n);
            Carregador *nc = criar_carredor(id);
            if (nc) {
                Carregador **tmpc = (Carregador**) safe_realloc(c, (car_conter+1) * sizeof(Carregador*));
                if (tmpc) {
                    tmpc[car_conter] = nc;
                    c = tmpc;
                    car_conter++;
                    carregar_carregador(ordem, c[car_conter - 1], n,txt);
                } else {
                    carregador_destruir(nc);
                }
            }
        }
        else if (strcmp(comando, "atch") == 0)
        {
            fscanf(qry, "%d %d %d", &id, &n, &k);
            for (i = 0; i < disp_conter; i++) {
                if (disparador_get_id(d[i]) == id) {
                    Carregador *ce = NULL;
                    Carregador *cd = NULL;
                    for (j = 0; j < car_conter; j++) {
                        int cid = carregador_get_id(c[j]);
                        if (cid == n) ce = c[j];
                        if (cid == k) cd = c[j];
                    }
                    if (ce) disparador_set_carregador_esq(d[i], ce);
                    if (cd) disparador_set_carregador_dir(d[i], cd);
                    break;
                }
            }
        } else if (strcmp(comando,"shft") == 0)
        {
            char lado[2];
            fscanf(qry, "%d %1s %d", &id, lado, &n);
            
            for (i = 0; i < disp_conter; i++) {
                if (disparador_get_id(d[i]) == id) {
                    carregar_disparador(d[i], n, lado);
                    void *forma_disparo = disparador_obter_forma_disparo(d[i]);
                    if (forma_disparo && txt) {
                        fprintf(txt, "Forma no ponto de disparo após shft:\n");
                        reportar_dados_forma(forma_disparo, txt);
                        fprintf(txt, "\n");
                    }
                    break;
                }
            }
        }
        else if (strcmp(comando, "dsp") == 0)
        {
            float dx, dy;
            char flag[2] = "";
            fscanf(qry, "%d %f %f %1s", &id, &dx, &dy, flag);
            
            for (i = 0; i < disp_conter; i++) {
                if (disparador_get_id(d[i]) == id) {
                    void *forma = disparador_obter_forma_disparo(d[i]);
                    if (forma) {
                        float x_disp_val = disparador_get_x(d[i]);
                        float y_disp_val = disparador_get_y(d[i]);
                        float x_final = x_disp_val + dx;
                        float y_final = y_disp_val + dy;
                        
                        if (txt) {
                            fprintf(txt, "Disparo realizado:\n");
                            reportar_dados_forma(forma, txt);
                            fprintf(txt, "Posição final: (%.1f,%.1f)\n\n", x_final, y_final);
                        }
                        
                        if (strcmp(flag, "v") == 0 && svg) {
                            fprintf(svg, "<line x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" stroke=\"blue\" stroke-width=\"2\" stroke-dasharray=\"5,5\"/>\n",
                                    x_disp_val, y_disp_val, x_final, y_final);
                        }
                    }
                    break;
                }
            }
        }
        else if (strcmp(comando, "rjd") == 0)
        {
            char lado[2];
            float dx, dy, ix, iy;
            fscanf(qry, "%d %1s %f %f %f %f", &id, lado, &dx, &dy, &ix, &iy);
            
            for (i = 0; i < disp_conter; i++) {
                if (disparador_get_id(d[i]) == id) {
                    int contador_disparos = 0;
                    while (1) {
                        carregar_disparador(d[i], 1, lado);
                        void *forma = disparador_obter_forma_disparo(d[i]);
                        if (!forma) break;
                        
                        float x_disp_val = disparador_get_x(d[i]);
                        float y_disp_val = disparador_get_y(d[i]);
                        float x_final = x_disp_val + dx + contador_disparos * ix;
                        float y_final = y_disp_val + dy + contador_disparos * iy;
                        
                        if (txt) {
                            fprintf(txt, "Rajada - Disparo %d:\n", contador_disparos + 1);
                            reportar_dados_forma(forma, txt);
                            fprintf(txt, "Posição final: (%.1f,%.1f)\n\n", x_final, y_final);
                        }
                        contador_disparos++;
                    }
                    break;
                }
            }
        }
        else if (strcmp(comando, "calc") == 0)
        {
            if (nTextos > 0) {
                calcular_sobreps(textos, nTextos, txt);
            }
            if (txt) {
                fprintf(txt, "Processamento de sobreposições concluído.\n");
            }
        }
    }

    // Limpeza de memória
    for (i = 0; i < disp_conter; i++) destruir_disparador(d[i]);
    free(d);
    
    for (i = 0; i < car_conter; i++) carregador_destruir(c[i]);
    free(c);
    
    // Libera formas (A fila não tem posse dos ponteiros, então eles devem ser liberados aqui)
    for (i = 0; i < nCirculos; i++) liberar_circulo(circulo[i]);
    free(circulo);
    
    for (i = 0; i < nRetangulos; i++) liberar_retangulo(retangulos[i]);
    free(retangulos);
    
    for (i = 0; i < nLinhas; i++) liberar_linha(linhas[i]);
    free(linhas);
    
    for (i = 0; i < nTextos; i++) liberar_texto(textos[i]);
    free(textos);
    
    if (ordem) {
        destruir_fila(ordem);
    }

    // Fim do SVG
    fprintf(svg, "</svg>\n");
}
