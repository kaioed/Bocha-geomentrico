#include <stdio.h>
#include <stdlib.h>
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
    

        if (entrada_geo == NULL || saida_pasta == NULL) {
            fprintf(stderr, "Uso: %s -f <arquivo.geo> -o <pasta_saida> [-q <arquivo.qry>] [comando]\n", argv[0]);
            return 1;
        }
    
        // Abre o arquivo .geo
        FILE *geo = abrirArquivo(entrada_geo, "r");
        if (geo == NULL) {
            fprintf(stderr, "Erro ao abrir o arquivo .geo: %s\n", entrada_geo);
            return 1;
        }
        
        // Cria o nome do arquivo SVG de saída
        char svg_arqnome[512];
        snprintf(svg_arqnome, sizeof(svg_arqnome), "%s/arq.svg", saida_pasta);
    
        // Abre o arquivo SVG para escrita
        FILE *svg = abrirArquivo(svg_arqnome, "w");
        if (svg == NULL) {
            fprintf(stderr, "Erro ao criar o arquivo SVG: %s\n", svg_arqnome);
            fecharArquivo(geo);
            return 1;
        }
    
        // Processa o arquivo .geo e gera o SVG
        printf("Processando arquivo .geo: %s\n", entrada_geo);
        process_geo(geo, svg);
        printf("Arquivo SVG gerado: %s\n", svg_arqnome);
    
        // Fecha o arquivo SVG
        fecharArquivo(svg);
        
        if (entrada_qry != NULL) {
            printf("Processando arquivo .qry: %s\n", entrada_qry);
            
            // Abre o arquivo .qry
            FILE *qry = abrirArquivo(entrada_qry, "r");
            if (qry == NULL) {
                fprintf(stderr, "Erro ao abrir o arquivo .qry: %s\n", entrada_qry);
            } else {
                char svg_arqnome_qry[512];
                snprintf(svg_arqnome_qry, sizeof(svg_arqnome_qry), "%s/arq-arqconst.svg", saida_pasta);
                FILE *svg_qry = abrirArquivo(svg_arqnome_qry, "w");

                char txt_arqnome[512];
                snprintf(txt_arqnome, sizeof(txt_arqnome), "%s/arq-arqcons.txt", saida_pasta);
                FILE *txt = abrirArquivo(txt_arqnome, "w");

                if (svg_qry == NULL || txt == NULL) {
                    fprintf(stderr, "Erro ao criar arquivos de saída para processamento .qry\n");
                    if (svg_qry == NULL) fprintf(stderr, "  - Erro ao criar: %s\n", svg_arqnome_qry);
                    if (txt == NULL) fprintf(stderr, "  - Erro ao criar: %s\n", txt_arqnome);
                    
                    // Fecha arquivos que foram abertos com sucesso
                    if (svg_qry) fecharArquivo(svg_qry);
                    if (txt) fecharArquivo(txt);
                } else {
                    printf("Processando comandos .qry...\n");
                    // Processa o arquivo .qry com os parâmetros na ordem correta
                    process_qry(qry, svg_qry, geo, txt);
                    printf("Processamento .qry concluído.\n");
                }
                
                // Fecha os arquivos
                fecharArquivo(qry);
                if (svg_qry) fecharArquivo(svg_qry);
                if (txt) fecharArquivo(txt);
            }
        }
        
        // Fecha o arquivo geo (apenas uma vez)
        fecharArquivo(geo);

    return 0;
}