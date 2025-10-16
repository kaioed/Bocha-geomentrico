
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../manipilarArq/arquivo.h"
#include "qry.h"
#include "../geo/geo.h"
#include "../disparador/disparador.h"

void process_qry(FILE *qry, FILE *svg, FILE *geo,FILE *txt)
{
    if (!qry || !svg||!geo)
        return;

        char comando[4];
    int i, l,p;
    double x, y, r, w, h, x1, y1, x2, y2;
    char corb[32], corp[32], a, texto[256];
    char fFamily[32], fWeight[16];
    double fSize;


    // Início do SVG
    fprintf(svg, "<svg xmlns='http://www.w3.org/2000/svg'>\n");

    // Lê o arquivo linha por linha
    while (fscanf(geo, "%s", comando) != EOF) {

        if (strcmp(comando, "c") == 0) {
            fscanf(geo, "%d %lf %lf %lf %s %s", &i, &x, &y, &r, corb, corp);
            fprintf(svg, "<circle id='%d' cx='%lf' cy='%lf' r='%lf' stroke='%s' fill='%s'/>\n",
                    i, x, y, r, corb, corp);
        }
        else if (strcmp(comando, "r") == 0) {
            fscanf(geo, "%d %lf %lf %lf %lf %s %s", &i, &x, &y, &w, &h, corb, corp);
            fprintf(svg, "<rect id='%d' x='%lf' y='%lf' width='%lf' height='%lf' stroke='%s' fill='%s'/>\n",
                    i, x, y, w, h, corb, corp);
        }
        else if (strcmp(comando, "l") == 0) {
            fscanf(geo, "%d %lf %lf %lf %lf %s", &i, &x1, &y1, &x2, &y2, corb);
            fprintf(svg, "<line id='%d' x1='%lf' y1='%lf' x2='%lf' y2='%lf' stroke='%s'/>\n",
                    i, x1, y1, x2, y2, corb);
        }
        else if (strcmp(comando, "t") == 0) {
            fscanf(geo, "%d %lf %lf %s %s %c", &i, &x, &y, corb, corp, &a);
            fgets(texto, sizeof(texto), geo); // lê o resto da linha (texto)
            texto[strcspn(texto, "\n")] = '\0'; // remove o '\n'

            char *anchor;
            if (a == 'i') anchor = "start";
            else if (a == 'm') anchor = "middle";
            else anchor = "end";

            fprintf(svg, "<text id='%d' x='%lf' y='%lf' stroke='%s' fill='%s' text-anchor='%s'>%s</text>\n",
                    i, x, y, corb, corp, anchor, texto + 1); // +1 remove espaço inicial
        }
        else if (strcmp(comando, "ts") == 0) {
            fscanf(geo, "%s %s %lf", fFamily, fWeight, &fSize);
            fprintf(svg, "<style> text { font-family:%s; font-weight:%s; font-size:%lfpx; } </style>\n",
                    fFamily, fWeight, fSize);
        }
    }

    char linha[512];
    Disparador **d = NULL;
    int d_count = 0;
    Carregador **c = NULL;
    int c_count = 0;
    // Cabeçalho do SVG

    while (fgets(linha, sizeof(linha), qry))
    {

        linha[strcspn(linha, "\n")] = '\0';

        if (strcmp(linha, "pd") == 0)
        {
            fscanf(qry, "%d %lf %lf", &i, &x, &y);
            Disparador **tmp = realloc(d, sizeof(*d) * (d_count + 1));
            if (!tmp) {
                // alocacao falhou; continuar sem adicionar
            } else {
                d = tmp;
                d[d_count] = criar_disparador(i, (int)x, (int)y);
                d_count++;
            }
        }
        else if (strcmp(linha, "lc") == 0)
        {
             fscanf(qry, "%d %lf", &i, &x);
             Carregador **tmpc = realloc(c, sizeof(*c) * (c_count + 1));
             if (!tmpc) {
                 // alocacao falhou; continuar
             } else {
                 c = tmpc;
                 c[c_count] = criar_carredor(i);
                 /* carregar para esse carregador 'x' vezes usando o arquivo geo aberto */
                 carregar_carregador(geo, c[c_count], (int)x);
                 c_count++;
             }
        }
        else if (strcmp(linha, "atch") == 0)
        {
            fscanf(qry, "%d %d %d", &i, &l, &p);
            int idx;
            for (idx = 0; idx < d_count; idx++) {
                if (d[idx] != NULL && disparador_get_id(d[idx]) == i) {
                    break;
                }
            }
            /* Verifica se encontramos o disparador */
            if (idx >= d_count || d[idx] == NULL) {
                /* disparador não encontrado; ignorar o comando attach */
            } else {
                /* Procura os carregadores e associa; evita acessar fora do vetor
                   e interrompe cedo quando ambos já foram encontrados */
                int assigned_left = 0, assigned_right = 0;
                for (int idy = 0; idy < c_count && !(assigned_left && assigned_right); idy++) {
                    if (c[idy] == NULL) continue;
                    if (!assigned_left && carregador_get_id(c[idy]) == l) {
                        disparador_set_carregador_esq(d[idx], c[idy]);
                        assigned_left = 1;
                    }
                    if (!assigned_right && carregador_get_id(c[idy]) == p) {
                        disparador_set_carregador_dir(d[idx], c[idy]);
                        assigned_right = 1;
                    }
                }
            }
        }
        else if (strcmp(linha, "shft") == 0)
        {
            fprintf(svg, "<!-- comando:  -->\n");
        }
        else if (strcmp(linha, "dps") == 0)
        {
            fprintf(svg, "<!-- comando:  -->\n");
        }
        else if (strcmp(linha, "rjd") == 0)
        {
            fprintf(svg, "<!-- comando:  -->\n");
        }
        else if(strcmp(linha, "calc") == 0)
        {
            fprintf(svg, "<!-- comando:  -->\n");
        }
        else
        {
           continue;
        }
    }

    // Fechamento do SVG
    fprintf(svg, "</svg>\n");
}
