#ifndef CIRCULO_H
#define CIRCULO_H

/*
 * @brief Círculo é uma forma geométrica definida por um ponto central (x, y) e um raio.
 * O círculo deve possuir uma cor de preenchimento e uma cor de borda.
 */

typedef void *Circulo;

Circulo* criar_circulo(float x, float y, float r, char* corPreenchimento, char* corBorda, int id);
/**
 * @brief Cria um círculo com os parâmetros fornecidos
 * @param id inteiro usado para identificar a forma criada
 * @param x float utilizado para definir a posição horizontal do centro
 * @param y float utilizado para definir a posição vertical do centro
 * @param r raio do círculo
 * @param corBorda cor da borda do círculo (ex: #000000)
 * @param corPreenchimento cor de preenchimento do círculo (ex: #FFFFFF)
 * @return ponteiro para o círculo criado
 */

float area_circulo(Circulo* c);
/**
 * @brief Recebe um ponteiro para um círculo e retorna a área do círculo
 * @param c ponteiro para o círculo
 * @return área do círculo
 */

void liberar_circulo(Circulo* c);
/**
 * @brief Libera a memória alocada para o círculo
 * @param c ponteiro para o círculo
 */

#endif
