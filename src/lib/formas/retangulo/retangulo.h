#ifndef RETANGULO_H
#define RETANGULO_H

/*
 * @brief Retângulo é uma forma geométrica definida por um ponto de origem (x, y),
 * largura (w) e altura (h), além de cor de preenchimento e cor de borda.
 */

typedef void *Retangulo;

Retangulo* criar_retangulo(float x, float y, float w, float h, char* corPreenchimento, char* corBorda, int id);
/*
Função para criar um retângulo com origem (x, y), largura w, altura h,
cor de preenchimento e cor da borda no formato SVG
*/
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

float area_retangulo(Retangulo* r);
// Função recebe um ponteiro para um retângulo e retorna a área do retângulo
/**
 * @brief Recebe um ponteiro para um retângulo e retorna a área do retângulo
 * @param r ponteiro para o retângulo
 * @return área do retângulo
 */

void liberar_retangulo(Retangulo* r);
// Função para liberar a memória alocada para o retângulo

#endif
