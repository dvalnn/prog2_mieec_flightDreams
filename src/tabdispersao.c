/**
 * @file tabdispersao.c
 * @author Tiago Amorim     up202004720 
 *         Lucas Freitas    up202006938
 * @remark project repo:    https://github.com/dvalnn/prog2_mieec_flightDreams
 */

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

//* mensagens de erro
#define REALLOC_ERROR_MSG "\n[ERRO] - Falha ao alocar memória. - realloc\n"
#define MALLOC_ERROR_MSG "\n[ERRO] - Falha ao alocar memória. - malloc/calloc\n"
#define FILE_ERROR_MSG "\n[ERRO] - Falha ao abrir ficheiro\n"
#define TD_CREATION_ERROR_MSG "\n[ERRO] - Falha ao criar a tabela de dispersão de endereçamento aberto\n"

enum ESTADO_CELULA { REMOVIDO = -1,
                     VAZIO,
                     VALIDO };

static int check_ptr(void *ptr, const char *msg, const char *origem) {
    if (!ptr) {
        printf("%s", msg);
        printf("[INFO] - Erro originado por: \"%s\"\n", origem);
        return TRUE;
    }
    return FALSE;
}

tabela_dispersao *tabela_nova(int capacidade, hash_func *hfunc, sond_func *sfunc) {
    if (!capacidade || !hfunc || !sfunc)
        return NULL;

    tabela_dispersao *tabela_criada = (tabela_dispersao *)malloc(sizeof(*tabela_criada));
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

/**
 * @brief insere no_grafo *entrada na tabela *td num index específico
 * 
 * @param td tabela de dispersão
 * @param entrada apontador do nó de origem
 * @param index índex no vetor de arestas do nó de entrada
 * @return int index em que o nó foi inserido
 */
int tabela_insere(tabela_dispersao *td, no_grafo *entrada, int index) {
    td->estado_celulas[index] = VALIDO;
    td->nos[index] = entrada;
    td->tamanho++;
    return index;
}

int tabela_adiciona(tabela_dispersao *td, no_grafo *entrada) {
    if (!td || !entrada || td->tamanho >= td->capacidade) return -1;

    //* calcula o index da tabela para a o novo novo nó.
    //* caso não haja colisões (index está vazio) insere e retorna index.
    int hash_index = (int)td->hfunc(entrada->cidade, td->capacidade);
    if (td->estado_celulas[hash_index] == VAZIO)
        return tabela_insere(td, entrada, hash_index);

    //* variáveis auxiliares para ajudar com a sondagem, para resolver as colisões
    int index_sond = hash_index;
    int removido_index = -1;
    int tentativas = 1;

    while (TRUE) {
        //* calcula o novo index recorrendo à função de sondagem
        index_sond = td->sfunc(hash_index, tentativas, td->capacidade);

        //* retorna erro caso seja duplicado
        if ((td->estado_celulas[index_sond] == VALIDO) && td->nos[index_sond] == entrada)
            return -1;

        //* caso uma posição REMOVIDO seja encontrada, guarda o index e continua a sondagem
        if (removido_index == -1 && td->estado_celulas[index_sond] == REMOVIDO)
            removido_index = index_sond;

        //* caso uma posição VAZIO seja encontrada, o no é inserido no index vazio
        //* um num index REMOVIDO que tenha sido encontrado previamente
        else if (td->estado_celulas[index_sond] == VAZIO) {
            if (removido_index != -1)
                return tabela_insere(td, entrada, removido_index);
            return tabela_insere(td, entrada, index_sond);
        }
        //* caso a função de sondagem volte ao index inicial sem que o nó tenha sido inserido, retorna erro
        if (index_sond == hash_index) {
            if (removido_index != -1)
                return tabela_insere(td, entrada, removido_index);
            return -1;
        }

        tentativas++;
    }

    return -1;
}

int tabela_remove(tabela_dispersao *td, no_grafo *saida) {
    if (!td || !saida) return -1;

    int hash_index = td->hfunc(saida->cidade, td->capacidade);
    int index_sond = hash_index;
    int tentativas = 0;

    while (TRUE) {
        if (td->estado_celulas[index_sond] == VALIDO && td->nos[index_sond] == saida) {
            td->estado_celulas[index_sond] = REMOVIDO;
            td->nos[index_sond] = NULL;
            td->tamanho--;
            break;
        }

        //* no_grafo *saída não existe na tabela, retorna erro
        if (td->estado_celulas[index_sond] == VAZIO)
            return -1;

        tentativas++;
        index_sond = td->sfunc(hash_index, tentativas, td->capacidade);

        //* caso todas a função de sondagem volte ao index original retornado por td->hfunc
        //* retorna erro de modo a evitar loops infinitos - caso extremo em que só exitem células de
        //* estado -1 e 1 e o nó a remover não existe na tabela.
        if (index_sond == hash_index)
            return -1;
    }
    return 0;
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
    if (!td || !cidade) return -1;

    unsigned long hash_index = td->hfunc(cidade, td->capacidade);
    unsigned long index = hash_index;

    int tentativas = 0;
    while (TRUE) {
        if (td->estado_celulas[index] == VALIDO && !strcmp(td->nos[index]->cidade, cidade))
            return index;
        else if (td->estado_celulas[index] == VAZIO)
            return -1;

        tentativas++;
        index = td->sfunc(hash_index, tentativas, td->capacidade);
    }
    return -1;
}

tabela_dispersao *tabela_carrega(grafo *g, int capacidade) {
    if (!g || !capacidade) return NULL;

    tabela_dispersao *td_a_preencher = tabela_nova(capacidade, hash_krm, sond_rh);
    if (check_ptr(td_a_preencher, TD_CREATION_ERROR_MSG, "tabdispersao.c - tabela_carrega() - td_a_preencher"))
        return NULL;
    for (int i = 0; g->tamanho > i; i++)
        if (tabela_adiciona(td_a_preencher, g->nos[i]) == -1)
            return NULL;

    return td_a_preencher;
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

#undef TRUE
#undef FALSE

#undef REALLOC_ERROR_MSG
#undef MALLOC_ERROR_MSG
#undef FILE_ERROR_MSG
#undef CREATION_ERROR_MSG