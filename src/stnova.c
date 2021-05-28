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
    nova_tabela->origens = NULL;

    return nova_tabela;
}

mapa_destinos *novo_mapa() {
}

int elemento_insere(elemento *elem, no_grafo *node) {
    if (!elem || !node) return -1;
    elem->proximo = NULL;

    elem->destinos = (objeto *)malloc(sizeof(*elem->destinos));
    if (check_ptr(elem->destinos, MALLOC_ERROR_MSG, "stnova.c - elemento_insere() - elem->destinos"))
        return -1;
}

int st_insere(estrutura *st, no_grafo *node) {
    if (!st || !node) return -1;
    int index = (int)st->hfunc(node->cidade, st->n_origens);

    elemento *pos_a_inserir = st->origens[index];

    // testa a existência de colisões e se o nó a inserir é duplicado
    while (pos_a_inserir) {
        if (pos_a_inserir->destinos->no_de_origem == node)
            return 0;
        pos_a_inserir = pos_a_inserir->proximo;
    }

    //no a inserir n existe
    pos_a_inserir = (elemento *)malloc(sizeof(*pos_a_inserir));
    if (check_ptr(pos_a_inserir, MALLOC_ERROR_MSG, "stnova.c - st_insere() - pos_a_inserir"))
        return -1;

    if (elemento_insere(pos_a_inserir, node) == -1) {
        free(pos_a_inserir);
        return -1;
    }

    return 0;
}

int st_importa_grafo(estrutura *st, grafo *g) {
    if (!st || !g) return -1;

    st->n_origens = g->tamanho;
    st->origens = (elemento **)calloc(st->n_origens, sizeof(*st->origens));
    if (check_ptr(st->origens, MALLOC_ERROR_MSG, "stnova.c - st_importa_grafo - st->origens"))
        return -1;

    for (int i = 0; i < g->tamanho; i++)
        if (st_insere(st, g->nos[i]) == -1) return -1;

    return 0;
}

char *st_pesquisa(estrutura *st, char *origem, char *destino) {
    return NULL;
}

int apaga_destinos(objeto *origem) {
}

void mapa_apaga() {
}

void apaga_origem(elemento *origem) {
    if (!origem) return;
    elemento *origem_next = NULL;

    while (origem) {
        origem_next = origem->proximo;
        apaga_destinos(origem->destinos);
        free(origem);
        origem = origem_next;
    }
}

int st_apaga(estrutura *st) {
    if (!st) return -1;
    for (int i = 0; i < st->n_origens; i++)
        apaga_origem(st->origens[i]);

    free(st->origens);
    free(st);

    return -1;
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

#undef TRUE 1
#undef FALSE 0

#undef REALLOC_ERROR_MSG
#undef MALLOC_ERROR_MSG
