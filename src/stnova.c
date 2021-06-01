/**
 * @file stnova.c
 * @author Tiago Amorim     up202004720 
 *         Lucas Freitas    up202006938
 * @remark project repo:    https://github.com/dvalnn/prog2_mieec_flightDreams
 */

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

// ************************** FUNÇÕES AUXILIARES ************************** //

/**
 * @brief Verifica se o pointer passado em ptr é NULL e imprime mensagens de erro.
 *
 * @param ptr pointer a verificar
 * @param msg Mensagem de erro a imprimir 
 * @param origem Mensagem adicional com origem do erro
 * @return int 1 se o ptr for NULL, senão 0
 */
static int check_ptr(void *ptr, const char *msg, const char *origem) {
    if (!ptr) {
        printf("%s", msg);
        printf("[INFO] - Erro originado por: \"%s\"\n", origem);
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief calcula hash com base na seguinte formula:
 *            hash(i) = hash(i-1)* 31 (+) chave[i]
 *        em que hash(0) = 5347
 *
 * @param chave string para o qual se pretende calcular o chave de hash
 * @param tamanho da tabela de dispersão
 * @remark chave[i] é o caracter no indice de i da chave
 * @remark (+) representa "ou exclusivo" que em C é indicado por ^
 */
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

// ************************** INICIALIZAÇÃO DA STRUCT ************************** //

/**
 * @brief insere destino da aresta de no_grafo *origem indexada por index_ligacao na tabela
 *        mapa_destinos correspondente ao nó de origem em questão e guarda o preço do voo. 
 *        Caso o destino seja duplicado, atualiza o preço, caso a nova ligação seja mais barata.
 * 
 * @param md mapa de destinos onde adicionar a ligação
 * @param origem nó de origem do voo
 * @param index_ligacao index do voo no vetor de arestas do nó de origem
 * @return int 0 se for inserida sem problemas, caso contrário -1 
 */
int mapa_insere(mapa_destinos *md, no_grafo *origem, int index_ligacao) {
    if (!md) return -1;

    //* encontra a aresta em questão e calcula o index do destino
    //* no vetor de voos usando a cidade como chave
    aresta_grafo *ligacao = origem->arestas[index_ligacao];
    int index = md->hfunc(ligacao->destino->cidade, md->n_voos);
    ligacao_direta *voo = md->voos[index];

    //* navega a lista criada por eventuais colisões que tenham havido no index calculado da tabela
    while (voo) {
        //* caso o destino a inserir seja duplicado, é apenas atualizado
        //* o index da viagem mais barata, caso necessário
        if (voo->no_destino == ligacao->destino) {
            if (origem->arestas[voo->index_mais_barato]->preco > ligacao->preco)
                voo->index_mais_barato = index_ligacao;
            return 0;
        }
        voo = voo->proximo;
    }

    //* o voo a inserir é uma nova ligação entre o nó de origem e chegada.
    //* é alocado espaço para na memória para uma nova ligação_direta e o index atual da
    //* aresta é guardado como sendo o mais barato, por default
    voo = (ligacao_direta *)malloc(sizeof(*voo));
    voo->index_mais_barato = index_ligacao;
    voo->no_destino = ligacao->destino;

    //* o novo voo criado é inserido no início da lista de colisões
    voo->proximo = md->voos[index];
    md->voos[index] = voo;

    return 0;
}

/**
 * @brief Cria e retorna uma instância inicializada da tabela de dispersão mapa_destinos.
 *        Colisões na tabela são resolvidas recorrendo a encadeamento dos valores numa lista ligada.
 * 
 * @param n_voos tamanho da tabela
 * @return mapa_destinos* apontador para a tabela criada
 */
mapa_destinos *novo_mapa(int n_voos) {
    if (n_voos < 0) return NULL;

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

/**
 * @brief cria e retorna instância inicializada de um elemento *elem, correspondente ao
 *        no_grafo passado como argumento 
 * 
 * @param node no_grafo no qual basear a criação do elemento
 * @return elemento* apontador do novo elemento
 */
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

    //* insere todas as arestas do no_grafo *node no mapa_destinos criado para o novo elemento
    for (int index_aresta = 0; index_aresta < node->tamanho; index_aresta++)
        mapa_insere(elem->todos_os_destinos, node, index_aresta);

    return elem;
}

/**
 * @brief insere no_grafo *node na estrutura st com recurso a funções auxiliares de modo a 
 *        inicializar corretamente todos os campos.
 * 
 * @param st apontador para a estrutura
 * @param node no_grafo a inserir 
 * @return int 0 em caso de sucesso e -1 em caso de erro.
 */
int st_insere(estrutura *st, no_grafo *node) {
    if (!st || !node) return -1;

    //* calcula o index apropriado para o nó no vetor, usando a cidade como chave
    int index = (int)st->hfunc(node->cidade, st->n_origens);
    elemento *elem = st->elementos[index];

    //* navega através da possíveis colisões.
    //* Todos os nós importados do grafo são únicos logo não são testados duplicados
    while (elem)
        elem = elem->proximo;

    //* É criado um novo elemento (elemento *) para inserir na tabela
    elem = novo_elemento(node);
    if (check_ptr(elem, MALLOC_ERROR_MSG, "stnova.c - st_insere() - pos_a_inserir"))
        return -1;

    //* insere no início da lista
    elem->proximo = st->elementos[index];
    st->elementos[index] = elem;

    return 0;
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

// ************************** REMOÇÃO DA STRUCT ************************** //

/**
 * @brief apaga ligacao_direta *voo e liberta toda a memória associada
 * 
 * @param voo 
 */
void apaga_ligacao(ligacao_direta *voo) {
    if (!voo) return;
    ligacao_direta *voo_next = NULL;

    while (voo) {
        voo_next = voo->proximo;
        free(voo);
        voo = voo_next;
    }
}

/**
 * @brief apaga tabela de dispersão mapa_destinos *md e liberta toda a memória associada
 * 
 * @param md 
 */
void mapa_apaga(mapa_destinos *md) {
    if (!md) return;
    for (int i = 0; i < md->n_voos; i++)
        apaga_ligacao(md->voos[i]);
    free(md->voos);
    free(md);
}

/**
 * @brief apaga lista-ligada de "elemento" e liberta toda a memória associada
 * 
 * @param elem 
 */
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

// ************************** PESQUISA ************************** //

char *st_pesquisa(estrutura *st, char *origem, char *destino) {
    if (!st || !origem || !destino) return NULL;

    //* encontra o nó de origem na tabela de origens, através da função de hash
    int index_origem = (int)st->hfunc(origem, st->n_origens);
    elemento *pos_atual = st->elementos[index_origem];

    //* navega através da lista de colisões no index calculado
    while (pos_atual && strcmp(pos_atual->no_de_origem->cidade, origem))
        pos_atual = pos_atual->proximo;

    //* caso a origem não exista ou não existam voos, retorna erro
    if (!pos_atual || !pos_atual->todos_os_destinos->n_voos) return NULL;

    //* encontra o nó de destino na tabela de destinos correspondente à origem escolhida
    //* através da função de hash
    int index_destino = (int)pos_atual->todos_os_destinos->hfunc(destino, pos_atual->todos_os_destinos->n_voos);
    ligacao_direta *voo = pos_atual->todos_os_destinos->voos[index_destino];

    //* navega através da lista de colisões no index calculado
    while (voo && strcmp(voo->no_destino->cidade, destino))
        voo = voo->proximo;

    //* caso o destino não exista para a origem pretendida retorna erro
    if (!voo) return NULL;

    //* retorna o código do melhor voo recorrendo ao index guardado previamente durante
    //* a função de inserção.
    return pos_atual->no_de_origem->arestas[voo->index_mais_barato]->codigo;
}

#undef TRUE
#undef FALSE

#undef REALLOC_ERROR_MSG
#undef MALLOC_ERROR_MSG
