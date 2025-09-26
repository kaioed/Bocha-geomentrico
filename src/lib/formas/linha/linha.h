#ifndef LINHA_H
#define LINHA_H

/*
 * @brief Linha é uma forma geométrica definida por dois pontos (x1, y1) e (x2, y2),
 * além de cor da linha e um identificador.
 */

typedef void *Linha;


/**
 * @brief Cria uma linha entre os pontos (x1, y1) e (x2, y2), com cor e id.
 * @param id inteiro usado para identificar a linha
 * @param x1 posição x do ponto inicial
 * @param y1 posição y do ponto inicial
 * @param x2 posição x do ponto final
 * @param y2 posição y do ponto final
 * @param cor cor da linha (ex: #000000)
 * @return ponteiro para a linha criada
 */
Linha* criar_linha(float x1, float y1, float x2, float y2, char* cor, int id);

/**
 * @brief Recebe um ponteiro para uma linha e retorna o comprimento da linha
 * @param l ponteiro para a linha
 * @return comprimento da linha
 */
float comprimento_linha(Linha* l);

/**
 * @brief Recebe um ponteiro para uma linha e retorna a "área" da linha (2.0 * comprimento)
 * @param l ponteiro para a linha
 * @return "área" da linha
 */
float area_linha(Linha* l);

/**
 * @brief Libera a memória alocada para a linha
 * @param l ponteiro para a linha
 */
void liberar_linha(Linha* l);

#endif
