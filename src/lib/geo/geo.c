#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geo.h"

void process_geo(FILE *geo, FILE *svg) {
    char comando[4];
    int i;
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

    // Fim do SVG
    fprintf(svg, "</svg>\n");
}
