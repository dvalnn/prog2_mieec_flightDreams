/*****************************************************************/
/*    Estrutura nova a implementar | PROG2 | MIEEC | 2020/21     */
/*****************************************************************/

#ifndef _STNOVA_H_
#define _STNOVA_H_

#include "grafo.h"
#include "tabdispersao.h"

/* podem criar mais struct que achem necessárias */

typedef struct
{

} estrutura;

/*
*  cria e inicializa a estrutura criada
*  retorna a estrutura se bem-sucedido ou NULL caso contrário
*/
estrutura *st_nova();

/*
*  importa todo o conteúdo do grafo g para o novo formato de acesso
*  parametro st - apontador para a estrutura criada 
*  parametro g - apontador para o grafo
*  retorna 0 se bem-sucedido ou -1 em contrário
*/
int st_importa_grafo(estrutura *st, grafo *g);

/* 
*  obtém o código de voo do par origem-destino com menor preço 
*  a instância retornada deverá ser mantida, i.e., deverá ficar uma cópia dela no respetivo elemento de ‘st’
*  NOTA: esta função será avaliada pelo tempo de execução!
*  parametro st - apontador para a estrutura
*  parametro origem - nome da cidade de origem dos voos a pesquisar
*  parametro destino - nome da cidade de destino dos voos a pesquisar
*  retorna o código do voo ou em caso de insucesso, retorna NULL
*/
char *st_pesquisa(estrutura *st, char *origem, char *destino);

/*
*  elimina todas as instâncias presentes na estrutura st e desaloca toda a memória da mesma
*  NOTA: esta função será avaliada pelo tempo de execução!
*  parametro st - apontador para a estrutura 
*  retorna 0 se bem-sucedido e -1 se ocorrer algum erro
*/
int st_apaga(estrutura *st);

#endif
