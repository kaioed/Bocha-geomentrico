#ifndef ARGS_HANDLER_H
#define ARGS_HANDLER_H

#include <stdio.h>

/**
 * @brief Obtém o valor de uma opção no formato -opt valor
 *
 * @param argc número de argumentos recebidos pela aplicação (main)
 * @param argv vetor de strings com os argumentos
 * @param opt_name nome da opção (sem o '-') por exemplo: "opt"
 * @return ponteiro para a string do valor encontrado ou NULL se não encontrado
 */
char *obter_valor_opcao(int argc, char *argv[], char *opt_name);

/**
 * @brief Obtém o sufixo do comando (última string que não começa com '-')
 *
 * Essa função remove todos os pares -opt valor antes de buscar o sufixo.
 * Se houver mais de um candidato a sufixo, a função imprime erro e termina o programa.
 *
 * @param argc número de argumentos
 * @param argv vetor de argumentos
 * @return ponteiro para a string do sufixo (ponteiro para argv original) ou NULL se não existir
 */
char *obter_sufixo_comando(int argc, char *argv[]);

#endif
