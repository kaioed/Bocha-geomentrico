#ifndef TEXTO_H
#define TEXTO_H

/*
 * @brief Texto é uma "forma" posicionada em (x, y), com cor, conteúdo e id.
 * Sua "área" é definida como 20.0 * número de caracteres do texto.
 * MODIFICADO: Agora inclui corBorda, corPreenchimento e anchor, como no Projeto 1.
 */

typedef void *Texto;

/**
 * @brief Cria um texto na posição (x, y), com cores, âncora, conteúdo e id.
 * @param id inteiro usado para identificar o texto
 * @param x posição horizontal do texto
 * @param y posição vertical do texto
 * @param corBorda cor da borda do texto (ex: #000000)
 * @param corPreenchimento cor de preenchimento do texto (ex: #FFFFFF)
 * @param anchor caractere de âncora (ex: 'm', 'i', 'f')
 * @param conteudo string com o texto a ser exibido
 * @param fonte nome/família da fonte (ignorado no P1, mantido para compatibilidade)
 * @return ponteiro para o texto criado
 */
Texto* criar_texto(float x, float y, char* corBorda, char* corPreenchimento, char anchor, char* conteudo, const char* fonte, int id);


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
 * @brief Retorna a cor de preenchimento do texto
 * @param t ponteiro para o texto
 * @return ponteiro para string da cor de preenchimento
 */
const char* get_corPreenchimento_texto(const Texto* t);

/**
 * @brief Retorna a cor da borda do texto
 * @param t ponteiro para o texto
 * @return ponteiro para string da cor da borda
 */
const char* get_corBorda_texto(const Texto* t);

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

/**
 * @brief Retorna o caractere de âncora do texto
 * @param t ponteiro para o texto
 * @return caractere de âncora
 */
char get_anchor_texto(const Texto* t);

#endif