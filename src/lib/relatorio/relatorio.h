#ifndef RELATORIO_H
#define RELATORIO_H

#include "../geo/geo.h"
#include <stdio.h>

/**
 * @brief Gera o arquivo SVG final com todas as formas do ground para visualização
 * @param ground estrutura contendo todas as formas (originais e clones) do jogo
 * @param svg ponteiro para o arquivo SVG onde a saída será escrita
 */
void relatorio_gerar_svg(Ground ground, FILE* svg);

#endif