/*****************************************************************/
/*         Tabela de Dispersao | PROG2 | MIEEC | 2020/21         */
/*****************************************************************/

#ifndef TABDISPERSAO_H
#define TABDISPERSAO_H

#include "grafo.h"

/* tabela de dispersão baseada em encadeamento */

typedef unsigned long hash_func(const char *, int);

typedef unsigned long sond_func(int, int, int);

/*
*  estrutura tabela_dispersao para armazenar dados relativos a uma tabela de dispersão
*/
typedef struct
{
    hash_func *hfunc;    /* apontador para função de dispersão */
    sond_func *sfunc;    /* apontador para função de sondagem */
    int capacidade;      /* número de posições alocadas de ‘nos’ */
    int tamanho;         /* número de posições preenchidas de ‘nos’ */
    no_grafo **nos;      /* vetor de apontadores para nó */
    int *estado_celulas; /* vetor de indicadores de estado 0:vazio, 1:válido, -1:removido */
} tabela_dispersao;

/**
 * @brief cria uma tabela de dispersão
 * 
 * @param capacidade capacidade da tabela de dispersão
 * @param hfunc apontador para função de dispersão a ser usada nesta tabela
 * @param sfunc apontador para função de sondagem a ser usada nesta tabela
 * @return tabela_dispersao* uma tabela de dispersão vazia que usa a função de dispersão e a função de sondagem indicadas
 */
tabela_dispersao *tabela_nova(int capacidade, hash_func *hfunc, sond_func *sfunc);

/**
 * @brief adiciona um novo nó à tabela de dispersão
 * 
 * @param td tabela onde adicionar o nó
 * @param entrada nó a ser adicionado
 * @return int o índice do nó adicionado se for bem sucedido e -1 em caso contrário
 */
int tabela_adiciona(tabela_dispersao *td, no_grafo *entrada);

/**
 * @brief remove um nó da tabela não desalocando a memória do mesmo
 * 
 * @param td tabela onde remover o nó
 * @param saida nó a ser removido
 * @return int 0 se a remoção for bem sucedido e -1 em caso contrário
 */
int tabela_remove(tabela_dispersao *td, no_grafo *saida);

/**
 * @brief elimina uma tabela, removendo todos os nós e apaga a memória da estrutura da tabela de dispersão 
 * 
 * @param td tabela de dispersão a ser apagada
 */
void tabela_apaga(tabela_dispersao *td);

/**
 * @brief verifica se determinada cidade existe na tabela
 * 
 * @param td tabela onde procurar o valor
 * @param cidade cidade a procurar na tabela
 * @return int o índice do nó encontrado na tabela se for bem sucedido e -1 em caso contrário
 */
int tabela_existe(tabela_dispersao *td, const char *cidade);

/**
 * @brief cria e preenche uma nova tabela de dispersão a partir do grafo
 * 
 * @param g apontador para o grafo
 * @param capacidade capacidade da nova tabela de dispersão 
 * @return tabela_dispersao* a tabela de dispersão criada ou NULL se ocorrer algum erro
 */
tabela_dispersao *tabela_carrega(grafo *g, int capacidade);

/**
 * @brief calcula hash com base na seguinte formula:
 *            hash(i) = hash(i-1) + chave[i]
 *        em que hash(0) = 7 
 * 
 * @param chave string para a qual se pretende calcular o chave de hash
 * @param tamanho tamanho da tabela de dispersão
 * @return unsigned long hash value
 * @remark nota - chave[i] é o caracter no índice de i da chave
 */
unsigned long hash_krm(const char *chave, int tamanho);

unsigned long sond_rh(int pos, int tentativas, int tamanho);

#endif
