#ifndef ELEMENTO_H
#define ELEMENTO_H

#include <stdbool.h>
#include "../qry/qry.h" 

/*
 * @brief Elemento é um tipo opaco que encapsula uma forma com seu tipo, ID, posição e estado.
 */
typedef void* Elemento;

/**
 * @brief Cria um wrapper (encapsulador) para uma forma com tipo, ID, posição e dados associados
 * @param id identificador numérico do elemento
 * @param tipo tipo da forma (TIPO_CIRCULO, TIPO_RETANGULO, TIPO_LINHA ou TIPO_TEXTO)
 * @param dados ponteiro para os dados da forma específica
 * @param x coordenada x do elemento
 * @param y coordenada y do elemento
 * @return ponteiro opaco para o elemento criado
 */
Elemento elemento_criar_wrapper(int id, TipoForma tipo, void* dados, float x, float y);

/**
 * @brief Obtém o tipo da forma encapsulada no elemento
 * @param e ponteiro para o elemento
 * @return tipo da forma (TIPO_CIRCULO, TIPO_RETANGULO, TIPO_LINHA ou TIPO_TEXTO)
 */
TipoForma elemento_get_tipo(Elemento e);

/**
 * @brief Obtém o ID original da forma do elemento
 * @param e ponteiro para o elemento
 * @return ID da forma original
 */
int elemento_get_id_original(Elemento e);

/**
 * @brief Obtém os dados (void*) da forma específica encapsulada
 * @param e ponteiro para o elemento
 * @return ponteiro genérico para os dados da forma
 */
void* elemento_get_dados(Elemento e);

/**
 * @brief Obtém a coordenada x do elemento
 * @param e ponteiro para o elemento
 * @return coordenada x
 */
float elemento_get_x(Elemento e);

/**
 * @brief Obtém a coordenada y do elemento
 * @param e ponteiro para o elemento
 * @return coordenada y
 */
float elemento_get_y(Elemento e);

/**
 * @brief Verifica se o elemento foi marcado como destruído
 * @param e ponteiro para o elemento
 * @return true se destruído, false caso contrário
 */
bool elemento_foi_destruido(Elemento e);

/**
 * @brief Define o status de destruição do elemento
 * @param e ponteiro para o elemento
 * @param status true para marcar como destruído, false caso contrário
 */
void elemento_set_destruido(Elemento e, bool status);

/**
 * @brief Calcula a área do elemento baseado no tipo de forma encapsulada
 * @param e ponteiro para o elemento
 * @return área calculada da forma
 */
float elemento_calcular_area(Elemento e);

/**
 * @brief Cria um clone do elemento em nova posição com possível troca de cores
 * @param original ponteiro para o elemento original a ser clonado
 * @param x nova coordenada x do clone
 * @param y nova coordenada y do clone
 * @param nova_cor_borda nova cor de borda (NULL para manter original)
 * @param trocar_cores se true, inverte cores de preenchimento e borda
 * @return ponteiro para o novo elemento clonado
 */
Elemento elemento_clonar(Elemento original, float x, float y, const char* nova_cor_borda, bool trocar_cores);

#endif