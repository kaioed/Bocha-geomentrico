#ifndef CIRCULO_H
#define CIRCULO_H

/*
    Círculo
*/

typedef void *Circulo;

/*
Função para criar um círculo com centro (x, y), raio r,
cor de preenchimento e cor da borda no formato SVG
*/
Circulo* criar_circulo(float x, float y, float r, char* corPreenchimento, char* corBorda);

// Função para calcular a área do círculo
float area_circulo(Circulo* c);
// Função para liberar a memória do círculo
void liberar_circulo(Circulo* c);

#endif
