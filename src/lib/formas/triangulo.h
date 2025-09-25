#ifndef TRISANGULO_H
#define TRISANGULO_H

/*
    Triandulo 

*/

typedef void *Triangulo;


/*
Função para criar um triângulo com as vertises nos pontos p1, p2, p3,de um plano,
com a cor de preenchimento e cor da borda no formato SVG
*/
Triangulo* criar_triangulo(float p1, float p2, float p3, char* corPreenchimento,char *corBorda);   


// Função para calcular a área do triângulo
float area_triangulo(Triangulo* t);
// Função para liberar a memória do triângulo
void liberar_triangulo(Triangulo* t);
#endif