/*****************************************************************/
/*         Tabela de Dispersao | PROG2 | MIEEC | 2020/21         */
/*****************************************************************/

#include "../include/tabdispersao.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/grafo.h"

#define TRUE 1
#define FALSE 0

// * mensagens de erro
#define REALLOC_ERROR_MSG "\n[ERRO] - Falha ao alocar memória. - realloc\n"
#define MALLOC_ERROR_MSG "\n[ERRO] - Falha ao alocar memória. - malloc/calloc\n"
#define FILE_ERROR_MSG "\n[ERRO] - Falha ao abrir ficheiro\n"

static int check_ptr(void *ptr, const char *msg, const char *origem) {
    if (!ptr) {
        printf("%s", msg);
        printf("[INFO] - Erro originado por: \"%s\"\n", origem);
        return TRUE;
    }
    return FALSE;
}

enum ESTADO_CELULA { REMOVIDO = -1,
                     VAZIO,
                     VALIDO };

tabela_dispersao *tabela_nova(int capacidade, hash_func *hfunc, sond_func *sfunc) {
    if (!capacidade || !hfunc || !sfunc)
        return NULL;

    tabela_dispersao *tabela_criada = (tabela_dispersao *)malloc(sizeof(tabela_dispersao));
    if (check_ptr(tabela_criada, MALLOC_ERROR_MSG, "tabdispersao.c - tabela_nova() - tabela_criada"))
        return NULL;

    tabela_criada->tamanho = 0;
    tabela_criada->capacidade = capacidade;
    tabela_criada->hfunc = hfunc;
    tabela_criada->sfunc = sfunc;

    tabela_criada->estado_celulas = (int *)calloc(capacidade, sizeof(int));

    tabela_criada->nos = (no_grafo **)calloc(capacidade, sizeof(no_grafo **));
    if (check_ptr(tabela_criada->nos, MALLOC_ERROR_MSG, "tabdispersao.c - tabela_nova() - tabela_criada-> nos")) {
        free(tabela_criada);
        return NULL;
    }

    return tabela_criada;
}

int tabela_adiciona(tabela_dispersao *td, no_grafo *entrada) {
    if (!td || !entrada || td->tamanho >= td->capacidade) return -1;

    unsigned long hash_index = td->hfunc(entrada->cidade, td->capacidade);
    unsigned long index = hash_index;

    int tentativas = 0;
    while (TRUE) {
        if (td->estado_celulas[index] == VAZIO) {
            td->nos[index] = entrada;
            td->estado_celulas[index] = VALIDO;
            td->tamanho++;
            break;
        }

        tentativas++;
        index = td->sfunc(hash_index, tentativas, td->capacidade);
    }
    return 0;
}

int tabela_remove(tabela_dispersao *td, no_grafo *saida) {
    return -1;
}

void tabela_apaga(tabela_dispersao *td) {
    if (!td)
        return;

    free(td->nos);
    free(td->estado_celulas);
    free(td);
    td = NULL;
}

int tabela_existe(tabela_dispersao *td, const char *cidade) {
    return -1;
}

tabela_dispersao *tabela_carrega(grafo *g, int tamanho) {
    return NULL;
}

/*================================================================================*/
unsigned long sond_rh(int pos, int tentativas, int tamanho) {
    return (pos + tentativas * tentativas) % tamanho;
}

unsigned long hash_krm(const char *chave, int tamanho) {
    int c, t = strlen(chave);
    unsigned long hash = 7;

    for (c = 0; c < t; c++) {
        hash += (int)chave[c];
    }

    return hash % tamanho;

    return 0;
}
