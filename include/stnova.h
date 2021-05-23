/*****************************************************************/
/*    Estrutura nova a implementar | PROG2 | MIEEC | 2020/21     */
/*****************************************************************/

#ifndef _STNOVA_H_
#define _STNOVA_H_

#include "grafo.h"
#include "tabdispersao.h"

/* podem criar mais struct que achem necessárias*/

typedef struct
{
} estrutura;

/**
 * @brief cria e inicializa a estrutura criada
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
 * @brief obtém o código de voo do par origem-destino com menor preço 
 *        a instância retornada deverá ser mantida, i.e., deverá ficar uma cópia dela no respetivo elemento de ‘st’
 *        NOTA: esta função será avaliada pelo tempo de execução!
 * 
 * @param st apontador para a estrutura
 * @param origem nome da cidade de origem dos voos a pesquisar
 * @param destino nome da cidade de destino dos voos a pesquisar
 * @return char* código do voo ou em caso de insucesso, retorna NULL
 */
char *st_pesquisa(estrutura *st, char *origem, char *destino);

/**
 * @brief elimina todas as instâncias presentes na estrutura st e desaloca toda a memória da mesma 
 *        NOTA: esta função será avaliada pelo tempo de execução!
 * @param st apontador para a estrutura 
 * @return int 0 se bem-sucedido e -1 se ocorrer algum erro
 */
int st_apaga(estrutura *st);

#endif
