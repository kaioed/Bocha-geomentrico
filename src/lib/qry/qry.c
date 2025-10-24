#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "qry.h" // Inclui os protótipos e typedefs
#include "../geo/geo.h"
#include "../disparador/disparador.h"
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"

// Definição da FormaStruct (mantida no .c)
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

// (Lógica P1) Calcula a área da forma
static float calcular_area_forma(FormaStruct *f)
{
    // ... (igual à versão anterior) ...
    if (!f || !f->dados_forma) // Adicionada verificação de dados_forma
        return 0.0f;
    switch (f->tipo)
    {
    case TIPO_CIRCULO:
        return area_circulo(f->dados_forma);
    case TIPO_RETANGULO:
        return area_retangulo(f->dados_forma);
    case TIPO_LINHA:
        return area_linha(f->dados_forma);
    case TIPO_TEXTO:
        return area_texto(f->dados_forma);
    default:
        return 0.0f;
    }
}

// Implementação dos Getters
TipoForma forma_get_tipo(Forma forma)
{
    // ... (igual à versão anterior) ...
    if (!forma)
        return (TipoForma)-1; // Retorna um valor inválido, talvez logar erro?
    return ((FormaStruct *)forma)->tipo;
}
int forma_get_id_original(Forma forma)
{
    // ... (igual à versão anterior) ...
    if (!forma)
        return -1;
    return ((FormaStruct *)forma)->id_original;
}

// Implementação da função acessora
void forma_set_destruida(Forma forma, bool status)
{
    // ... (igual à versão anterior) ...
    if (!forma)
        return;
    ((FormaStruct *)forma)->foi_destruida = status;
}

// (Lógica P1) Clona uma forma...
static FormaStruct *clonar_forma(FormaStruct *original, float x, float y, const char *nova_cor_borda, bool trocar_cores)
{
    // ... (igual à versão anterior, com verificações internas) ...
    if (!original || !original->dados_forma) // Verifica original e seus dados
        return NULL;

    int id_clone = proximo_id_clone++;
    void *dados_clonados = NULL;

    switch (original->tipo)
    {
    case TIPO_CIRCULO:
    {
        Circulo *orig_c = (Circulo *)original->dados_forma;
        const char *cor_b_orig = get_corBorda_circulo(orig_c);
        const char *cor_p_orig = get_corPreenchimento_circulo(orig_c);
        if (!cor_b_orig || !cor_p_orig)
            return NULL; // Verifica cores originais

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
        if (!cor_b_orig || !cor_p_orig)
            return NULL; // Verifica cores

        const char *cor_b = trocar_cores ? cor_p_orig : (nova_cor_borda ? nova_cor_borda : cor_b_orig);
        const char *cor_p = trocar_cores ? cor_b_orig : cor_p_orig;
        dados_clonados = criar_retangulo(x, y, get_largura(orig_r), get_altura(orig_r), (char *)cor_p, (char *)cor_b, id_clone);
        break;
    }
    case TIPO_LINHA:
    {
        Linha *orig_l = (Linha *)original->dados_forma;
        const char *cor_original = get_cor_linha(orig_l);
        if (!cor_original)
            return NULL; // Verifica cor

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
        if (!orig_borda || !orig_preench || !orig_conteudo)
            return NULL; // Verifica cores e conteúdo

        char orig_anchor = get_anchor_texto(orig_t);

        const char *cor_b = trocar_cores ? orig_preench : (nova_cor_borda ? nova_cor_borda : orig_borda);
        const char *cor_p = trocar_cores ? orig_borda : orig_preench;

        dados_clonados = criar_texto(x, y, (char *)cor_b, (char *)cor_p, orig_anchor, (char *)orig_conteudo, NULL, id_clone);
        break;
    }
    }

    if (!dados_clonados)
        return NULL; // Falha na criação específica da forma

    FormaStruct *wrapper_clone = malloc(sizeof(FormaStruct));
    if (!wrapper_clone)
    {
        // Libera dados_clonados se a alocação do wrapper falhar
        switch (original->tipo)
        {
        case TIPO_CIRCULO:
            if (dados_clonados)
                liberar_circulo(dados_clonados);
            break;
        case TIPO_RETANGULO:
            if (dados_clonados)
                liberar_retangulo(dados_clonados);
            break;
        case TIPO_LINHA:
            if (dados_clonados)
                liberar_linha(dados_clonados);
            break;
        case TIPO_TEXTO:
            if (dados_clonados)
                liberar_texto(dados_clonados);
            break;
        }
        fprintf(stderr, "Falha ao alocar wrapper para clone ID %d\n", id_clone);
        return NULL;
    }

    wrapper_clone->id_original = id_clone;
    wrapper_clone->tipo = original->tipo;
    wrapper_clone->dados_forma = dados_clonados;
    wrapper_clone->foi_clonada = true;
    wrapper_clone->foi_destruida = false;
    wrapper_clone->x_landed = x;
    wrapper_clone->y_landed = y;
    return wrapper_clone;
}

// Estrutura da Arena ...
typedef struct
{
    Fila formas_na_arena;
} ArenaStruct;

Arena criar_arena()
{
    // ... (igual, com verificações) ...
    ArenaStruct *a = malloc(sizeof(ArenaStruct));
    if (!a)
    {
        fprintf(stderr, "Falha ao alocar ArenaStruct\n");
        return NULL;
    }
    a->formas_na_arena = iniciar_fila();
    if (!a->formas_na_arena)
    {
        fprintf(stderr, "Falha ao iniciar fila da arena\n");
        free(a);
        return NULL;
    }
    return a;
}

void arena_adicionar_forma(Arena a, Forma forma)
{
    // ... (igual, com verificações) ...
    ArenaStruct *arena = (ArenaStruct *)a;
    if (!arena || !arena->formas_na_arena || !forma)
    {
        fprintf(stderr, "Erro: Tentativa de adicionar forma NULL ou a arena/fila inválida.\n");
        return;
    }
    adicionar_na_fila(arena->formas_na_arena, forma);
}

void destruir_arena(Arena a)
{
    // ... (igual) ...
    if (!a)
        return;
    ArenaStruct *arena = (ArenaStruct *)a;
    if (arena->formas_na_arena)
        destruir_fila(arena->formas_na_arena);
    free(a);
}

// Lógica de Colisão AABB ...
typedef struct
{
    float minX, minY, maxX, maxY;
} Aabb;

static Aabb make_aabb_for_shape(FormaStruct *s)
{
    // ... (igual, com verificações internas) ...
    Aabb box = {0, 0, 0, 0};
    if (!s || !s->dados_forma)
        return box; // Verifica s e dados_forma

    float landed_x = s->x_landed;
    float landed_y = s->y_landed;

    switch (s->tipo)
    {
    case TIPO_CIRCULO:
    {
        // ... (igual) ...
        float r = get_raio((Circulo *)s->dados_forma);
        box.minX = landed_x - r;
        box.maxX = landed_x + r;
        box.minY = landed_y - r;
        box.maxY = landed_y + r;
        break;
    }
    case TIPO_RETANGULO:
    {
        // ... (igual) ...
        float w = get_largura((Retangulo *)s->dados_forma);
        float h = get_altura((Retangulo *)s->dados_forma);
        box.minX = landed_x;
        box.maxX = landed_x + w;
        box.minY = landed_y;
        box.maxY = landed_y + h;
        break;
    }
    case TIPO_TEXTO:
    {
        // ... (igual, com verificação de txt) ...
        Texto *t = (Texto *)s->dados_forma;
        const char *txt = get_conteudo_texto(t);
        int len = txt ? (int)strlen(txt) : 0;
        float segLen = 10.0f * (float)len;
        char anchor = get_anchor_texto(t);
        float x1 = landed_x, y1 = landed_y, x2 = landed_x;

        if (anchor == 'i' || anchor == 'I' || anchor == 's' || anchor == 'S')
        {
            x2 = landed_x + segLen;
        }
        else if (anchor == 'f' || anchor == 'F' || anchor == 'e' || anchor == 'E')
        {
            x1 = landed_x - segLen;
        }
        else if (anchor == 'm' || anchor == 'M')
        {
            x1 = landed_x - segLen * 0.5f;
            x2 = landed_x + segLen * 0.5f;
        }
        else
        { // default to start
            x2 = landed_x + segLen;
        }

        box.minX = (x1 < x2 ? x1 : x2) - 1.0f;
        box.maxX = (x1 > x2 ? x1 : x2) + 1.0f;
        box.minY = y1 - 1.0f;
        box.maxY = y1 + 1.0f;
        break;
    }
    case TIPO_LINHA:
    {
        // ... (igual) ...
        Linha *l = (Linha *)s->dados_forma;
        float x1 = get_x1_linha(l);
        float y1 = get_y1_linha(l);
        float x2 = get_x2_linha(l);
        float y2 = get_y2_linha(l);

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
    // ... (igual) ...
    if (a.maxX < b.minX)
        return false;
    if (b.maxX < a.minX)
        return false;
    if (a.maxY < b.minY)
        return false;
    if (b.maxY < a.minY)
        return false;
    return true;
}

bool formas_colidem(FormaStruct *f1, FormaStruct *f2)
{
    // ... (igual, com verificação de NULL) ...
    if (!f1 || !f2)
        return false;
    Aabb a = make_aabb_for_shape(f1);
    Aabb b = make_aabb_for_shape(f2);
    return aabb_overlap(a, b);
}

// Função principal process_qry ...
void process_qry(FILE *qry, FILE *svg, Ground ground, FILE *txt)
{
    // Verifica ponteiros no início
    if (!svg || !ground || !txt)
    {
        fprintf(stderr, "Erro: Ponteiro de arquivo inválido em process_qry (svg, ground ou txt).\n");
        return;
    }
    fprintf(svg, "<svg xmlns='http://www.w3.org/2000/svg'>\n");

    Arena arena = criar_arena();
    if (!arena)
    {
        fprintf(stderr, "Erro fatal: Falha ao criar arena!\n");
        // Idealmente, deveria liberar outras coisas aqui, mas vamos simplificar
        return;
    }

    // Arrays para armazenar ponteiros para os ponteiros opacos
    Disparador **d = NULL; // Array de Disparador* (void**)
    Carregador **c = NULL; // Array de Carregador* (void**)
    int disp_conter = 0, car_conter = 0;

    char comando[64];
    while (qry && fscanf(qry, "%63s", comando) == 1) // Verifica qry
    {
        // --- Comando pd ---
        if (strcmp(comando, "pd") == 0)
        {
            int id;
            float x, y;
            if (fscanf(qry, "%d %f %f", &id, &x, &y) != 3)
            {
                fprintf(stderr, "Erro lendo args para pd\n");
                continue;
            }
            Disparador nd_opaco = criar_disparador(id, (int)x, (int)y); // Retorna Disparador (void*)
            if (!nd_opaco)
            {
                fprintf(stderr, "Erro criando disparador %d\n", id);
                continue;
            }

            // Realoca o array de ponteiros
            Disparador **temp_d = realloc(d, (disp_conter + 1) * sizeof(Disparador *));
            if (!temp_d)
            {
                fprintf(stderr, "Falha no realloc para array d\n");
                destruir_disparador(&nd_opaco); // Libera o disparador criado
                continue;
            }
            d = temp_d;

            // *** CORREÇÃO: Aloca memória para o PONTEIRO no array ***
            d[disp_conter] = malloc(sizeof(Disparador)); // Aloca Disparador* (void**)
            if (!d[disp_conter])
            { // *** CORREÇÃO: Verifica malloc ***
                fprintf(stderr, "Falha no malloc para ponteiro d[%d]\n", disp_conter);
                destruir_disparador(&nd_opaco); // Libera o disparador criado
                // Reduz o contador se o realloc aumentou, para evitar acesso inválido na limpeza
                // (Alternativamente, poderia tentar realocar para tamanho menor, mas é complexo)
                continue; // Pula este disparador
            }
            *d[disp_conter] = nd_opaco; // Armazena o ponteiro opaco (void*)
            disp_conter++;
        }
        // --- Comando lc ---
        else if (strcmp(comando, "lc") == 0)
        {
            int id, n;
            if (fscanf(qry, "%d %d", &id, &n) != 2)
            {
                fprintf(stderr, "Erro lendo args para lc\n");
                continue;
            }
            if (n <= 0)
                continue;

            Carregador nc_opaco = criar_carredor(id); // Retorna Carregador (void*)
            if (!nc_opaco)
            {
                fprintf(stderr, "Erro criando carregador %d\n", id);
                continue;
            }

            // Passa o endereço do ponteiro opaco para carregar
            carregar_carregador(get_ground_fila(ground), &nc_opaco, n, txt);

            // Realoca o array de ponteiros
            Carregador **temp_c = realloc(c, (car_conter + 1) * sizeof(Carregador *));
            if (!temp_c)
            {
                fprintf(stderr, "Falha no realloc para array c\n");
                destruir_carregador(nc_opaco); // Libera o carregador criado
                continue;
            }
            c = temp_c;

            // *** CORREÇÃO: Aloca memória para o PONTEIRO no array ***
            c[car_conter] = malloc(sizeof(Carregador)); // Aloca Carregador* (void**)
            if (!c[car_conter])
            { // *** CORREÇÃO: Verifica malloc ***
                fprintf(stderr, "Falha no malloc para ponteiro c[%d]\n", car_conter);
                destruir_carregador(nc_opaco); // Libera o carregador criado
                continue;                      // Pula este carregador
            }
            *c[car_conter] = nc_opaco; // Armazena o ponteiro opaco (void*)
            car_conter++;
        }
        // --- Comando atch ---
        else if (strcmp(comando, "atch") == 0)
        {
            // ... (lógica igual, mas com mais verificações de NULL) ...
            int id_disp, id_ce, id_cd;
            if (fscanf(qry, "%d %d %d", &id_disp, &id_ce, &id_cd) != 3)
            {
                fprintf(stderr, "Erro lendo args para atch\n");
                continue;
            }

            Disparador *disp_alvo_ptr = NULL; // Ponteiro para (void**)
            for (int i = 0; i < disp_conter; i++)
            {
                // *** CORREÇÃO: Verifica d[i] ANTES de *d[i] ***
                if (d[i] && *d[i] && disparador_get_id(d[i]) == id_disp)
                {
                    disp_alvo_ptr = d[i];
                    break;
                }
            }

            if (disp_alvo_ptr)
            {
                Carregador *carregador_esq_ptr = NULL; // Ponteiro para (void**)
                Carregador *carregador_dir_ptr = NULL; // Ponteiro para (void**)
                int idx_esq = -1, idx_dir = -1;

                for (int i = 0; i < car_conter; i++)
                {
                    // *** CORREÇÃO: Verifica c[i] ANTES de *c[i] ***
                    if (c[i] != NULL && *c[i] != NULL)
                    {
                        if (carregador_get_id(c[i]) == id_ce)
                        {
                            carregador_esq_ptr = c[i];
                            idx_esq = i;
                        }
                        if (carregador_get_id(c[i]) == id_cd)
                        {
                            carregador_dir_ptr = c[i];
                            idx_dir = i;
                        }
                    }
                }
                // Associa e anula ponteiro no array c
                if (carregador_esq_ptr)
                {
                    disparador_set_carregador_esq(disp_alvo_ptr, carregador_esq_ptr);
                    if (idx_esq != -1)
                    {
                        // A memória apontada por *c[idx_esq] agora pertence ao disparador
                        free(c[idx_esq]); // Libera o container do ponteiro no array
                        c[idx_esq] = NULL;
                    }
                }
                if (carregador_dir_ptr)
                {
                    disparador_set_carregador_dir(disp_alvo_ptr, carregador_dir_ptr);
                    if (idx_dir != -1 && idx_dir != idx_esq)
                    {
                        // A memória apontada por *c[idx_dir] agora pertence ao disparador
                        free(c[idx_dir]); // Libera o container do ponteiro no array
                        c[idx_dir] = NULL;
                    }
                    else if (idx_dir != -1 && idx_dir == idx_esq)
                    {
                        // Se for o mesmo carregador, já foi anulado
                    }
                }
            }
            else
            {
                if (txt)
                    fprintf(txt, "ATCH: Disparador ID %d não encontrado.\n", id_disp);
            }
        }
        // --- Comando shft ---
        else if (strcmp(comando, "shft") == 0)
        {
            // ... (lógica igual, com mais verificações de NULL) ...
            int id, n;
            char lado[2];
            if (fscanf(qry, "%d %1s %d", &id, lado, &n) != 3)
            {
                fprintf(stderr, "Erro lendo args para shft\n");
                continue;
            }
            if (n <= 0 || (lado[0] != 'e' && lado[0] != 'd'))
                continue;

            bool found = false;
            for (int i = 0; i < disp_conter; i++)
            {
                // *** CORREÇÃO: Verifica d[i] ANTES de *d[i] ***
                if (d[i] && *d[i] && disparador_get_id(d[i]) == id)
                {
                    carregar_disparador(d[i], n, lado);
                    found = true;
                    break;
                }
            }
            if (!found && txt)
                fprintf(txt, "SHFT: Disparador ID %d não encontrado.\n", id);
        }
        // --- Comando rjd ---
        else if (strcmp(comando, "rjd") == 0)
        {
            // ... (lógica igual, com mais verificações de NULL) ...
            int id;
            float dx_inicial, dy_inicial, ix, iy;
            char lado[2];
            if (fscanf(qry, "%d %1s %f %f %f %f", &id, lado, &dx_inicial, &dy_inicial, &ix, &iy) != 6)
            {
                fprintf(stderr, "Erro lendo args para rjd\n");
                continue;
            }
            if (lado[0] != 'e' && lado[0] != 'd')
                continue;

            Disparador *disp_alvo_ptr = NULL;
            for (int i = 0; i < disp_conter; i++)
            {
                // *** CORREÇÃO: Verifica d[i] ANTES de *d[i] ***
                if (d[i] && *d[i] && disparador_get_id(d[i]) == id)
                {
                    disp_alvo_ptr = d[i];
                    break;
                }
            }

            if (disp_alvo_ptr)
            {
                int contador_disparos = 0;
                if (txt)
                    fprintf(txt, "[rjd] Iniciando rajada para disparador ID %d, lado %s\n", id, lado);

                while (1)
                {
                    carregar_disparador(disp_alvo_ptr, 1, lado);
                    Forma forma_disparada_opaco = disparador_disparar_forma(disp_alvo_ptr);
                    if (!forma_disparada_opaco)
                    {
                        if (txt)
                            fprintf(txt, "\t-> Rajada concluída. Carregador esgotado ou erro no disparo.\n\n");
                        break;
                    }
                    FormaStruct *forma_disparada = (FormaStruct *)forma_disparada_opaco;

                    float dx_atual = dx_inicial + contador_disparos * ix;
                    float dy_atual = dy_inicial + contador_disparos * iy;
                    float x_disp_val = disparador_get_x(disp_alvo_ptr);
                    float y_disp_val = disparador_get_y(disp_alvo_ptr);
                    float x_final = x_disp_val + dx_atual;
                    float y_final = y_disp_val + dy_atual;

                    FormaStruct *forma_na_arena = clonar_forma(forma_disparada, x_final, y_final, NULL, false);

                    if (forma_na_arena)
                    {
                        arena_adicionar_forma(arena, (Forma)forma_na_arena);
                        adicionar_na_fila(get_ground_fila(ground), (void *)forma_na_arena);

                        if (txt)
                        {
                            fprintf(txt, "\t- Rajada Disparo %d: Forma ID %d -> Posicao Final (%.1f, %.1f) | Deslocamento (dx:%.1f, dy:%.1f)\n",
                                    contador_disparos + 1,
                                    forma_disparada->id_original,
                                    x_final, y_final, dx_atual, dy_atual);
                        }
                    }
                    else
                    {
                        if (txt)
                            fprintf(txt, "\t- Rajada Disparo %d: Erro ao clonar forma ID %d\n", contador_disparos + 1, forma_disparada->id_original);
                    }
                    contador_disparos++;
                }
            }
            else
            {
                if (txt)
                    fprintf(txt, "[rjd] Erro: Disparador com ID %d não encontrado.\n", id);
            }
        }
        // --- Comando dsp ---
        else if (strcmp(comando, "dsp") == 0)
        {
            // ... (lógica igual, com mais verificações de NULL) ...
            int id;
            float dx, dy;
            char flag[2];
            if (fscanf(qry, "%d %f %f %1s", &id, &dx, &dy, flag) != 4)
            {
                fprintf(stderr, "Erro lendo args para dsp\n");
                continue;
            }

            bool found = false;
            for (int i = 0; i < disp_conter; i++)
            {
                // *** CORREÇÃO: Verifica d[i] ANTES de *d[i] ***
                if (d[i] && *d[i] && disparador_get_id(d[i]) == id)
                {
                    Forma forma_disparada_opaco = disparador_disparar_forma(d[i]);
                    if (!forma_disparada_opaco)
                    { // Se não disparou nada, termina
                        if (txt)
                            fprintf(txt, "DSP: Disparador ID %d vazio.\n", id);
                        found = true; // Marca como encontrado para não logar erro
                        break;
                    }
                    FormaStruct *forma_disparada = (FormaStruct *)forma_disparada_opaco;

                    float x_final = disparador_get_x(d[i]) + dx;
                    float y_final = disparador_get_y(d[i]) + dy;

                    FormaStruct *forma_na_arena = clonar_forma(forma_disparada, x_final, y_final, NULL, false);

                    if (forma_na_arena)
                    {
                        arena_adicionar_forma(arena, (Forma)forma_na_arena);
                        adicionar_na_fila(get_ground_fila(ground), (void *)forma_na_arena);
                    }
                    else
                    {
                        if (txt)
                            fprintf(txt, "DSP: Erro ao clonar forma ID %d para arena.\n", forma_disparada->id_original);
                    }

                    if (txt)
                    {
                        fprintf(txt, "Disparo: ID %d -> Posição (%.1f, %.1f)\n", forma_disparada->id_original, x_final, y_final);
                    }
                    found = true;
                    break;
                }
            }
            if (!found && txt)
                fprintf(txt, "DSP: Disparador ID %d não encontrado.\n", id);
        }

        // --- Comando calc ---
        else if (strcmp(comando, "calc") == 0)
        {
            // ... (lógica igual, com mais verificações de NULL) ...
            if (txt)
                fprintf(txt, "\n--- Início do Processamento de Colisões (calc) ---\n");

            // Verifica se a arena e o ground são válidos
            Fila fila_arena = arena ? ((ArenaStruct *)arena)->formas_na_arena : NULL;
            Fila fila_ground = get_ground_fila(ground);
            if (!fila_arena || !fila_ground)
            {
                fprintf(stderr, "Erro: Fila da arena ou ground inválida no CALC.\n");
                continue;
            }

            double total_crushed_area = 0.0;
            void *I_ptr_void = NULL;
            void *J_ptr_void = NULL;

            while (remover_da_fila(fila_arena, &I_ptr_void))
            {
                FormaStruct *I = (FormaStruct *)I_ptr_void;
                if (!I)
                    continue;

                forma_set_destruida((Forma)I, true);

                if (!remover_da_fila(fila_arena, &J_ptr_void))
                {
                    FormaStruct *Ipos = clonar_forma(I, I->x_landed, I->y_landed, NULL, false);
                    if (Ipos)
                    {
                        adicionar_na_fila(fila_ground, (void *)Ipos);
                    }
                    else
                    {
                        if (txt)
                            fprintf(txt, "CALC: Erro ao clonar forma sobrevivente I (ID %d).\n", I->id_original);
                    }
                    continue;
                }

                FormaStruct *J = (FormaStruct *)J_ptr_void;
                if (!J)
                { // Se J for NULL (erro na fila?), I sobrevive
                    FormaStruct *Ipos = clonar_forma(I, I->x_landed, I->y_landed, NULL, false);
                    if (Ipos)
                        adicionar_na_fila(fila_ground, (void *)Ipos);
                    else
                    {
                        if (txt)
                            fprintf(txt, "CALC: Erro ao clonar forma sobrevivente I (ID %d) após J NULL.\n", I->id_original);
                    }
                    continue;
                }

                forma_set_destruida((Forma)J, true);

                bool overlap = formas_colidem(I, J);

                if (overlap)
                {
                    float areaI_float = calcular_area_forma(I);
                    float areaJ_float = calcular_area_forma(J);
                    double areaI = (double)areaI_float;
                    double areaJ = (double)areaJ_float;

                    if (txt)
                        fprintf(txt, "\tColisao: ID %d (Area %.2f) vs ID %d (Area %.2f)\n", I->id_original, areaI, J->id_original, areaJ);

                    total_crushed_area += (areaI < areaJ) ? areaI : areaJ;

                    if (areaI < areaJ)
                    {
                        if (txt)
                            fprintf(txt, "\t-> ID %d sobrevive, ID %d destruido.\n", J->id_original, I->id_original);

                        FormaStruct *Jpos = clonar_forma(J, J->x_landed, J->y_landed, NULL, false);
                        if (Jpos)
                        {
                            adicionar_na_fila(fila_ground, (void *)Jpos);
                        }
                        else
                        {
                            if (txt)
                                fprintf(txt, "CALC: Erro ao clonar forma sobrevivente J (ID %d).\n", J->id_original);
                        }
                    }
                    else
                    {
                        if (txt)
                            fprintf(txt, "\t-> ID %d sobrevive, ID %d destruido. Clones gerados.\n", I->id_original, J->id_original);

                        const char *cor_preenchimento_i = NULL;
                        if (I->dados_forma)
                        { // Verifica se dados_forma não é NULL
                            if (I->tipo == TIPO_CIRCULO)
                                cor_preenchimento_i = get_corPreenchimento_circulo(I->dados_forma);
                            else if (I->tipo == TIPO_RETANGULO)
                                cor_preenchimento_i = get_corPreenchimento_retangulo(I->dados_forma);
                            else if (I->tipo == TIPO_TEXTO)
                                cor_preenchimento_i = get_corPreenchimento_texto(I->dados_forma);
                            else if (I->tipo == TIPO_LINHA)
                                cor_preenchimento_i = get_cor_linha(I->dados_forma);
                        }

                        FormaStruct *JprimePos = clonar_forma(J, J->x_landed, J->y_landed, cor_preenchimento_i, false);
                        FormaStruct *Ipos = clonar_forma(I, I->x_landed, I->y_landed, NULL, false);
                        FormaStruct *IclonePos = clonar_forma(I, I->x_landed, I->y_landed, NULL, true);

                        if (Ipos)
                            adicionar_na_fila(fila_ground, (void *)Ipos);
                        else
                        {
                            if (txt)
                                fprintf(txt, "CALC: Erro ao clonar Ipos (ID %d).\n", I->id_original);
                        }
                        if (JprimePos)
                            adicionar_na_fila(fila_ground, (void *)JprimePos);
                        else
                        {
                            if (txt)
                                fprintf(txt, "CALC: Erro ao clonar JprimePos (orig ID %d).\n", J->id_original);
                        }
                        if (IclonePos)
                            adicionar_na_fila(fila_ground, (void *)IclonePos);
                        else
                        {
                            if (txt)
                                fprintf(txt, "CALC: Erro ao clonar IclonePos (ID %d).\n", I->id_original);
                        }
                    }
                }
                else // Sem overlap
                {
                    FormaStruct *Ipos = clonar_forma(I, I->x_landed, I->y_landed, NULL, false);
                    FormaStruct *Jpos = clonar_forma(J, J->x_landed, J->y_landed, NULL, false);
                    if (Ipos)
                        adicionar_na_fila(fila_ground, (void *)Ipos);
                    else
                    {
                        if (txt)
                            fprintf(txt, "CALC: Erro ao clonar Ipos (sem overlap, ID %d).\n", I->id_original);
                    }
                    if (Jpos)
                        adicionar_na_fila(fila_ground, (void *)Jpos);
                    else
                    {
                        if (txt)
                            fprintf(txt, "CALC: Erro ao clonar Jpos (sem overlap, ID %d).\n", J->id_original);
                    }
                }
            } // fim while (loop da arena)

            if (txt)
            {
                fprintf(txt, "\n[calc]\n");
                fprintf(txt, "\tResult: %.2f\n", (float)total_crushed_area);
                fprintf(txt, "\n");
            }

            // Esvazia e recria a fila da arena
            if (((ArenaStruct *)arena)->formas_na_arena)
                destruir_fila(((ArenaStruct *)arena)->formas_na_arena);
            ((ArenaStruct *)arena)->formas_na_arena = iniciar_fila();
            if (!((ArenaStruct *)arena)->formas_na_arena)
            {
                fprintf(stderr, "Erro fatal: Falha ao recriar fila da arena após CALC!\n");
                // Aqui talvez seja melhor sair, pois o estado está inconsistente
                exit(1);
            }
        }
        else
        { // Comando desconhecido
            if (txt)
                fprintf(txt, "Comando QRY desconhecido: %s\n", comando);
        }

    } // fim while (leitura qry)

    // Desenho final no SVG...
    Fila fila_ground = get_ground_fila(ground);
    if (!fila_ground)
    {
        fprintf(stderr, "Erro: Fila ground inválida antes do desenho final.\n");
        // Tenta limpar o que foi alocado
        destruir_arena(arena);
        // Não podemos limpar d e c facilmente aqui sem potencialmente causar double free
        return;
    }

    Fila temp_fila = iniciar_fila();
    if (!temp_fila)
    { /* Tratar erro */
        destruir_arena(arena);
        return;
    }
    void *forma_wrapper_ptr_void;

    while (remover_da_fila(fila_ground, &forma_wrapper_ptr_void))
    {
        FormaStruct *forma = (FormaStruct *)forma_wrapper_ptr_void;
        if (!forma)
            continue;

        if (!forma->foi_destruida) // Só desenha se não foi destruída
        {
            // ... (código de desenho igual à versão anterior, com verificações internas) ...
            switch (forma->tipo)
            {
            case TIPO_CIRCULO:
            {
                Circulo *c = (Circulo *)forma->dados_forma;
                if (c)
                    fprintf(svg, "<circle id='%d' cx='%.1f' cy='%.1f' r='%.1f' stroke='%s' fill='%s'/>\n",
                            forma->id_original, get_x(c), get_y(c), get_raio(c), get_corBorda_circulo(c), get_corPreenchimento_circulo(c));
                break;
            }
            case TIPO_RETANGULO:
            {
                Retangulo *r = (Retangulo *)forma->dados_forma;
                if (r)
                    fprintf(svg, "<rect id='%d' x='%.1f' y='%.1f' width='%.1f' height='%.1f' stroke='%s' fill='%s'/>\n",
                            forma->id_original, get_x_retangulo(r), get_y_retangulo(r), get_largura(r), get_altura(r), get_corBorda_retangulo(r), get_corPreenchimento_retangulo(r));
                break;
            }
            case TIPO_LINHA:
            {
                Linha *l = (Linha *)forma->dados_forma;
                if (l)
                    fprintf(svg, "<line id='%d' x1='%.1f' y1='%.1f' x2='%.1f' y2='%.1f' stroke='%s'/>\n",
                            forma->id_original, get_x1_linha(l), get_y1_linha(l), get_x2_linha(l), get_y2_linha(l), get_cor_linha(l));
                break;
            }
            case TIPO_TEXTO:
            {
                Texto *t = (Texto *)forma->dados_forma;
                if (t)
                {
                    char anchor = get_anchor_texto(t);
                    const char *ancora_svg = "start";
                    if (anchor == 'm' || anchor == 'M')
                        ancora_svg = "middle";
                    else if (anchor == 'e' || anchor == 'E' || anchor == 'f' || anchor == 'F')
                        ancora_svg = "end";
                    else if (anchor == 's' || anchor == 'S' || anchor == 'i' || anchor == 'I')
                        ancora_svg = "start";

                    const char *fill_cor = get_corPreenchimento_texto(t);
                    const char *border_cor = get_corBorda_texto(t);
                    const char *content = get_conteudo_texto(t);

                    fprintf(svg, "<text id='%d' x='%.1f' y='%.1f' stroke='%s' fill='%s' text-anchor='%s'>%s</text>\n",
                            forma->id_original, get_x_texto(t), get_y_texto(t),
                            border_cor ? border_cor : "black",
                            fill_cor ? fill_cor : "black",
                            ancora_svg,
                            content ? content : "");
                }
                break;
            }
            }
        }

        adicionar_na_fila(temp_fila, forma_wrapper_ptr_void);
    }

    while (remover_da_fila(temp_fila, &forma_wrapper_ptr_void))
    {
        adicionar_na_fila(fila_ground, forma_wrapper_ptr_void);
    }

    destruir_fila(temp_fila);

    fprintf(svg, "</svg>\n");

    if (d)
    {
        for (int i = 0; i < disp_conter; i++)
        {

            if (d[i])
            {

                destruir_disparador(d[i]);

                free(d[i]);
                d[i] = NULL;
            }
        }
        free(d);
        d = NULL;
    }

    if (c)
    {
        for (int i = 0; i < car_conter; i++)
        {

            if (c[i] != NULL)
            {

                if (*c[i] != NULL)
                {
                    
                    destruir_carregador(*c[i]);
                }

                free(c[i]);
                c[i] = NULL;
            }
        }
        free(c);
        c = NULL;
    }

    destruir_arena(arena);
    arena = NULL;
}