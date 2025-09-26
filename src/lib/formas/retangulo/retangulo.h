#ifndef RETANGULO_H
#define RETANGULO_H

/*
 * @brief Retângulo é uma forma geométrica definida por um ponto de origem (x, y),
 * largura (w) e altura (h), além de cor de preenchimento e cor de borda.
 */

typedef void *Retangulo;

Retangulo* criar_retangulo(float x, float y, float w, float h, char* corPreenchimento, char* corBorda, int id);
/**
 * @brief Cria um retângulo com os parâmetros fornecidos
 * @param id inteiro usado para identificar a forma criada
 * @param x float utilizado para definir a posição horizontal no gráfico
 * @param y float utilizado para definir a posição vertical no gráfico
 * @param w largura do retângulo
 * @param h altura do retângulo
 * @param corBorda cor da borda do retângulo (ex: #000000)
 * @param corPreenchimento cor de preenchimento do retângulo (ex: #FFFFFF)
 * @return ponteiro para o retângulo criado
 */

float area_retangulo(const Retangulo* r);
/**
 * @brief Recebe um ponteiro para um retângulo e retorna a área do retângulo
 * @param r ponteiro para o retângulo
 * @return área do retângulo
 */

void liberar_retangulo(Retangulo* r);
/**
 * @brief Libera a memória alocada para o retângulo
 * @param r ponteiro para o retângulo
 */

float get_largura(const Retangulo* r);
/**
 * @brief Retorna a largura do retângulo
 * @param r ponteiro para o retângulo
 * @return largura do retângulo
 */

float get_altura(const Retangulo* r);
/**
 * @brief Retorna a altura do retângulo
 * @param r ponteiro para o retângulo
 * @return altura do retângulo
 */

float get_x_retangulo(const Retangulo* r);
/**
 * @brief Retorna a coordenada x do retângulo
 * @param r ponteiro para o retângulo
 * @return coordenada x do retângulo
 */

float get_y_retangulo(const Retangulo* r);
/**
 * @brief Retorna a coordenada y do retângulo
 * @param r ponteiro para o retângulo
 * @return coordenada y do retângulo
 */

int get_id_retangulo(const Retangulo* r);
/**
 * @brief Retorna o id do retângulo
 * @param r ponteiro para o retângulo
 * @return id do retângulo
 */

const char* get_corPreenchimento_retangulo(const Retangulo* r);
/**
 * @brief Retorna a cor de preenchimento do retângulo
 * @param r ponteiro para o retângulo
 * @return ponteiro para string da cor de preenchimento
 */

const char* get_corBorda_retangulo(const Retangulo* r);
/**
 * @brief Retorna a cor da borda do retângulo
 * @param r ponteiro para o retângulo
 * @return ponteiro para string da cor da borda
 */

#endif
