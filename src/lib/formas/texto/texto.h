#ifndef TEXTO_H
#define TEXTO_H

/*
 * @brief Texto é uma "forma" posicionada em (x, y), com cor, conteúdo e id.
 * Sua "área" é definida como 20.0 * número de caracteres do texto.
 */

typedef void *Texto;

Texto* criar_texto(float x, float y, char* cor, char* conteudo, int id);

/**
 * @brief Cria um texto na posição (x, y), com cor, conteúdo e id.
 * @param id inteiro usado para identificar o texto
 * @param x posição horizontal do texto
 * @param y posição vertical do texto
 * @param cor cor do texto (ex: #000000)
 * @param conteudo string com o texto a ser exibido
 * @return ponteiro para o texto criado
 */

float area_texto(Texto* t);
/**
 * @brief Recebe um ponteiro para um texto e retorna a "área" do texto
 * @param t ponteiro para o texto
 * @return "área" do texto (20.0 * número de caracteres)
 */

void liberar_texto(Texto* t);
/**
 * @brief Libera a memória alocada para o texto
 * @param t ponteiro para o texto
 */

#endif
