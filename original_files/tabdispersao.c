/*****************************************************************/
/*         Tabela de Dispersao | PROG2 | MIEEC | 2020/21         */
/*****************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tabdispersao.h"
#include "grafo.h"

tabela_dispersao *tabela_nova(int capacidade, hash_func *hfunc, sond_func *sfunc)
{
    return NULL;
}

int tabela_adiciona(tabela_dispersao *td, no_grafo *entrada)
{
    return -1;
}

int tabela_remove(tabela_dispersao *td, no_grafo *saida)
{
    return -1;
}

void tabela_apaga(tabela_dispersao *td)
{
}

int tabela_existe(tabela_dispersao *td, const char *cidade)
{
    return -1;
}

tabela_dispersao *tabela_carrega(grafo *g, int tamanho)
{
    return NULL;
}

/*================================================================================*/
unsigned long sond_rh(int pos, int tentativas, int tamanho)
{

    return (pos + tentativas * tentativas) % tamanho;
}

unsigned long hash_krm(const char *chave, int tamanho)
{
    int c, t = strlen(chave);
    unsigned long hash = 7;

    for (c = 0; c < t; c++)
    {
        hash += (int)chave[c];
    }

    return hash % tamanho;
}
