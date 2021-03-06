/**
 * @file stnova.h
 * @author Tiago Amorim     up202004720 
 *         Lucas Freitas    up202006938
 * @remark project repo:    https://github.com/dvalnn/prog2_mieec_flightDreams
 */

/*****************************************************************/
/*    Estrutura nova a implementar | PROG2 | MIEEC | 2020/21     */
/*****************************************************************/

#ifndef _STNOVA_H_
#define _STNOVA_H_

#include "grafo.h"

typedef unsigned long hash_func(const char *, int);

//* Tabela Secundária
typedef struct ligacao_direta {
    no_grafo *no_destino;
    int index_mais_barato;
    struct ligacao_direta *proximo;
} ligacao_direta;

typedef struct mapa_destinos {
    hash_func *hfunc;
    ligacao_direta **voos;
    int n_voos;
} mapa_destinos;

//* Tabela Principal
typedef struct origem {
    no_grafo *no_de_origem;
    mapa_destinos *todos_os_destinos;
    struct origem *proximo;
} elemento;

typedef struct mapa_origens {
    hash_func *hfunc;
    elemento **elementos;
    int n_origens;
} estrutura;

/**
 * @brief cria e inicializa um mapa baseado em tabelas de dispersão que para cada par de char *cidade (origem, destino)
 *        associa o voo direto mais barato entre elas -- desempenha uma funcionalidade semelhante a um dicionário 
 *        do tipo {(origem, destino) : codigo_melhor_voo} em Python. 
 *        Complexidade de pesquisa: O(1) (admitindo que a quantidade de colisões na tabela é desprezável)
 *   
 * @return estrutura* se bem-sucedido ou NULL caso contrário
 */
estrutura *st_nova();

/**
 * @brief importa todo o conteúdo do grafo g para o novo formato de acesso
 * 
 * @param st apontador para a estrutura criada 
 * @param g apontador para o grafo
 * @return int 0 se bem-sucedido ou -1 em contrário
 */
int st_importa_grafo(estrutura *st, grafo *g);

/**
 * @brief obtém o código de voo do par origem-destino com menor preço.
 *        Complexidade de pesquisa: O(1) (admitindo que a quantidade de colisões na tabela é desprezável)
 * 
 * @param st apontador para a estrutura
 * @param origem nome da cidade de origem dos voos a pesquisar
 * @param destino nome da cidade de destino dos voos a pesquisar
 * @return char* código do voo ou em caso de insucesso, retorna NULL
 */
char *st_pesquisa(estrutura *st, char *origem, char *destino);

/**
 * @brief elimina todas as instâncias presentes na estrutura st e desaloca toda a memória da mesma.
 *        
 * @param st apontador para a estrutura 
 * @return int 0 se bem-sucedido e -1 se ocorrer algum erro
 */
int st_apaga(estrutura *st);

#endif