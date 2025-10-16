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
        process_geo(geo, svg);
    
        // Fecha os arquivos
        fecharArquivo(geo);
        fecharArquivo(svg);
    
        if(entrada_qry != NULL){
            // Abre o arquivo .qry
            FILE *qry = abrirArquivo(entrada_qry, "r");

            char svg_arqnome_qry[512];
            snprintf(svg_arqnome_qry, sizeof(svg_arqnome_qry), "%s/arq-arqconst.svg", saida_pasta);
            FILE *svg_qry = abrirArquivo(svg_arqnome_qry, "w");
    
            char txt_arqnome[512];
            snprintf(txt_arqnome, sizeof(txt_arqnome), "%s/arq-arqcons.txt", saida_pasta);
            FILE *txt = abrirArquivo(txt_arqnome, "w");
            
            // Processa o arquivo .qry e gera o SVG
           // process_qry(qry, svg_qry,geo,txt);
        
            // Fecha os arquivos
            fecharArquivo(qry);
        }

    return 0;
}