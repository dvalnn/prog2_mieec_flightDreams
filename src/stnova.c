/*****************************************************************/
/*    Estrutura nova a implementar | PROG2 | MIEEC | 2020/21     */
/*****************************************************************/

#include "../include/stnova.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

// * mensagens de erro
#define REALLOC_ERROR_MSG "\n[ERRO] - Falha ao alocar memória. - realloc\n"
#define MALLOC_ERROR_MSG "\n[ERRO] - Falha ao alocar memória. - malloc/calloc\n"

static int check_ptr(void *ptr, const char *msg, const char *origem) {
    if (!ptr) {
        printf("%s", msg);
        printf("[INFO] - Erro originado por: \"%s\"\n", origem);
        return TRUE;
    }
    return FALSE;
}

estrutura *st_nova() {
    estrutura *nova_tabela = (estrutura *)malloc(sizeof(*nova_tabela));
    if (check_ptr(nova_tabela, MALLOC_ERROR_MSG, "stnova.c - st_nova() - nova_tabela"))
        return NULL;

    nova_tabela->hfunc = hash_djbm;
    nova_tabela->n_origens = 0;
    nova_tabela->elementos = NULL;

    return nova_tabela;
}

mapa_destinos *novo_mapa(int n_voos) {
    mapa_destinos *novo = (mapa_destinos *)malloc(sizeof(*novo));
    if (check_ptr(novo, MALLOC_ERROR_MSG, "stnova.c - st_nova() - nova_tabela"))
        return NULL;

    novo->hfunc = hash_djbm;
    novo->n_voos = n_voos;
    novo->voos = (ligacao_direta **)calloc(novo->n_voos, sizeof(*novo->voos));
    if (check_ptr(novo->voos, MALLOC_ERROR_MSG, "stnova.c - novo_mapa() - novo->voos")) {
        free(novo);
        return NULL;
    }

    return novo;
}

int mapa_insere(mapa_destinos *md, no_grafo *origem, int index_ligacao) {
    if (!md) return -1;

    aresta_grafo *ligacao = origem->arestas[index_ligacao];
    int index = md->hfunc(ligacao->destino->cidade, md->n_voos);
    ligacao_direta *voo = md->voos[index];

    while (voo) {
        if (voo->no_destino == ligacao->destino) {
            if (origem->arestas[voo->index_mais_barato]->preco > ligacao->preco)
                voo->index_mais_barato = index_ligacao;
            return 0;
        }
        voo = voo->proximo;
    }

    voo = (ligacao_direta *)malloc(sizeof(*voo));
    voo->index_mais_barato = index_ligacao;
    voo->no_destino = ligacao->destino;

    voo->proximo = md->voos[index];
    md->voos[index] = voo;

    return 0;
}

elemento *novo_elemento(no_grafo *node) {
    if (!node) return NULL;

    elemento *elem = (elemento *)malloc(sizeof(*elem));

    elem->proximo = NULL;
    elem->no_de_origem = node;
    elem->todos_os_destinos = novo_mapa(node->tamanho);

    if (!elem->todos_os_destinos) {
        free(elem);
        return NULL;
    }

    for (int i = 0; i < node->tamanho; i++) {
        mapa_insere(elem->todos_os_destinos, node, i);
    }

    return elem;
}

int st_insere(estrutura *st, no_grafo *node) {
    if (!st || !node) return -1;

    int index = (int)st->hfunc(node->cidade, st->n_origens);
    elemento *elem = st->elementos[index];

    //* testa a existência de colisões e se o nó a inserir é duplicado
    while (elem) {
        if (elem->no_de_origem == node)
            return 0;
        elem = elem->proximo;
    }

    //* no a inserir n existe
    elem = novo_elemento(node);
    if (check_ptr(elem, MALLOC_ERROR_MSG, "stnova.c - st_insere() - pos_a_inserir"))
        return -1;

    //* insere no início da lista
    elem->proximo = st->elementos[index];
    st->elementos[index] = elem;

    return 0;
}

void apaga_ligacao(ligacao_direta *voo) {
    if (!voo) return;
    ligacao_direta *voo_next = NULL;

    while (voo) {
        voo_next = voo->proximo;
        free(voo);
        voo = voo_next;
    }
}

void mapa_apaga(mapa_destinos *md) {
    if (!md) return;
    for (int i = 0; i < md->n_voos; i++)
        apaga_ligacao(md->voos[i]);
    free(md->voos);
    free(md);
}

void apaga_elemento(elemento *elem) {
    if (!elem) return;
    elemento *origem_next = NULL;

    while (elem) {
        origem_next = elem->proximo;
        mapa_apaga(elem->todos_os_destinos);
        free(elem);
        elem = origem_next;
    }
}

int st_apaga(estrutura *st) {
    if (!st) return -1;

    for (int i = 0; i < st->n_origens; i++)
        apaga_elemento(st->elementos[i]);

    free(st->elementos);
    free(st);

    return -1;
}

int st_importa_grafo(estrutura *st, grafo *g) {
    if (!st || !g) return -1;

    st->n_origens = g->tamanho;
    st->elementos = (elemento **)calloc(st->n_origens, sizeof(*st->elementos));
    if (check_ptr(st->elementos, MALLOC_ERROR_MSG, "stnova.c - st_importa_grafo - st->origens"))
        return -1;

    for (int i = 0; i < g->tamanho; i++)
        if (st_insere(st, g->nos[i]) == -1) {
            free(st->elementos);
            return -1;
        }

    return 0;
}

char *st_pesquisa(estrutura *st, char *origem, char *destino) {
    if (!st || !origem || !destino) return NULL;

    int index_origem = (int)st->hfunc(origem, st->n_origens);
    elemento *pos_atual = st->elementos[index_origem];

    while (pos_atual && strcmp(pos_atual->no_de_origem->cidade, origem))
        pos_atual = pos_atual->proximo;
    

    if (!pos_atual || !pos_atual->todos_os_destinos->n_voos) return NULL;

    int index_destino = (int)pos_atual->todos_os_destinos->hfunc(destino, pos_atual->todos_os_destinos->n_voos);
    ligacao_direta *voo = pos_atual->todos_os_destinos->voos[index_destino];

    while (voo && strcmp(voo->no_destino->cidade, destino))
        voo = voo->proximo;

    if (!voo) return NULL;

    return pos_atual->no_de_origem->arestas[voo->index_mais_barato]->codigo;
}

unsigned long hash_djbm(const char *chave, int tamanho) {
    unsigned long h, i;
    i = 0;
    h = 5347;

    while (chave[i]) {
        h *= 31;
        h ^= chave[i++];
        h &= 0xffffffff; /* consideram-se apenas 32 bits */
    }

    return h % tamanho;
}

#undef TRUE
#undef FALSE

#undef REALLOC_ERROR_MSG
#undef MALLOC_ERROR_MSG
