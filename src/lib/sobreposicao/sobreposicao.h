#ifndef SOBREPOSICAO_H
#define SOBREPOSICAO_H

#include <stdbool.h>
#include "../qry/qry.h" 

/**
 * @brief Verifica se duas formas se sobrepõem
 * @param dados1 ponteiro para os dados da primeira forma
 * @param tipo1 tipo da primeira forma (TIPO_CIRCULO, TIPO_RETANGULO, TIPO_LINHA ou TIPO_TEXTO)
 * @param dados2 ponteiro para os dados da segunda forma
 * @param tipo2 tipo da segunda forma (TIPO_CIRCULO, TIPO_RETANGULO, TIPO_LINHA ou TIPO_TEXTO)
 * @return true se as formas se sobrepõem, false caso contrário
 */
bool formas_sobrepoem(void *dados1, TipoForma tipo1, void *dados2, TipoForma tipo2);

#endif