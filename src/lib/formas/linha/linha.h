#ifndef LINHA_H
#define LINHA_H

/*
    Linha é uma forma geométrica definida por dois pontos (x1, y1) e (x2, y2),
    além de cor da linha e um identificador.
*/

typedef void *Linha;

/*
Função para criar uma linha entre os pontos (x1, y1) e (x2, y2),
com cor e id.
*/
Linha* criar_linha(float x1, float y1, float x2, float y2, char* cor, int id);

// Função recebe um ponteiro para uma linha e retorna o comprimento da linha
float comprimento_linha(Linha* l);

// Função recebe um ponteiro para uma linha e retorna a "área" da linha (2.0 * comprimento)
float area_linha(Linha* l);

// Função para liberar a memória alocada para a linha
void liberar_linha(Linha* l);

#endif
