#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "qry.h"
#include "../manipilarArq/arquivo.h"
#include "../disparador/disparador.h"
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../fila/fila.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    int id_original;
    TipoForma tipo;
    void* dados_forma;
    bool foi_destruida;
    bool foi_clonada;
    float x_centro, y_centro;
} FormaStruct;

typedef struct {
    FormaStruct** formas;
    int nFormas;
    int capacidade;
} ArenaStruct;

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

int arena_get_nFormas(Arena a) {
    return ((ArenaStruct*)a)->nFormas;
}

Forma arena_get_forma(Arena a, int index) {
    if (index < 0 || index >= ((ArenaStruct*)a)->nFormas) return NULL;
    return ((ArenaStruct*)a)->formas[index];
}

int forma_get_id(Forma f) { return ((FormaStruct*)f)->id_original; }
TipoForma forma_get_tipo(Forma f) { return ((FormaStruct*)f)->tipo; }
void* forma_get_dados(Forma f) { return ((FormaStruct*)f)->dados_forma; }
bool forma_foi_destruida(Forma f) { return ((FormaStruct*)f)->foi_destruida; }
float forma_get_x_centro(Forma f) { return ((FormaStruct*)f)->x_centro; }
float forma_get_y_centro(Forma f) { return ((FormaStruct*)f)->y_centro; }
void forma_set_destruida(Forma f, bool status) { ((FormaStruct*)f)->foi_destruida = status; }


// =======================================================================
// SEÇÃO DE FUNÇÕES AUXILIARES PARA A LÓGICA DO JOGO (calc)
// =======================================================================

// --- Funções de Colisão ---
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

// --- Gerenciamento da Arena ---
Arena criar_arena() {
    ArenaStruct* a = malloc(sizeof(ArenaStruct));
    a->capacidade = 16;
    a->nFormas = 0;
    a->formas = malloc(a->capacidade * sizeof(FormaStruct*));
    return a;
}

void arena_adicionar_forma(Arena a, Forma f) {
    ArenaStruct* arena = (ArenaStruct*)a;
    if (arena->nFormas >= arena->capacidade) {
        arena->capacidade *= 2;
        arena->formas = realloc(arena->formas, arena->capacidade * sizeof(FormaStruct*));
    }
    arena->formas[arena->nFormas++] = f;
}

void destruir_arena(Arena a) {
    ArenaStruct* arena = (ArenaStruct*)a;
    if (!arena) return;
    for (int i = 0; i < arena->nFormas; i++) {
        FormaStruct* forma = arena->formas[i];
        if (forma->foi_clonada) {
            switch(forma->tipo) {
                case TIPO_CIRCULO:   liberar_circulo(forma->dados_forma);   break;
                case TIPO_RETANGULO: liberar_retangulo(forma->dados_forma); break;
                default: break;
            }
        }
        free(forma);
    }
    free(arena->formas);
    free(arena);
}

// --- Funções Genéricas para Formas ---
bool formas_colidem(Forma f1, Forma f2) {
    TipoForma tipo1 = forma_get_tipo(f1);
    TipoForma tipo2 = forma_get_tipo(f2);
    void* dados1 = forma_get_dados(f1);
    void* dados2 = forma_get_dados(f2);

    if (tipo1 == TIPO_CIRCULO && tipo2 == TIPO_CIRCULO)
        return circulo_colide_circulo(dados1, dados2);
    if (tipo1 == TIPO_RETANGULO && tipo2 == TIPO_RETANGULO)
        return retangulo_colide_retangulo(dados1, dados2);
    if (tipo1 == TIPO_CIRCULO && tipo2 == TIPO_RETANGULO)
        return circulo_colide_retangulo(dados1, dados2);
    if (tipo1 == TIPO_RETANGULO && tipo2 == TIPO_CIRCULO)
        return circulo_colide_retangulo(dados2, dados1);
    return false;
}

float forma_get_area(Forma f) {
    if (!f || !forma_get_dados(f)) return 0.0f;
    switch (forma_get_tipo(f)) {
        case TIPO_CIRCULO:   return area_circulo(forma_get_dados(f));
        case TIPO_RETANGULO: return area_retangulo(forma_get_dados(f));
        default:             return 0.0f;
    }
}

Forma clonar_forma(Forma original_f, int novo_id) {
    FormaStruct* original = (FormaStruct*) original_f;
    FormaStruct* clone = malloc(sizeof(FormaStruct));
    clone->tipo = original->tipo;
    clone->foi_destruida = false;
    clone->foi_clonada = true;
    clone->x_centro = original->x_centro;
    clone->y_centro = original->y_centro;
    clone->id_original = novo_id;

    switch(original->tipo) {
        case TIPO_CIRCULO: {
            Circulo* orig_c = (Circulo*)original->dados_forma;
            clone->dados_forma = criar_circulo(get_x(orig_c), get_y(orig_c), get_raio(orig_c),
                                             (char*)get_corPreenchimento_circulo(orig_c), (char*)get_corBorda_circulo(orig_c), novo_id);
            break;
        }
        case TIPO_RETANGULO: {
            Retangulo* orig_r = (Retangulo*)original->dados_forma;
            clone->dados_forma = criar_retangulo(get_x_retangulo(orig_r), get_y_retangulo(orig_r), get_largura(orig_r), get_altura(orig_r),
                                               (char*)get_corPreenchimento_retangulo(orig_r), (char*)get_corBorda_retangulo(orig_r), novo_id);
            break;
        }
        default:
            free(clone);
            return NULL;
    }
    return clone;
}

void process_qry(FILE *qry, FILE *svg, FILE *geo, FILE *txt)
{
    // Início do SVG
    fprintf(svg, "<svg xmlns='http://www.w3.org/2000/svg'>\n");

   Disparador **d = NULL;
    Carregador **c = NULL;
    int id, n, disp_conter = 0, car_conter = 0, i, k, j;
    float x_disp, y_disp;
    char comando[64];
    
    Circulo** circulos_originais = NULL;     int nCirculos = 0;
    Retangulo** retangulos_originais = NULL; int nRetangulos = 0;
    Linha** linhas_originais = NULL;         int nLinhas = 0;
    Texto** textos_originais = NULL;         int nTextos = 0;
    Fila ordem_carregamento;

    Arena arena = criar_arena();
    int proximo_id_clone = 10000;

    ordem_carregamento = ler_geo_armazenar(geo, &circulos_originais, &nCirculos, &retangulos_originais, &nRetangulos, &linhas_originais, &nLinhas, &textos_originais, &nTextos);


    fprintf(svg, "\n");

    // Desenha todos os círculos
    for (i = 0; i < nCirculos; i++) {
        if (circulos_originais[i]) {
            // NOTA: Assumi que existe uma função get_corPreenchimento_circulo. Se o nome for outro, ajuste-o.
            fprintf(svg, "<circle cx='%.1f' cy='%.1f' r='%.1f' stroke='%s' fill='%s' />\n",
                    get_x(circulos_originais[i]),
                    get_y(circulos_originais[i]),
                    get_raio(circulos_originais[i]),
                    get_corBorda_circulo(circulos_originais[i]),
                    get_corPreenchimento_circulo(circulos_originais[i])); 
        }
    }

    // Desenha todos os retângulos
    for (i = 0; i < nRetangulos; i++) {
        if (retangulos_originais[i]) {
            fprintf(svg, "<rect x='%.1f' y='%.1f' width='%.1f' height='%.1f' stroke='%s' fill='%s' />\n",
                    get_x_retangulo(retangulos_originais[i]),
                    get_y_retangulo(retangulos_originais[i]),
                    get_largura(retangulos_originais[i]),
                    get_altura(retangulos_originais[i]),
                    get_corBorda_retangulo(retangulos_originais[i]),
                    get_corPreenchimento_retangulo(retangulos_originais[i]));
        }
    }

    // Desenha todas as linhas
    for (i = 0; i < nLinhas; i++) {
        if (linhas_originais[i]) {
            fprintf(svg, "<line x1='%.1f' y1='%.1f' x2='%.1f' y2='%.1f' stroke='%s' stroke-width='2' />\n",
                    get_x1_linha(linhas_originais[i]),
                    get_y1_linha(linhas_originais[i]),
                    get_x2_linha(linhas_originais[i]),  
                    get_y2_linha(linhas_originais[i]),
                    get_cor_linha(linhas_originais[i]));
        }
    }

    // Desenha todos os textos
    for (i = 0; i < nTextos; i++) {
        if (textos_originais[i]) {
            fprintf(svg, "<text x='%.1f' y='%.1f' fill='%s' >%s</text>\n",
                    get_x_texto(textos_originais[i]),
                    get_y_texto(textos_originais[i]),
                    get_cor_texto(textos_originais[i]),
                    get_conteudo_texto(textos_originais[i]));
        }
    }

    fprintf(svg, "\n\n");

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
                    carregar_carregador( ordem_carregamento, c[car_conter - 1], n,txt);
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
          fprintf(txt, "\n--- Início do Processamento de Colisões (calc) ---\n");
            double area_esmagada_total = 0.0;
            int formas_no_inicio_rodada = arena_get_nFormas(arena);

            for (i = 0; i < formas_no_inicio_rodada; i++) {
                for (j = i + 1; j < formas_no_inicio_rodada; j++) {
                    Forma f_i = arena_get_forma(arena, i);
                    Forma f_j = arena_get_forma(arena, j);

                    if (forma_foi_destruida(f_i) || forma_foi_destruida(f_j)) continue;

                    if (formas_colidem(f_i, f_j)) {
                        float area_i = forma_get_area(f_i);
                        float area_j = forma_get_area(f_j);

                        fprintf(txt, "Colisão: ID %d (área %.2f) vs ID %d (área %.2f)\n", forma_get_id(f_i), area_i, forma_get_id(f_j), area_j);

                        if (area_i > area_j) {
                            forma_set_destruida(f_j, true);
                            area_esmagada_total += area_j;
                            fprintf(txt, "  -> ID %d esmagou ID %d.\n", forma_get_id(f_i), forma_get_id(f_j));
                        } else { 
                            forma_set_destruida(f_i, true);
                            area_esmagada_total += area_i;
                            fprintf(txt, "  -> ID %d esmagou ID %d.\n", forma_get_id(f_j), forma_get_id(f_i));

                            Forma clone = clonar_forma(f_i, proximo_id_clone++);
                            
                            if (forma_get_tipo(clone) == TIPO_CIRCULO) {
                                set_corBorda_circulo(forma_get_dados(clone), get_corBorda_circulo(forma_get_dados(f_j)));
                                set_corPreenchimento_circulo(forma_get_dados(clone), get_corPreenchimento_circulo(forma_get_dados(f_j)));
                            } else if (forma_get_tipo(clone) == TIPO_RETANGULO) {
                                set_corBorda_retangulo(forma_get_dados(clone), get_corBorda_retangulo(forma_get_dados(f_j)));
                                set_corPreenchimento_retangulo(forma_get_dados(clone), get_corPreenchimento_retangulo(forma_get_dados(f_j)));
                            }
                            arena_adicionar_forma(arena, clone);
                            fprintf(txt, "  -> ID %d foi clonado (novo ID %d) com as cores de ID %d.\n", forma_get_id(f_i), forma_get_id(clone), forma_get_id(f_j));
                        }
                    }
                }
            }
            fprintf(txt, "Área total esmagada: %.2f\n", round(area_esmagada_total));
            fprintf(txt, "--- Fim do Processamento de Colisões ---\n");

            fprintf(svg, "\n\n");
            for (i = 0; i < arena_get_nFormas(arena); i++) {
                Forma f = arena_get_forma(arena, i);
                if (forma_foi_destruida(f)) {
                    fprintf(svg, "<text x='%.1f' y='%.1f' fill='red' font-size='20' text-anchor='middle' dominant-baseline='central'>*</text>\n",
                            forma_get_x_centro(f), forma_get_y_centro(f));
                }
            }
        }
        }

    // Limpeza de memória
    for (i = 0; i < disp_conter; i++) destruir_disparador(d[i]);
    free(d);
        
    for (i = 0; i < car_conter; i++) carregador_destruir(c[i]);
    free(c);
    
    // Libera formas (A fila não tem posse dos ponteiros, então eles devem ser liberados aqui)
    for (i = 0; i < nCirculos; i++) liberar_circulo(circulos_originais[i]);
    free(circulos_originais);
    
    for (i = 0; i < nRetangulos; i++) liberar_retangulo(retangulos_originais[i]);
    free(retangulos_originais);
    
    for (i = 0; i < nLinhas; i++) liberar_linha(linhas_originais[i]);
    free(linhas_originais);
    
    for (i = 0; i < nTextos; i++) liberar_texto(textos_originais[i]);
    free(textos_originais);
    
    if (ordem_carregamento) {
        destruir_fila(ordem_carregamento);
    }

    // Fim do SVG
    fprintf(svg, "</svg>\n");
}
