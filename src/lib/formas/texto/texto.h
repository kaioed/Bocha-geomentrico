#ifndef TEXTO_H
#define TEXTO_H

/*
 * @brief Texto é uma "forma" posicionada em (x, y), com cor, conteúdo e id.
 * Sua "área" é definida como 20.0 * número de caracteres do texto.
 */

typedef void *Texto;

Texto* criar_texto(float x, float y, char* cor, char* conteudo, const char* fonte, int id);

/**
 * @brief Cria um texto na posição (x, y), com cor, conteúdo e id.
 * @param id inteiro usado para identificar o texto
 * @param x posição horizontal do texto
 * @param y posição vertical do texto
 * @param cor cor do texto (ex: #000000)
 * @param conteudo string com o texto a ser exibido
 * @param fonte nome/família da fonte a ser usada no SVG (ex: "Arial", "Times New Roman")
 * @return ponteiro para o texto criado
 */

float area_texto(const Texto* t);
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

/**
 * @brief Ajusta a fonte do texto em tempo de execução
 * @param t ponteiro para o texto a ser alterado
 * @param fonte nome/família da fonte (ex: "Arial", "Times New Roman"); se NULL, não altera
 */
void set_fonte_texto(Texto* t, const char* fonte);

/**
 * @brief Retorna a fonte atual do texto
 * @param t ponteiro para o texto
 * @return ponteiro para string com o nome da fonte (não alocar/duplicar)
 */
const char* get_fonte_texto(const Texto* t);

/**
 * @brief Retorna o id do texto
 * @param t ponteiro para o texto
 * @return id do texto
 */
int get_id_texto(const Texto* t);

/**
 * @brief Retorna a cor do texto
 * @param t ponteiro para o texto
 * @return ponteiro para string da cor
 */
const char* get_cor_texto(const Texto* t);

/**
 * @brief Retorna o conteúdo do texto
 * @param t ponteiro para o texto
 * @return ponteiro para string do conteúdo
 */
const char* get_conteudo_texto(const Texto* t);

/**
 * @brief Retorna a coordenada x do texto
 * @param t ponteiro para o texto
 * @return coordenada x
 */
float get_x_texto(const Texto* t);

/**
 * @brief Retorna a coordenada y do texto
 * @param t ponteiro para o texto
 * @return coordenada y
 */
float get_y_texto(const Texto* t);

#endif
