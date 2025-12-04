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

typedef struct
{
    int id_original;
    TipoForma tipo;
    void *dados_forma;
    bool foi_destruida;
    bool foi_clonada;
    float x_landed;
    float y_landed;
} FormaStruct;

static int proximo_id_clone = 10000; 
static int total_disparos = 0;
static int total_formas_esmagadas = 0;
static int total_formas_clonadas = 0;
static double pontuacao_final = 0.0;

static float calcular_area_forma(FormaStruct *f)
{
    if (!f || !f->dados_forma)
        return 0.0f;
    switch (f->tipo)
    {
    case TIPO_CIRCULO: return area_circulo(f->dados_forma);
    case TIPO_RETANGULO: return area_retangulo(f->dados_forma);
    case TIPO_LINHA: return area_linha(f->dados_forma);
    case TIPO_TEXTO: return area_texto(f->dados_forma);
    default: return 0.0f;
    }
}

TipoForma forma_get_tipo(Forma forma)
{
    if (!forma) return (TipoForma)-1;
    return ((FormaStruct *)forma)->tipo;
}
int forma_get_id_original(Forma forma)
{
    if (!forma) return -1;
    return ((FormaStruct *)forma)->id_original;
}

void forma_set_destruida(Forma forma, bool status)
{
    if (!forma) return;
    ((FormaStruct *)forma)->foi_destruida = status;
}

static FormaStruct *clonar_forma(FormaStruct *original, float x, float y, const char *nova_cor_borda, bool trocar_cores)
{
    if (!original || !original->dados_forma) return NULL;

    int id_clone = proximo_id_clone++; 
    
    void *dados_clonados = NULL;

    switch (original->tipo)
    {
    case TIPO_CIRCULO:
    {
        Circulo *orig_c = (Circulo *)original->dados_forma;
        const char *cor_b_orig = get_corBorda_circulo(orig_c);
        const char *cor_p_orig = get_corPreenchimento_circulo(orig_c);
        const char *cor_b = trocar_cores ? cor_p_orig : (nova_cor_borda ? nova_cor_borda : cor_b_orig);
        const char *cor_p = trocar_cores ? cor_b_orig : cor_p_orig;
        dados_clonados = criar_circulo(x, y, get_raio(orig_c), (char *)cor_p, (char *)cor_b, id_clone);
        break;
    }
    case TIPO_RETANGULO:
    {
        Retangulo *orig_r = (Retangulo *)original->dados_forma;
        const char *cor_b_orig = get_corBorda_retangulo(orig_r);
        const char *cor_p_orig = get_corPreenchimento_retangulo(orig_r);
        const char *cor_b = trocar_cores ? cor_p_orig : (nova_cor_borda ? nova_cor_borda : cor_b_orig);
        const char *cor_p = trocar_cores ? cor_b_orig : cor_p_orig;
        dados_clonados = criar_retangulo(x, y, get_largura(orig_r), get_altura(orig_r), (char *)cor_p, (char *)cor_b, id_clone);
        break;
    }
    case TIPO_LINHA:
    {
        Linha *orig_l = (Linha *)original->dados_forma;
        const char *cor_original = get_cor_linha(orig_l);
        const char *nova_cor = nova_cor_borda ? nova_cor_borda : cor_original;
        float x1_orig = get_x1_linha(orig_l);
        float y1_orig = get_y1_linha(orig_l);
        float x2_orig = get_x2_linha(orig_l);
        float y2_orig = get_y2_linha(orig_l);
        float dx = x2_orig - x1_orig;
        float dy = y2_orig - y1_orig;
        dados_clonados = criar_linha(x, y, x + dx, y + dy, (char *)nova_cor, id_clone);
        break;
    }
    case TIPO_TEXTO:
    {
        Texto *orig_t = (Texto *)original->dados_forma;
        const char *orig_borda = get_corBorda_texto(orig_t);
        const char *orig_preench = get_corPreenchimento_texto(orig_t);
        const char *orig_conteudo = get_conteudo_texto(orig_t);
        const char *orig_fonte = get_fonte_texto(orig_t);
        char orig_anchor = get_anchor_texto(orig_t);

        const char *cor_b = trocar_cores ? orig_preench : (nova_cor_borda ? nova_cor_borda : orig_borda);
        const char *cor_p = trocar_cores ? orig_borda : orig_preench;

        dados_clonados = criar_texto(x, y, (char *)cor_b, (char *)cor_p, orig_anchor, (char *)orig_conteudo, (char*)orig_fonte, id_clone);
        break;
    }
    }

    if (!dados_clonados) return NULL;

    FormaStruct *wrapper_clone = malloc(sizeof(FormaStruct));
    if (!wrapper_clone) return NULL;

    wrapper_clone->id_original = id_clone;
    wrapper_clone->tipo = original->tipo;
    wrapper_clone->dados_forma = dados_clonados;
    wrapper_clone->foi_clonada = true;
    wrapper_clone->foi_destruida = false;
    wrapper_clone->x_landed = x;
    wrapper_clone->y_landed = y;
    total_formas_clonadas++;
    return wrapper_clone;
}

typedef struct { Fila formas_na_arena; } ArenaStruct;

Arena criar_arena()
{
    ArenaStruct *a = malloc(sizeof(ArenaStruct));
    if (!a) return NULL;
    a->formas_na_arena = iniciar_fila();
    return a;
}

void arena_adicionar_forma(Arena a, Forma forma)
{
    ArenaStruct *arena = (ArenaStruct *)a;
    if (arena && arena->formas_na_arena && forma)
        adicionar_na_fila(arena->formas_na_arena, forma);
}

void destruir_arena(Arena a)
{
    if (!a) return;
    ArenaStruct *arena = (ArenaStruct *)a;
    if (arena->formas_na_arena) destruir_fila(arena->formas_na_arena);
    free(a);
}

typedef struct { float minX, minY, maxX, maxY; } Aabb;

static Aabb make_aabb_for_shape(FormaStruct *s)
{
    Aabb box = {0, 0, 0, 0};
    if (!s || !s->dados_forma) return box;

    float landed_x = s->x_landed;
    float landed_y = s->y_landed;

    switch (s->tipo)
    {
    case TIPO_CIRCULO: {
        float r = get_raio((Circulo *)s->dados_forma);
        box.minX = landed_x - r; box.maxX = landed_x + r;
        box.minY = landed_y - r; box.maxY = landed_y + r;
        break;
    }
    case TIPO_RETANGULO: {
        float w = get_largura((Retangulo *)s->dados_forma);
        float h = get_altura((Retangulo *)s->dados_forma);
        box.minX = landed_x; box.maxX = landed_x + w;
        box.minY = landed_y; box.maxY = landed_y + h;
        break;
    }
    case TIPO_TEXTO: {
        Texto *t = (Texto *)s->dados_forma;
        const char *txt_content = get_conteudo_texto(t);
        int len = txt_content ? (int)strlen(txt_content) : 0;
        float segLen = 10.0f * (float)len;
        char anchor = get_anchor_texto(t);
        float x1 = landed_x, x2 = landed_x;
        if (anchor == 'i' || anchor == 'I' || anchor == 's' || anchor == 'S') x2 = landed_x + segLen;
        else if (anchor == 'f' || anchor == 'F' || anchor == 'e' || anchor == 'E') x1 = landed_x - segLen;
        else if (anchor == 'm' || anchor == 'M') { x1 = landed_x - segLen * 0.5f; x2 = landed_x + segLen * 0.5f; }
        else x2 = landed_x + segLen;
        box.minX = (x1 < x2 ? x1 : x2) - 1.0f;
        box.maxX = (x1 > x2 ? x1 : x2) + 1.0f;
        box.minY = landed_y - 1.0f;
        box.maxY = landed_y + 1.0f;
        break;
    }
    case TIPO_LINHA: {
        Linha *l = (Linha *)s->dados_forma;
        float x1 = get_x1_linha(l), y1 = get_y1_linha(l);
        float x2 = get_x2_linha(l), y2 = get_y2_linha(l);
        box.minX = (x1 < x2 ? x1 : x2) - 1.0f;
        box.maxX = (x1 > x2 ? x1 : x2) + 1.0f;
        box.minY = (y1 < y2 ? y1 : y2) - 1.0f;
        box.maxY = (y1 > y2 ? y1 : y2) + 1.0f;
        break;
    }
    }
    return box;
}

static bool aabb_overlap(Aabb a, Aabb b)
{
    if (a.maxX < b.minX || b.maxX < a.minX || a.maxY < b.minY || b.maxY < a.minY) return false;
    return true;
}

bool formas_colidem(FormaStruct *f1, FormaStruct *f2)
{
    if (!f1 || !f2) return false;
    Aabb a = make_aabb_for_shape(f1);
    Aabb b = make_aabb_for_shape(f2);
    return aabb_overlap(a, b);
}

void process_qry(FILE *qry, FILE *svg, Ground ground, FILE *txt)
{
    if (!svg || !ground || !txt) return;
    
    fprintf(svg, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    fprintf(svg, "<svg xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns=\"http://www.w3.org/2000/svg\" width=\"1000\" height=\"1000\">\n");
    fprintf(svg, "<g>\n");

    Arena arena = criar_arena();
    Disparador **d = NULL; 
    Carregador **c = NULL; 
    int disp_conter = 0, car_conter = 0;
    int comandos_executados = 0;

    char linha_buffer[512];
    char comando[64];

    while (fgets(linha_buffer, sizeof(linha_buffer), qry) != NULL)
    {
        comando[0] = '\0';
        sscanf(linha_buffer, "%s", comando);
        if (strlen(comando) == 0) continue;

        comandos_executados++;

        if (strncmp(comando, "pd", 2) == 0)
        {
            int id; float x, y;
            sscanf(linha_buffer, "pd %d %f %f", &id, &x, &y);
            Disparador nd_opaco = criar_disparador(id, (int)x, (int)y);
            
            d = realloc(d, (disp_conter + 1) * sizeof(Disparador *));
            d[disp_conter] = malloc(sizeof(Disparador));
            *d[disp_conter] = nd_opaco;
            disp_conter++;
        }
        else if (strncmp(comando, "lc", 2) == 0)
        {
            int id, n;
            sscanf(linha_buffer, "lc %d %d", &id, &n);
            if (n <= 0) continue;

            Carregador nc_opaco = criar_carredor(id);
            // PASSA O ARQUIVO TXT PARA LOGAR
            carregar_carregador(get_ground_fila(ground), &nc_opaco, n, txt); 

            c = realloc(c, (car_conter + 1) * sizeof(Carregador *));
            c[car_conter] = malloc(sizeof(Carregador));
            *c[car_conter] = nc_opaco;
            car_conter++;
        }
        else if (strncmp(comando, "atch", 4) == 0)
        {
            int id_disp, id_ce, id_cd;
            sscanf(linha_buffer, "atch %d %d %d", &id_disp, &id_ce, &id_cd);

            Disparador *disp_alvo_ptr = NULL;
            for (int i = 0; i < disp_conter; i++) {
                if (d[i] && *d[i] && disparador_get_id(d[i]) == id_disp) { disp_alvo_ptr = d[i]; break; }
            }

            if (disp_alvo_ptr) {
                Carregador *ce_ptr = NULL, *cd_ptr = NULL;
                int idx_esq = -1, idx_dir = -1;
                for (int i = 0; i < car_conter; i++) {
                    if (c[i] && *c[i]) {
                        if (carregador_get_id(c[i]) == id_ce) { ce_ptr = c[i]; idx_esq = i; }
                        if (carregador_get_id(c[i]) == id_cd) { cd_ptr = c[i]; idx_dir = i; }
                    }
                }
                if (ce_ptr) { disparador_set_carregador_esq(disp_alvo_ptr, ce_ptr); if(idx_esq!=-1) {free(c[idx_esq]); c[idx_esq]=NULL;} }
                if (cd_ptr) { disparador_set_carregador_dir(disp_alvo_ptr, cd_ptr); if(idx_dir!=-1 && idx_dir!=idx_esq) {free(c[idx_dir]); c[idx_dir]=NULL;} }
            } else {
                if (txt) fprintf(txt, "ATCH: Disparador ID %d não encontrado.\n", id_disp);
            }
        }
        else if (strncmp(comando, "shft", 4) == 0)
        {
            int id, n; char lado[2];
            sscanf(linha_buffer, "shft %d %1s %d", &id, lado, &n);
            
            bool found = false;
            for (int i = 0; i < disp_conter; i++) {
                if (d[i] && *d[i] && disparador_get_id(d[i]) == id) {
                    carregar_disparador(d[i], n, lado);
                    found = true;
                    break;
                }
            }
            if (!found && txt) fprintf(txt, "SHFT: Disparador ID %d não encontrado.\n", id);
        }
        else if (strncmp(comando, "rjd", 3) == 0)
        {
            int id; float dx, dy, ix, iy; char lado[2];
            sscanf(linha_buffer, "rjd %d %1s %f %f %f %f", &id, lado, &dx, &dy, &ix, &iy);
            
            Disparador *disp_ptr = NULL;
            for (int i = 0; i < disp_conter; i++) {
                if (d[i] && *d[i] && disparador_get_id(d[i]) == id) { disp_ptr = d[i]; break; }
            }

            if (disp_ptr) {
                if (txt) fprintf(txt, "[rjd] Iniciando rajada para disparador ID %d, lado %s\n", id, lado);
                int k = 0;
                while(1) {
                    // Tenta carregar. Se falhar no lado pedido, a função corrigida no disparador.c vai tentar o outro lado.
                    carregar_disparador(disp_ptr, 1, lado);
                    Forma f_opaco = disparador_disparar_forma(disp_ptr);
                    if (!f_opaco) {
                        if (txt) fprintf(txt, "\t-> Rajada concluída. Carregador esgotado ou erro no disparo.\n\n");
                        break;
                    }
                    total_disparos++;
                    FormaStruct *f = (FormaStruct *)f_opaco;
                    float dx_atual = dx + k*ix;
                    float dy_atual = dy + k*iy;
                    float x_disp = disparador_get_x(disp_ptr);
                    float y_disp = disparador_get_y(disp_ptr);
                    float x_final = x_disp + dx_atual;
                    float y_final = y_disp + dy_atual;
                    
                    FormaStruct *clone = clonar_forma(f, x_final, y_final, NULL, false);
                    if (clone) {
                        arena_adicionar_forma(arena, (Forma)clone);
                        adicionar_na_fila(get_ground_fila(ground), (void*)clone);
                        if (txt) {
                            fprintf(txt, "\t- Rajada Disparo %d: Forma ID %d -> Posicao Final (%.1f, %.1f) | Deslocamento (dx:%.1f, dy:%.1f)\n",
                                    k + 1, f->id_original, x_final, y_final, dx_atual, dy_atual);
                        }
                    } else {
                        if (txt) fprintf(txt, "\t- Rajada Disparo %d: Erro ao clonar forma ID %d\n", k + 1, f->id_original);
                    }
                    k++;
                }
            } else {
                if (txt) fprintf(txt, "[rjd] Erro: Disparador com ID %d não encontrado.\n", id);
            }
        }
        else if (strncmp(comando, "dsp", 3) == 0)
        {
            int id; float dx, dy;
            sscanf(linha_buffer, "dsp %d %f %f", &id, &dx, &dy);
            
            bool found = false;
            for (int i = 0; i < disp_conter; i++) {
                if (d[i] && *d[i] && disparador_get_id(d[i]) == id) {
                    Forma f_opaco = disparador_disparar_forma(d[i]);
                    if (f_opaco) {
                        total_disparos++;
                        FormaStruct *f = (FormaStruct*)f_opaco;
                        float x_f = disparador_get_x(d[i]) + dx;
                        float y_f = disparador_get_y(d[i]) + dy;
                        FormaStruct *clone = clonar_forma(f, x_f, y_f, NULL, false);
                        if (clone) {
                            arena_adicionar_forma(arena, (Forma)clone);
                            adicionar_na_fila(get_ground_fila(ground), (void*)clone);
                        } else {
                            if (txt) fprintf(txt, "DSP: Erro ao clonar forma ID %d para arena.\n", f->id_original);
                        }
                        if (txt) fprintf(txt, "Disparo: ID %d -> Posição (%.1f, %.1f)\n", f->id_original, x_f, y_f);
                    } else {
                        if (txt) fprintf(txt, "DSP: Disparador ID %d vazio.\n", id);
                    }
                    found = true;
                    break;
                }
            }
            if (!found && txt) fprintf(txt, "DSP: Disparador ID %d não encontrado.\n", id);
        }
        else if (strncmp(comando, "calc", 4) == 0)
        {
            if (txt) fprintf(txt, "\n--- Início do Processamento de Colisões (calc) ---\n");

            Fila fila_arena = arena ? ((ArenaStruct *)arena)->formas_na_arena : NULL;
            Fila fila_ground = get_ground_fila(ground);

            if (fila_arena) {
                void *I_void, *J_void;
                while (remover_da_fila(fila_arena, &I_void)) {
                    FormaStruct *I = (FormaStruct *)I_void;
                    if (!I) continue;
                    forma_set_destruida((Forma)I, true);

                    if (!remover_da_fila(fila_arena, &J_void)) {
                        FormaStruct *Ipos = clonar_forma(I, I->x_landed, I->y_landed, NULL, false);
                        if(Ipos) adicionar_na_fila(fila_ground, Ipos);
                        else if(txt) fprintf(txt, "CALC: Erro ao clonar forma sobrevivente I (ID %d).\n", I->id_original);
                        continue;
                    }
                    FormaStruct *J = (FormaStruct *)J_void;
                    if (!J) {
                        FormaStruct *Ipos = clonar_forma(I, I->x_landed, I->y_landed, NULL, false);
                        if(Ipos) adicionar_na_fila(fila_ground, Ipos);
                        else if(txt) fprintf(txt, "CALC: Erro ao clonar forma sobrevivente I (ID %d) após J NULL.\n", I->id_original);
                        continue;
                    }

                    forma_set_destruida((Forma)J, true);

                    if (formas_colidem(I, J)) {
                        double aI = (double)calcular_area_forma(I);
                        double aJ = (double)calcular_area_forma(J);

                        if(txt) fprintf(txt, "\tColisao: ID %d (Area %.2f) vs ID %d (Area %.2f)\n", I->id_original, aI, J->id_original, aJ);

                        if (aI < aJ) {
                            pontuacao_final += aI;
                            if(txt) fprintf(txt, "\t-> ID %d sobrevive, ID %d destruido.\n", J->id_original, I->id_original);
                            total_formas_esmagadas++;
                            FormaStruct *Jpos = clonar_forma(J, J->x_landed, J->y_landed, NULL, false);
                            if(Jpos) adicionar_na_fila(fila_ground, Jpos);
                            else if(txt) fprintf(txt, "CALC: Erro ao clonar forma sobrevivente J (ID %d).\n", J->id_original);
                        } else {
                            pontuacao_final += aJ;
                            if(txt) fprintf(txt, "\t-> ID %d sobrevive, ID %d destruido. Clones gerados.\n", I->id_original, J->id_original);
                            total_formas_esmagadas++; 
                            
                            const char *cor_i = NULL;
                            if(I->tipo == TIPO_CIRCULO) cor_i = get_corPreenchimento_circulo(I->dados_forma);
                            else if(I->tipo == TIPO_RETANGULO) cor_i = get_corPreenchimento_retangulo(I->dados_forma);
                            else if(I->tipo == TIPO_LINHA) cor_i = get_cor_linha(I->dados_forma);
                            else if(I->tipo == TIPO_TEXTO) cor_i = get_corPreenchimento_texto(I->dados_forma);

                            FormaStruct *Jprime = clonar_forma(J, J->x_landed, J->y_landed, cor_i, false);
                            FormaStruct *Ipos = clonar_forma(I, I->x_landed, I->y_landed, NULL, false);
                            FormaStruct *Iclone = clonar_forma(I, I->x_landed, I->y_landed, NULL, true); 

                            if(Ipos) adicionar_na_fila(fila_ground, Ipos);
                            else if(txt) fprintf(txt, "CALC: Erro ao clonar Ipos (ID %d).\n", I->id_original);
                            if(Jprime) adicionar_na_fila(fila_ground, Jprime);
                            else if(txt) fprintf(txt, "CALC: Erro ao clonar JprimePos (orig ID %d).\n", J->id_original);
                            if(Iclone) adicionar_na_fila(fila_ground, Iclone);
                            else if(txt) fprintf(txt, "CALC: Erro ao clonar IclonePos (ID %d).\n", I->id_original);
                        }
                    } else {
                        FormaStruct *Ipos = clonar_forma(I, I->x_landed, I->y_landed, NULL, false);
                        FormaStruct *Jpos = clonar_forma(J, J->x_landed, J->y_landed, NULL, false);
                        if(Ipos) adicionar_na_fila(fila_ground, Ipos);
                        else if(txt) fprintf(txt, "CALC: Erro ao clonar Ipos (sem overlap, ID %d).\n", I->id_original);
                        if(Jpos) adicionar_na_fila(fila_ground, Jpos);
                        else if(txt) fprintf(txt, "CALC: Erro ao clonar Jpos (sem overlap, ID %d).\n", J->id_original);
                    }
                }
            } else {
                if (txt) fprintf(txt, "Erro: Fila da arena ou ground inválida no CALC.\n");
            }
            
            if (txt)
            {
                fprintf(txt, "\n[calc]\n");
                fprintf(txt, "\tResult: %.2f\n", (float)pontuacao_final);
                fprintf(txt, "\n");
            }

            if (((ArenaStruct*)arena)->formas_na_arena) destruir_fila(((ArenaStruct*)arena)->formas_na_arena);
            ((ArenaStruct*)arena)->formas_na_arena = iniciar_fila();
            if (!((ArenaStruct *)arena)->formas_na_arena) {
                fprintf(stderr, "Erro fatal: Falha ao recriar fila da arena após CALC!\n");
                exit(1);
            }
        }
        else
        {
            if (txt) fprintf(txt, "Comando QRY desconhecido: %s\n", comando);
        }
    }

    if (txt) {
        fprintf(txt, "\n\n--- RELATORIO FINAL QRY ---\n");
        fprintf(txt, "pontuacao final: %.2f\n", (float)pontuacao_final);
        fprintf(txt, "numero total de instrucoes executadas: %d\n", comandos_executados);
        fprintf(txt, "numero total de disparos: %d\n", total_disparos);
        fprintf(txt, "numero total de formas esmagadas: %d\n", total_formas_esmagadas);
        fprintf(txt, "numero total de formas clonadas: %d\n", total_formas_clonadas);
    }

    Fila fila_ground = get_ground_fila(ground);
    Fila temp_fila = iniciar_fila();
    if (!temp_fila) { destruir_arena(arena); return; }
    void *forma_ptr;

    while (remover_da_fila(fila_ground, &forma_ptr))
    {
        FormaStruct *f = (FormaStruct *)forma_ptr;
        if (f && !f->foi_destruida)
        {
            switch (f->tipo)
            {
            case TIPO_CIRCULO: {
                Circulo *c = (Circulo *)f->dados_forma;
                fprintf(svg, " <circle id=\"%d\" cx=\"%lf\" cy=\"%lf\" r=\"%lf\" stroke=\"%s\" fill=\"%s\" opacity=\"0.500000\" stroke-width=\"1.500000\" />\n",
                        f->id_original, (double)get_x(c), (double)get_y(c), (double)get_raio(c), get_corBorda_circulo(c), get_corPreenchimento_circulo(c));
                break;
            }
            case TIPO_RETANGULO: {
                Retangulo *r = (Retangulo *)f->dados_forma;
                fprintf(svg, "\t<rect id=\"%d\" x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" stroke=\"%s\" fill=\"%s\" opacity=\"0.500000\" stroke-width=\"1.500000\" />\n",
                        f->id_original, (double)get_x_retangulo(r), (double)get_y_retangulo(r), (double)get_largura(r), (double)get_altura(r), get_corBorda_retangulo(r), get_corPreenchimento_retangulo(r));
                break;
            }
            case TIPO_LINHA: {
                Linha *l = (Linha *)f->dados_forma;
                fprintf(svg, "\t<line id=\"%d\" x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"%s\" stroke-width=\"1.500000\" />\n",
                        f->id_original, (double)get_x1_linha(l), (double)get_y1_linha(l), (double)get_x2_linha(l), (double)get_y2_linha(l), get_cor_linha(l));
                break;
            }
            case TIPO_TEXTO: {
                Texto *t = (Texto *)f->dados_forma;
                char anchor = get_anchor_texto(t);
                const char *ancora_svg = "start";
                if (anchor == 'm' || anchor == 'M') ancora_svg = "middle";
                else if (anchor == 'e' || anchor == 'E' || anchor == 'f' || anchor == 'F') ancora_svg = "end";
                
                const char* font_packed = get_fonte_texto(t);
                char fam[64]="sans-serif", weight[16]="normal", size[16]="12";
                if(font_packed && strlen(font_packed)>0) {
                    sscanf(font_packed, "%[^,],%[^,],%s", fam, weight, size);
                }

                fprintf(svg, "\t<text id=\"%d\" x=\"%lf\" y=\"%lf\" stroke=\"%s\" fill=\"%s\" font-family=\"%s\" font-weight=\"%s\" font-size=\"%spt\" text-anchor=\"%s\">%s</text>\n",
                        f->id_original, (double)get_x_texto(t), (double)get_y_texto(t),
                        get_corBorda_texto(t), get_corPreenchimento_texto(t),
                        fam, weight, size, ancora_svg, get_conteudo_texto(t));
                break;
            }
            }
        }
        adicionar_na_fila(temp_fila, forma_ptr);
    }
    while(remover_da_fila(temp_fila, &forma_ptr)) adicionar_na_fila(fila_ground, forma_ptr);
    destruir_fila(temp_fila);

    fprintf(svg, "</g>\n");
    fprintf(svg, "</svg>\n");

    if (d) { for(int i=0;i<disp_conter;i++) if(d[i]) { destruir_disparador(d[i]); free(d[i]); } free(d); }
    if (c) { for(int i=0;i<car_conter;i++) if(c[i] && *c[i]) { destruir_carregador(*c[i]); free(c[i]); } free(c); }
    destruir_arena(arena);
}