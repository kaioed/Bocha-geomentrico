#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "lib/formas/circulo/circulo.h"
#include "lib/formas/retangulo/retangulo.h"
#include "lib/formas/linha/linha.h"
#include "lib/formas/texto/texto.h"
#include "lib/pilha/pilha.h"
#include "lib/fila/fila.h"
#include "lib/arg/arg.h"
#include "lib/geo/geo.h"
#include "lib/manipilarArq/arquivo.h"
#include "lib/qry/qry.h"
#include "lib/disparador/disparador.h"

void criarDiretorioSeNaoExiste(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0777) != 0) {
            printf("Erro: não foi possível criar a pasta de saída %s\n", path);
            exit(1);
        }
    }
}


static const char* find_filename_start(const char* path) {
    if (path == NULL) return NULL;
    const char* last_slash = strrchr(path, '/');
    if (last_slash) {
        return last_slash + 1; 
    }
    return path; 
}


static char* get_basename_alloc(const char* path) {
    const char* filename = find_filename_start(path);
    if (filename == NULL) return NULL;

    const char* last_dot = strrchr(filename, '.');
    
    size_t length;
    if (last_dot && last_dot > filename) {
        length = last_dot - filename; 
    } else {
        length = strlen(filename); 
    }

    char* basename_str = (char*)malloc(length + 1);
    if (!basename_str) {
        fprintf(stderr, "Erro: Falha ao alocar memoria para basename\n");
        exit(1);
    }
    
    strncpy(basename_str, filename, length);
    basename_str[length] = '\0';
    return basename_str;
}




int main(int argc, char *argv[]) {

    const char *entrada_geo     = obter_valor_opcao(argc, argv, "f");
    const char *saida_pasta     = obter_valor_opcao(argc, argv, "o");
    const char *entrada_qry     = obter_valor_opcao(argc, argv, "q");
    const char *prefixo_pasta   = obter_valor_opcao(argc, argv, "e");

    char *geo_path_completo = NULL;
    char *qry_path_completo = NULL;

    if (prefixo_pasta != NULL) {
        int precisa_barra = prefixo_pasta[strlen(prefixo_pasta) - 1] != '/';

        if (entrada_geo) {
            geo_path_completo = malloc(strlen(prefixo_pasta) + strlen(entrada_geo) + 2);
            sprintf(geo_path_completo, "%s%s%s",
                    prefixo_pasta,
                    precisa_barra ? "/" : "",
                    entrada_geo);
            entrada_geo = geo_path_completo;
        }

        if (entrada_qry) {
            qry_path_completo = malloc(strlen(prefixo_pasta) + strlen(entrada_qry) + 2);
            sprintf(qry_path_completo, "%s%s%s",
                    prefixo_pasta,
                    precisa_barra ? "/" : "",
                    entrada_qry);
            entrada_qry = qry_path_completo;
        }
    }

    if (entrada_geo == NULL || saida_pasta == NULL) {
        fprintf(stderr, "Uso: %s -f <arquivo.geo> -o <pasta_saida> [-q <arquivo.qry>] [-e <prefixo>]\n", argv[0]);
        free(geo_path_completo);
        free(qry_path_completo);
        return 1;
    }

    
    char* geo_basename = get_basename_alloc(entrada_geo);
    char* qry_basename = NULL;
    if (entrada_qry) {
        qry_basename = get_basename_alloc(entrada_qry);
    }


    criarDiretorioSeNaoExiste(saida_pasta);

    FILE *geo = abrirArquivo(entrada_geo, "r");

    char svg_arqnome[512];
    
    snprintf(svg_arqnome, sizeof(svg_arqnome), "%s/%s.svg", saida_pasta, geo_basename);
    FILE *svg = abrirArquivo(svg_arqnome, "w");

    Ground ground = process_geo(geo, svg);

    fecharArquivo(svg);
    fecharArquivo(geo);

    
    if (entrada_qry != NULL && qry_basename != NULL) {
        FILE *qry_file = abrirArquivo(entrada_qry, "r");

        char svg_final_nome[512];
        
        snprintf(svg_final_nome, sizeof(svg_final_nome), "%s/%s-%s.svg", saida_pasta, geo_basename, qry_basename);
        FILE *svg_final = abrirArquivo(svg_final_nome, "w");

        char txt_nome[512];
        // MODIFICADO: Usa "geo-qry.txt"
        snprintf(txt_nome, sizeof(txt_nome), "%s/%s-%s.txt", saida_pasta, geo_basename, qry_basename);
        FILE *txt_file = abrirArquivo(txt_nome, "w");

        process_qry(qry_file, svg_final, ground, txt_file);

        fecharArquivo(qry_file);
        fecharArquivo(svg_final);
        fecharArquivo(txt_file);
    }

    destruir_ground(ground);

   
    free(geo_basename);
    if (qry_basename) {
        free(qry_basename);
    }

    free(geo_path_completo);
    free(qry_path_completo);

    return 0;
}