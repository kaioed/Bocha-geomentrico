#ifndef LINHA_H
#define LINHA_H

#include <stdbool.h>

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
 * @param dashed define se a linha é pontilhada (true) ou solida (false)
 * @return ponteiro para a linha criada
 */
Linha* criar_linha(float x1, float y1, float x2, float y2, char* cor, bool dashed, int id);

/**
 * @brief Recebe um ponteiro para uma linha e retorna o comprimento da linha
 * @param l ponteiro para a linha
 * @return comprimento da linha
 */
float comprimento_linha(const Linha* l);

/**
 * @brief Recebe um ponteiro para uma linha e retorna a "área" da linha (2.0 * comprimento)
 * @param l ponteiro para a linha
 * @return "área" da linha
 */
float area_linha(const Linha* l);

/**
 * @brief Libera a memória alocada para a linha
 * @param l ponteiro para a linha
 */
void liberar_linha(Linha* l);

/**
 * @brief Retorna a coordenada x do ponto inicial da linha
 * @param l ponteiro para a linha
 * @return coordenada x do ponto inicial
 */
float get_x1_linha(const Linha* l);

/**
 * @brief Retorna a coordenada y do ponto inicial da linha
 * @param l ponteiro para a linha
 * @return coordenada y do ponto inicial
 */
float get_y1_linha(const Linha* l);

/**
 * @brief Retorna a coordenada x do ponto final da linha
 * @param l ponteiro para a linha
 * @return coordenada x do ponto final
 */
float get_x2_linha(const Linha* l);

/**
 * @brief Retorna a coordenada y do ponto final da linha
 * @param l ponteiro para a linha
 * @return coordenada y do ponto final
 */
float get_y2_linha(const Linha* l);

/**
 * @brief Retorna o id da linha
 * @param l ponteiro para a linha
 * @return id da linha
 */
int get_id_linha(const Linha* l);

/**
 * @brief Retorna a cor da linha
 * @param l ponteiro para a linha
 * @return ponteiro para string da cor
 */
const char* get_cor_linha(const Linha* l);

/**
 * @brief Verifica se a linha é pontilhada
 * @param l ponteiro para a linha
 * @return true se pontilhada, false caso contrário
 */
bool is_dashed_linha(const Linha* l);

/**
 * @brief Define a cor da linha
 * @param l ponteiro para a linha
 * @param cor nova cor da linha (ex: #000000)
 */
void set_cor_linha(Linha* l, const char* cor);

#endif