#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <errno.h>   
#include "lib/formas/circulo/circulo.h"
#include "lib/formas/retangulo/retangulo.h"
#include "lib/formas/linha/linha.h"
#include "lib/formas/texto/texto.h"
#include "lib/pilha/pilha.h"
#include "lib/fila/fila.h"
#include "lib/arg/args.h" 
#include "lib/geo/geo.h"
#include "lib/manipilarArq/arquivo.h"
#include "lib/qry/qry.h"
#include "lib/disparador/disparador.h"




int main(int argc, char *argv[]) {

    const char *entrada_geo  = obter_valor_opcao(argc, argv, "f");
    const char *saida_pasta = obter_valor_opcao(argc, argv, "o");
    const char *entrada_qry  = obter_valor_opcao(argc, argv, "q");
    const char *prefixo_pasta = obter_valor_opcao(argc, argv, "e"); 

    char *geo_path_completo = NULL;
    char *saida_path_completo = NULL;
    char *qry_path_completo = NULL;

   
    if (prefixo_pasta != NULL) {
        size_t prefixo_len = strlen(prefixo_pasta);
        int precisa_barra = (prefixo_len > 0 && prefixo_pasta[prefixo_len - 1] != '/');

        if (entrada_geo != NULL) {
            size_t geo_len = strlen(entrada_geo);
            geo_path_completo = (char *)malloc(prefixo_len + geo_len + 2); 
            if (precisa_barra) {
                sprintf(geo_path_completo, "%s/%s", prefixo_pasta, entrada_geo);
            } else {
                sprintf(geo_path_completo, "%s%s", prefixo_pasta, entrada_geo);
            }
            entrada_geo = geo_path_completo; 
        }

        if (saida_pasta != NULL) {
            size_t saida_len = strlen(saida_pasta);
            saida_path_completo = (char *)malloc(prefixo_len + saida_len + 2);
            if (precisa_barra) {
                sprintf(saida_path_completo, "%s/%s", prefixo_pasta, saida_pasta);
            } else {
                sprintf(saida_path_completo, "%s%s", prefixo_pasta, saida_pasta);
            }
            saida_pasta = saida_path_completo; 
        }

        if (entrada_qry != NULL) {
            size_t qry_len = strlen(entrada_qry);
            qry_path_completo = (char *)malloc(prefixo_len + qry_len + 2);
            if (precisa_barra) {
                sprintf(qry_path_completo, "%s/%s", prefixo_pasta, entrada_qry);
            } else {
                sprintf(qry_path_completo, "%s%s", prefixo_pasta, entrada_qry);
            }
            entrada_qry = qry_path_completo; 
        }
    }
    
    
    if (entrada_geo == NULL || saida_pasta == NULL) {
        fprintf(stderr, "Uso: %s -f <arquivo.geo> -o <pasta_saida> [-q <arquivo.qry>] [-e <prefixo_pasta>]\n", argv[0]);
       
        free(geo_path_completo);
        free(saida_path_completo);
        free(qry_path_completo);
        return 1;
    }


    if (entrada_geo == NULL || saida_pasta == NULL) {
        fprintf(stderr, "Uso: %s -f <arquivo.geo> -o <pasta_saida> [-q <arquivo.qry>]\n", argv[0]);
        return 1;
    }

    FILE *geo = abrirArquivo(entrada_geo, "r");
  

    
    char svg_arqnome[512];
    snprintf(svg_arqnome, sizeof(svg_arqnome), "%s/arq.svg", saida_pasta);

   
    FILE *svg = abrirArquivo(svg_arqnome, "w");
    
    
    printf("Processando arquivo .geo: %s\n", entrada_geo);
    Ground ground = process_geo(geo, svg);
    printf("Arquivo SVG inicial gerado: %s\n", svg_arqnome);

    fecharArquivo(svg); // Fecha SVG inicial
    fecharArquivo(geo); // Fecha GEO

    
    if (entrada_qry != NULL) {
        printf("Processando arquivo .qry: %s\n", entrada_qry);

        FILE *qry_file = abrirArquivo(entrada_qry, "r");

        
        char svg_final_nome[512];
        snprintf(svg_final_nome, sizeof(svg_final_nome), "%s/arq-arqcons.svg", saida_pasta);
        FILE *svg_final = abrirArquivo(svg_final_nome, "w");

        char txt_nome[512];
        snprintf(txt_nome, sizeof(txt_nome), "%s/arq-arqcons.txt", saida_pasta);
        FILE *txt_file = abrirArquivo(txt_nome, "w");

        printf("Processando comandos .qry...\n");

        process_qry(qry_file, svg_final, ground, txt_file);


        fecharArquivo(qry_file);
        fecharArquivo(svg_final);
        fecharArquivo(txt_file);
    } else {
         printf("Nenhum arquivo .qry fornecido. Processamento concluído após .geo.\n");
    }

    
    destruir_ground(ground);
    printf("Memória liberada.\n");

    return 0;
}