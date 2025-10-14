#ifndef GEO_H
#define GEO_H

#include <stdio.h>

/**
 * @brief Processa um arquivo .geo e escreve o equivalente SVG no arquivo de saída
 *
 * O formato aceito pelo parser é:
 *  - c id x y r corBorda corPreenchimento
 *  - r id x y w h corBorda corPreenchimento
 *  - l id x1 y1 x2 y2 cor
 *  - t id x y corBorda corPreenchimento a texto...  (a: i/m/f -> anchor start/middle/end)
 *  - ts fFamily fWeight fSize  (configura estilo de texto)
 *
 * @param geo arquivo de entrada (.geo)
 * @param svg arquivo de saída (.svg)
 */
void process_geo(FILE *geo, FILE *svg);

#endif
