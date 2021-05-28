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

    // testa a existência de colisões e se o nó a inserir é duplicado
    while (elem) {
        if (elem->no_de_origem == node)
            return 0;
        elem = elem->proximo;
    }

    //no a inserir n existe
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

    // st->n_origens = g->tamanho;
    st->n_origens = 1;
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
    return NULL;
}
// tabela_dispersao* tabela_nova(int tamanho, hash_func *hfunc)
// {
//     /* aloca memoria para a estrutura tabela_dispersao */
//     if (tamanho < 1 || hfunc == NULL)
//         return NULL;

//     tabela_dispersao *t = (tabela_dispersao*) malloc(sizeof (tabela_dispersao));
//     if (t == NULL)
//         return NULL;

//     /* aloca memoria para os elementos */
//     t->elementos = (td_elemento **) calloc(tamanho, sizeof (td_elemento*));
//     if (t->elementos == NULL) {
//         free(t);
//         return NULL;
//     }

//     t->tamanho = tamanho;
//     t->hfunc = hfunc;

//     return t;
// }

// void tabela_apaga(tabela_dispersao *td)
// {
//     int i;
//     td_elemento *elem, *aux;

//     if (td == NULL) return;

//     /* para cada entrada na tabela */
//     for (i = 0; i < td->tamanho; i++)
//     {
//         /* e enquanto existirem elementos nessa entrada */
//         elem = td->elementos[i];
//         while (elem != NULL)
//         {
//             /* liberta cada elemento */
//             aux = elem->proximo;
//             free(elem->obj);
//             free(elem);
//             elem = aux;
//         }
//     }

//     /* liberta vetor e estrutura */
//     free(td->elementos);
//     free(td);
// }

// int tabela_insere(tabela_dispersao *td, const char *chave, const char *valor)
// {
//     int index;
//     td_elemento * elem;

//     if (!td || !chave || !valor) return TABDISPERSAO_ERRO;

//     /* calcula hash para a string a adicionar */
//     index = td->hfunc(chave, td->tamanho);

//     /* verifica se chave ja' existe na lista */
//     elem = td->elementos[index];
//     while (elem != NULL && strcmp(elem->obj->chave, chave) != 0)
//         elem = elem->proximo;

//     if (elem == NULL)
//     {
//         /* novo elemento, chave nao existe na lista */

//         /* aloca memoria para o elemento */
//         elem = (td_elemento*) malloc(sizeof (td_elemento));
//         if (elem == NULL)
//             return TABDISPERSAO_ERRO;

//         /* aloca memoria para o objeto */
//         elem->obj = (objeto*) malloc(sizeof (objeto));
//         if (elem->obj == NULL)
//             return TABDISPERSAO_ERRO;

//         /* copia chave e valor */
//         strcpy(elem->obj->chave, chave);
//         strcpy(elem->obj->valor, valor);

//         /* insere no inicio da lista */
//         elem->proximo = td->elementos[index];
//         td->elementos[index] = elem;
//     } else {
//         /* chave repetida, simplesmente atualiza o valor */
//         strcpy(elem->obj->valor, valor);
//     }

//     return TABDISPERSAO_OK;
// }

// int tabela_remove(tabela_dispersao *td, const char *chave)
// {
//     int index;
//     td_elemento * elem, * aux;

//     if (!td) return TABDISPERSAO_ERRO;

//     /* calcula hash para a string a adicionar */
//     index = td->hfunc(chave, td->tamanho);

//     elem = td->elementos[index];
//     aux = NULL;

//     /* para cada elemento na posicao index */
//     while(elem != NULL)
//     {
//         /* se for a string que se procura, e' removida */
//         if (strcmp(elem->obj->chave, chave) == 0)
//         {
//             /* se nao for a primeira da lista */
//             if (aux != NULL)
//                 aux->proximo = elem->proximo;
//             else
//                 td->elementos[index] = elem->proximo;
//             free(elem->obj);
//             free(elem);

//             return TABDISPERSAO_OK;
//         }

//         aux = elem;
//         elem = elem->proximo;
//     }
//     return TABDISPERSAO_NAOEXISTE;
// }

// int tabela_existe(tabela_dispersao *td, const char *chave)
// {
//     int index;
//     td_elemento * elem;

//     if (!chave || !td) return TABDISPERSAO_ERRO;

//     /* calcula hash para a string a adicionar */
//     index = td->hfunc(chave, td->tamanho);

//     /* percorre lista na posicao index e retorna 1 se encontrar */
//     elem = td->elementos[index];

//     while(elem != NULL)
//     {
//         if (!strcmp(elem->obj->chave, chave))
//             return TABDISPERSAO_EXISTE;
//         elem = elem->proximo;
//     }
//     return TABDISPERSAO_NAOEXISTE;
// }

// const char* tabela_valor(tabela_dispersao *td, const char *chave)
// {
//     int index;
//     td_elemento * elem;

//     if (!td) return NULL;

//     /* calcula hash para a string a adicionar */
//     index = td->hfunc(chave, td->tamanho);

//     /* percorre lista na posicao index e retorna 1 se encontrar */
//     elem = td->elementos[index];

//     while(elem != NULL)
//     {
//         if (!strcmp(elem->obj->chave, chave))
//             return elem->obj->valor;
//         elem = elem->proximo;
//     }
//     return NULL;
// }

// int tabela_esvazia(tabela_dispersao *td)
// {
//     int i;
//     td_elemento * elem, * aux;

//     if (!td) return TABDISPERSAO_ERRO;

//     /* para cada entrada na tabela */
//     for (i = 0; i < td->tamanho; i++)
//     {
//         /* apaga todos os elementos da entrada */
//         elem = td->elementos[i];
//         while(elem != NULL)
//         {
//             aux = elem->proximo;
//             free(elem->obj);
//             free(elem);
//             elem = aux;
//         }
//         td->elementos[i] = NULL;
//     }
//     return TABDISPERSAO_OK;
// }

// int tabela_numelementos(tabela_dispersao *td)
// {
//     int i, count = 0;
//     td_elemento * elem;

//     if (!td) return TABDISPERSAO_ERRO;

//     /* percorre todos os elementos da tabela */
//     for (i = 0; i < td->tamanho; i++)
//     {
//         elem = td->elementos[i];
//         while(elem != NULL)
//         {
//             elem = elem->proximo;
//             count++;
//         }
//     }
//     return count;
// }

// objeto * tabela_elementos(tabela_dispersao *td, int *n)
// {
//     objeto *v;
//     int i, j;
//     td_elemento * elem;
//     objeto * valor;

//     *n = tabela_numelementos(td);

//     if ((*n) <= 0)
//         return NULL;

//     v = (objeto*) malloc(sizeof(objeto)*(*n));
//     if (!v)
//         return NULL;

//     for (i=0,j=0; i < td->tamanho; i++)
//     {
//         if (td->elementos[i])
//         {
//             elem = td->elementos[i];
//             while (elem)
//             {
//                 valor = elem->obj;
//                 v[j++] = *valor;
//                 elem = elem->proximo;
//             }
//         }
//     }
//     return v;
// }

// unsigned long hash_krm(const char* chave, int tamanho)
// {
//     unsigned long h, i;
//     h = 7;
//     i = 0;

//     while(chave[i])
//         h += chave[i++];

//     return h % tamanho;
// }

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

// /*================================================================================*/
// void mostraTabela(tabela_dispersao *td)
// {
//     int i;
//     td_elemento * elem;
//     printf("TABELA DE DISPERSAO (%d elementos)\n", tabela_numelementos(td));
//     for (i = 0; i < td->tamanho; i++)
//     {
//         if (td->elementos[i])
//         {
//             printf("\t[%2d]", i);
//             elem = td->elementos[i];
//             while (elem)
//             {
//                 printf(" : [\"%s\" :: \"%s\"]", elem->obj->chave, elem->obj->valor);
//                 elem = elem->proximo;
//             }
//             printf("\n");
//         }
//     }
//     printf("\n");
// }

#undef TRUE
#undef FALSE

#undef REALLOC_ERROR_MSG
#undef MALLOC_ERROR_MSG
