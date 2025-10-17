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

// Helper: safe realloc wrapper
static void *safe_realloc(void *ptr, size_t newSize) {
    void *p = realloc(ptr, newSize);
    if (!p && newSize != 0) {
        // Realloc falhou; prefer não abortar a aplicação aqui, apenas retornar NULL
        return NULL;
    }
    return p;
}

void ler_geo_armazenar(FILE *geo,
                       Circulo*** circulos, int *nCirculos,
                       Retangulo*** retangulos, int *nRetangulos,
                       Linha*** linhas, int *nLinhas,
                       Texto*** textos, int *nTextos) {
    if (!geo) return;

    char line[1024];

    // Inicializa retornos
    if (circulos) { *circulos = NULL; *nCirculos = 0; }
    if (retangulos) { *retangulos = NULL; *nRetangulos = 0; }
    if (linhas) { *linhas = NULL; *nLinhas = 0; }
    if (textos) { *textos = NULL; *nTextos = 0; }

    while (fgets(line, sizeof(line), geo)) {
        // remove espaços iniciais
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\n' || *p == '#') continue; // vazio ou comentário

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
                    } else {
                        // falha em realloc: liberar objeto recém-criado
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
                    } else {
                        liberar_linha(obj);
                    }
                }
            }
        } else if (type == 't') {
            // tratamos texto: t id x y corB corP a texto com espaços
            int id; float x, y; char corB[128], corP[128], anchor[16];
            int consumed = 0;
            int matched = sscanf(rest, " %d %f %f %127s %127s %15s %n", &id, &x, &y, corB, corP, anchor, &consumed);
            if (matched >= 6 && textos) {
                char *texto = rest + consumed;
                while (*texto == ' ' || *texto == '\t') texto++;
                // remove newline final
                size_t len = strlen(texto);
                while (len > 0 && (texto[len-1] == '\n' || texto[len-1] == '\r')) texto[--len] = '\0';
                // Se conteúdo for vazio, usar string vazia
                if (!texto) texto = "";
                Texto* obj = criar_texto(x, y, corP, texto, NULL, id);
                if (obj) {
                    Texto **arr = safe_realloc(*textos, ((*nTextos)+1) * sizeof(Texto*));
                    if (arr) {
                        arr[*nTextos] = obj;
                        *textos = arr;
                        (*nTextos)++;
                    } else {
                        liberar_texto(obj);
                    }
                }
            }
        } else {
            // outros comandos/linhas ignorados por enquanto
            continue;
        }
    }
}


void process_qry(FILE *svg, FILE *txt, FILE *qry,Circulo*** circulos, int *nCirculos,
                       Retangulo*** retangulos, int *nRetangulos,
                       Linha*** linhas, int *nLinhas,
                       Texto*** textos, int *nTextos)
{


    // Início do SVG
    fprintf(svg, "<svg xmlns='http://www.w3.org/2000/svg'>\n");


    Disparador **d;
    Carregador **c;
    int id, n, disp_conter, car_conter, i;
    float x_disp, y_disp, x_car, y_car;
    char *comando;
    disp_conter = 0;
    car_conter = 0;

    while (fscanf(qry, "%s", comando) != -1)
    {
        if (strcmp(comando, "pd") == 0)
        {
            fscanf(qry, "%d %d %d", &id, &x_disp, &y_disp);
            d[disp_conter] = criar_disparador(id, x_disp, y_disp);
            disp_conter++;
        }
        else if (strcmp(comando, "lc") == 0)
        {
            fscanf(qry, "%d %d  ", &id, &n);
            c[car_conter] = criar_carredor(id);
            car_conter++;
            carregar_carregador(qry, c[car_conter - 1], n);

        }
        else if (strcmp(comando, "atch") == 0)
        {
            fscanf(qry, "%d %d %s", &id, &n, comando);
            for (i = 0; i < disp_conter; i++)
            {
                if (disparador_get_id(d[i]) == id)
                {
                    carregar_disparador(d[i], n, comando);
                    break;
                }
            }
        }
    }

    // Fim do SVG
    fprintf(svg, "</svg>\n");
}
