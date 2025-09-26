#ifndef CIRCULO_H
#define CIRCULO_H

/*
    Círculo e uma forma geométrica definida por um ponto central e um raio
    o circulo deve possuir uma cor de preenchimento e uma cor de borda
*/

typedef void *Circulo;

/*
Função para criar um círculo com centro (x, y), raio r,
cor de preenchimento e cor da borda no formato SVG
*/
Circulo* criar_circulo(float x, float y, float r, char* corPreenchimento, char* corBorda, int id);

// Função recebe um ponteiro para um círculo e retorna a área do círculo
float area_circulo(Circulo* c);

// Função para liberar a memória alocada para o círculo
void liberar_circulo(Circulo* c);

#endif
