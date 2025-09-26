#ifndef RETANGULO_H
#define RETANGULO_H

/*
	Retângulo é uma forma geométrica definida por um ponto de origem (x, y),
	largura (w) e altura (h), além de cor de preenchimento e cor de borda.
*/

typedef void *Retangulo;

/*
Função para criar um retângulo com origem (x, y), largura w, altura h,
cor de preenchimento e cor da borda no formato SVG
*/
Retangulo* criar_retangulo(float x, float y, float w, float h, char* corPreenchimento, char* corBorda, int id);

// Função recebe um ponteiro para um retângulo e retorna a área do retângulo
float area_retangulo(Retangulo* r);

// Função para liberar a memória alocada para o retângulo
void liberar_retangulo(Retangulo* r);

#endif
