/*****************************************************************/
/*          Grafo direcionado | PROG2 | MIEEC | 2020/21          */
/*****************************************************************/

#include "../include/grafo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/heap.h"

#define TRUE 1
#define FALSE 0

// * mensagens de erro
#define REALLOC_ERROR_MSG "\n[ERRO] - Falha ao realocar memória.\n"

int check_ptr(void *ptr, const char *msg, const char *origem) {
    if (!ptr) {
        printf("%s", msg);
        printf("[INFO] - Erro originado por: \"%s\"\n", origem);
        return TRUE;
    }
    return FALSE;
}

grafo *grafo_novo() {
    grafo *g = (grafo *)malloc(sizeof(grafo));
    g->tamanho = 0;
    g->nos = NULL;

    return g;
}

no_grafo *encontra_no(grafo *g, char *cidade) {
    if (!g || !cidade)
        return NULL;

    // pesquisa por cidade no vetor de nós
    for (int i = 0; i < g->tamanho; i++) {
        if (strcmp(g->nos[i]->cidade, cidade) == 0)
            return g->nos[i];
    }
    return NULL;
}

int encontra_no_idx(grafo *g, char *cidade) {
    if (!g || !cidade)
        return -1;

    // pesquisa por cidade no vetor de nós
    for (int i = 0; i < g->tamanho; i++) {
        if (strcmp(g->nos[i]->cidade, cidade) == 0)
            return i;
    }
    return -1;
}

no_grafo *no_insere(grafo *g, char *cidade) {
    if (!g || !cidade)
        return NULL;

    // verifica se o nó já existe
    no_grafo *no = encontra_no(g, cidade);
    if (no)
        return NULL;

    // cria o novo nó para o user
    no = (no_grafo *)malloc(sizeof(no_grafo));
    if (!no)
        return NULL;

    // aloca espaço para o campo cidade
    no->cidade = (char *)malloc((strlen(cidade) + 1) * sizeof(char));
    if (!no->cidade) {
        free(no);
        return NULL;
    }
    // inicializa todos os campos
    strcpy(no->cidade, cidade);
    no->tamanho = 0;
    no->arestas = NULL;
    no->p_acumulado = 0;
    no->anterior = NULL;
    no->dataatualizada = NULL;

    // insere o nó criado no final do vetor de nós
    g->nos = (no_grafo **)realloc(g->nos, (g->tamanho + 1) * sizeof(no_grafo *));
    if (!g->nos) {
        free(no->cidade);
        free(no);
        return NULL;
    }
    // incrementa o tamanho do numero de nós e insere no vetor de apontadores de nós
    g->tamanho++;
    g->nos[g->tamanho - 1] = no;

    return no;
}

// função auxiliar que retorna 1 se existir a aresta para destino ou 0,
// se a aresta não existir, -1 se der erro
int existe_aresta(no_grafo *origem, no_grafo *destino, char *codigo) {
    if (!origem || !destino)
        return -1;

    //pesquisa em todas as arestas se existe
    for (int i = 0; i < origem->tamanho; i++) {
        if ((origem->arestas[i]->destino == destino) && ((strcmp(origem->arestas[i]->codigo, codigo) == 0)))
            return 1;
    }

    return 0;
}

/**
 * @brief tem função semelhante a existe_aresta(), mas retorna um apontador para a (primeira) aresta encontrada.
 *        Caso código seja NULL, não é tido em conta para a seleção de aresta.
 * 
 * @param origem 
 * @param destino 
 * @param codigo 
 * @return aresta_grafo* apontador para a aresta encontrada
 */
aresta_grafo *encontra_aresta(no_grafo *origem, no_grafo *destino, char *codigo) {
    if (!origem || !destino)
        return NULL;

    //pesquisa em todas as arestas se existe
    for (int i = 0; i < origem->tamanho; i++) {
        if ((origem->arestas[i]->destino == destino) && !strcmp(origem->arestas[i]->codigo, codigo))
            return origem->arestas[i];
    }

    return NULL;
}

int cria_aresta(no_grafo *origem, no_grafo *destino, char *codigo, char *companhia, data partida, data chegada, float preco, int lugares) {
    if (!origem || !destino || !codigo || !companhia)
        return -1;

    if (preco < 0 || lugares < 0)
        return -1;

    // verifica se a ligação já existe
    if (existe_aresta(origem, destino, codigo) > 0)
        return -1;

    // cria a nova ligação
    aresta_grafo *ag = (aresta_grafo *)malloc(sizeof(aresta_grafo));
    if (!ag)
        return -1;

    ag->destino = destino;
    ag->preco = preco;
    ag->lugares = lugares;
    // aloca espaço para o código
    ag->codigo = (char *)malloc((strlen(codigo) + 1) * sizeof(char));
    if (!ag->codigo) {
        free(ag);
        return -1;
    }
    strcpy(ag->codigo, codigo);
    // aloca espaço para o companhia
    ag->companhia = (char *)malloc((strlen(companhia) + 1) * sizeof(char));
    if (!ag->companhia) {
        free(ag->codigo);
        free(ag);
        return -1;
    }
    strcpy(ag->companhia, companhia);

    // inicializa todos os campos
    ag->partida.tm_year = partida.tm_year;
    ag->partida.tm_mon = partida.tm_mon;
    ag->partida.tm_mday = partida.tm_mday;
    ag->partida.tm_hour = partida.tm_hour;
    ag->partida.tm_min = partida.tm_min;
    ag->partida.tm_sec = 0;
    ag->partida.tm_isdst = 0;

    // inicializa todos os campos
    ag->chegada.tm_year = chegada.tm_year;
    ag->chegada.tm_mon = chegada.tm_mon;
    ag->chegada.tm_mday = chegada.tm_mday;
    ag->chegada.tm_hour = chegada.tm_hour;
    ag->chegada.tm_min = chegada.tm_min;
    ag->chegada.tm_sec = 0;
    ag->chegada.tm_isdst = 0;

    // insere a nova ligação no vetor de ligações
    origem->arestas = (aresta_grafo **)realloc(origem->arestas, (origem->tamanho + 1) * sizeof(aresta_grafo *));
    if (!origem->arestas) {
        free(ag->codigo);
        free(ag->companhia);
        free(ag);
        return -1;
    }
    origem->tamanho++;
    origem->arestas[origem->tamanho - 1] = ag;

    return 0;
}

/**
 * @brief troca a posição entre dois nós do grafo
 * 
 * @param a &no_grafo * a
 * @param b &no_grafo * b
 */
void no_grafo_swap(no_grafo **a, no_grafo **b) {
    no_grafo *aux;
    aux = *a;
    *a = *b;
    *b = aux;
}

/**
 * @brief troca a posição entre duas arestas de um nó
 * 
 * @param a &aresta_grafo * a
 * @param b &aresta_grafo * b
 */
void aresta_grafo_swap(aresta_grafo **a, aresta_grafo **b) {
    aresta_grafo *aux;
    aux = *a;
    *a = *b;
    *b = aux;
}

/**
 * @brief remove uma aresta do vetor de arestas do no, conservando ordenação.
 * 
 * @param node nó de onde remover a aresta
 * @param aresta_index indíce da aresta a ser removida
 */
void aresta_vetor_apaga(no_grafo *node, int aresta_index) {
    for (int pos = aresta_index; pos < node->tamanho - 1; pos++)
        aresta_grafo_swap(&node->arestas[pos], &node->arestas[pos + 1]);

    aresta_apaga(node->arestas[node->tamanho - 1]);
    node->tamanho--;

    aresta_grafo **new_vec = (aresta_grafo **)realloc(node->arestas, (node->tamanho) * sizeof(node->arestas[0]));

    if (node->tamanho)
        if (check_ptr(new_vec, REALLOC_ERROR_MSG, "grafo.c - aresta_vetor_apaga() - node->arestas realloc"))
            return;

    node->arestas = new_vec;
}

no_grafo *no_remove(grafo *g, char *cidade) {
    if (!g || !cidade)
        return NULL;

    int pos_para_remover = encontra_no_idx(g, cidade);
    if (pos_para_remover == -1)
        return NULL;

    if (pos_para_remover != (g->tamanho - 1))
        no_grafo_swap(&g->nos[pos_para_remover], &g->nos[g->tamanho - 1]);

    g->tamanho--;
    no_grafo *no_para_remover = g->nos[g->tamanho];
    g->nos[g->tamanho] = NULL;
    no_grafo **novo_vetor_nos = (no_grafo **)realloc(g->nos, g->tamanho * sizeof(g->nos[0])); //!analisar

    if (g->tamanho)
        if (!check_ptr(novo_vetor_nos, REALLOC_ERROR_MSG, "grafo.c - no_remove() - g->nos realloc"))
            g->nos = novo_vetor_nos;
    no_grafo *no_para_remover = g->nos[pos_para_remover];

    for (int node = 0; node < g->tamanho; node++)
        for (int aresta = 0; aresta < g->nos[node]->tamanho; aresta++)
            if (g->nos[node]->arestas[aresta]->destino == no_para_remover) {
                aresta_vetor_apaga(g->nos[node], aresta);
                aresta--;
            }

    if (pos_para_remover != (g->tamanho - 1))
        for (int pos = pos_para_remover; pos < g->tamanho - 1; pos++)
            g->nos[pos] = g->nos[pos + 1];

    no_grafo **novo_vetor_nos = (no_grafo **)realloc(g->nos, (g->tamanho - 1) * sizeof(g->nos[0]));
    g->tamanho--;

    if (g->tamanho) {
        if (!check_ptr(novo_vetor_nos, REALLOC_ERROR_MSG, "grafo.c - no_remove() - g->nos realloc")) {
            g->nos = novo_vetor_nos;
        }
    }

    return no_para_remover;
}

int aresta_apaga(aresta_grafo *aresta) {
    if (!aresta)
        return -1;

    free(aresta->codigo);
    free(aresta->companhia);
    free(aresta);

    return 0;
}

int no_apaga(no_grafo *no) {
    if (!no)
        return -1;

    free(no->cidade);

    for (int i = 0; i < no->tamanho; i++)
        aresta_apaga(no->arestas[i]);
    free(no->arestas);

    free(no);
    return 0;
}

void grafo_apaga(grafo *g) {
    if (!g)
        return;

    for (int i = 0; i < g->tamanho; i++)
        no_apaga(g->nos[i]);

    free(g);
}

no_grafo *encontra_voo(grafo *g, char *codigo, int *aresta_pos) {
    if (!g || !codigo || !aresta_pos)
        return NULL;

    for (int node = 0; node < g->tamanho; node++)
        for (int aresta = 0; aresta < g->nos[node]->tamanho; aresta++)
            if (!strcmp(g->nos[node]->arestas[aresta]->codigo, codigo)) {
                *aresta_pos = aresta;
                return g->nos[node];
            }

    return NULL;
}

no_grafo **pesquisa_avancada(grafo *g, char *destino, data chegada, double preco_max, int *n) {
    if (!g || !destino || preco_max <= 0 || !n)
        return NULL;

    //tamanho inicial do vetor de retorno
    int encontrados_size = 50;
    int n_econtrados = 0;
    no_grafo **voos_encontrados = calloc(encontrados_size, sizeof(*voos_encontrados));

    for (int node = 0; node < g->tamanho; node++)
        for (int aresta = 0; aresta < g->nos[node]->tamanho; aresta++)
            if (!strcmp(g->nos[node]->arestas[aresta]->destino->cidade, destino) &&
                g->nos[node]->arestas[aresta]->chegada.tm_mday == chegada.tm_mday &&
                g->nos[node]->arestas[aresta]->preco <= preco_max) {
                if (n_econtrados == encontrados_size) {
                    voos_encontrados = realloc(voos_encontrados, (encontrados_size * 2) * sizeof(*voos_encontrados));
                    encontrados_size *= 2;
                }

                voos_encontrados[n_econtrados] = g->nos[node];
                n_econtrados++;
            }

    voos_encontrados = realloc(voos_encontrados, n_econtrados * sizeof(*voos_encontrados));
    *n = n_econtrados;

    return voos_encontrados;
}

no_grafo **trajeto_mais_rapido(grafo *g, char *origem, char *destino, data partida, int *n) {
    
    if(!g || !origem || !destino || !n) return NULL; // !partida ? 
    heap * 

        for(int vertice = 0 ;  vertice < g->tamanho ;vertice++) {
            
        }
    return NULL;
}

// function dijkstra(G, s):
// G grafo, s inicio/nó

 // Input: A graph G with vertices V, and a start vertex s
 // Output: Nothing
//Purpose: Decorate nodes with shortest distance from s
//  for v in V:
//  v.dist = infinity // Initialize distance decorations
//  v.prev = null // Initialize previous pointers to null
//  s.dist = 0 // Set distance to start to 0
//  PQ = PriorityQueue(V) // Use v.dist as priorities
//  while PQ not empty:
//  u = PQ.removeMin()
//  for all edges (u, v):
//  if v.dist > u.dist + cost(u, v): // cost() is weight
//  v.dist = u.dist + cost(u,v) // Replace as necessary
//  v.prev = u // Maintain pointers for path
//  PQ.replaceKey(v, v.dist)


// dos slides
/* DIJKSTRA COMPLEXIDADE – HEAP
BINÁRIO
PROGRAMAÇÃO 2 | MIEEC | 2020/21
function dijkstra(G, s): */
 // Input: A graph G with vertices V, and a start vertex s
 // Output: Nothing
 // Purpose: Decorate nodes with shortest distance from s
/*  for v in V: // O(V)
 v.dist = infinity
 v.prev = null
 s.dist = 0
 PQ = PriorityQueue(V)
 while PQ not empty: // O(V)
 u = PQ.removeMin() // O(log(V))
 for all edges (u, v): // O(E)
 if v.dist > u.dist + cost(u, v):
 v.dist = u.dist + cost(u,v)
 v.prev = u
 PQ.replaceKey(v, v.dist) // O(log(V))
 */

no_grafo **menos_transbordos(grafo *g, char *origem, char *destino, data partida, int *n) {
    return NULL;
}

aresta_grafo **atualiza_lugares(char *ficheiro, grafo *g, int *n) {
    return NULL;
}

grafo *grafo_importa(const char *nome_ficheiro) {
    if (nome_ficheiro == NULL)
        return NULL;

    FILE *f = fopen(nome_ficheiro, "r");
    if (f == NULL)
        return NULL;

    grafo *g = grafo_novo();
    if (g == NULL) {
        fclose(f);
        return NULL;
    }
    char str[500] = {0}, codigo[15] = {0}, origem[50] = {0}, destino[50] = {0}, companhia[4] = {0};
    int lugares = 0;
    double preco = 0;
    struct tm partida, chegada;

    char *token;
    no_grafo *no_origem, *no_destino;
    int ret;
    while (fgets(str, 500 * sizeof(char), f) != NULL) {
        str[strlen(str) - 1] = '\0';

        token = strtok(str, ",");
        if (!token)
            break;
        strcpy(codigo, token);

        token = strtok(NULL, ",");
        strcpy(origem, token);

        token = strtok(NULL, ",");
        strcpy(destino, token);

        token = strtok(NULL, ",");
        partida.tm_year = atoi(token);

        token = strtok(NULL, ",");
        partida.tm_mon = atoi(token) - 1;

        token = strtok(NULL, ",");
        partida.tm_mday = atoi(token);

        token = strtok(NULL, ",");
        partida.tm_hour = atoi(token);

        token = strtok(NULL, ",");
        partida.tm_min = atoi(token);

        token = strtok(NULL, ",");
        chegada.tm_year = atoi(token);

        token = strtok(NULL, ",");
        chegada.tm_mon = atoi(token) - 1;

        token = strtok(NULL, ",");
        chegada.tm_mday = atoi(token);

        token = strtok(NULL, ",");
        chegada.tm_hour = atoi(token);

        token = strtok(NULL, ",");
        chegada.tm_min = atoi(token);

        token = strtok(NULL, ",");
        preco = atof(token);

        token = strtok(NULL, ",");
        lugares = atoi(token);

        token = strtok(NULL, ",");
        strcpy(companhia, token);

        // procura no grafo um nó com o nome da origem
        no_origem = encontra_no(g, origem);
        // se o nó ainda não existe, cria-o e insere-o no grafo
        if (!no_origem) {
            no_origem = no_insere(g, origem);

            if (!no_origem) {
                fclose(f);
                return NULL;
            }
        }
        // procura no grafo um nó com o nome do destino
        no_destino = encontra_no(g, destino);

        // se o nó ainda não existe, cria-o e insere-o no grafo
        if (!no_destino) {
            no_destino = no_insere(g, destino);

            if (!no_destino) {
                fclose(f);
                return NULL;
            }
        }

        if (existe_aresta(no_origem, no_destino, codigo) == 0) {
            ret = cria_aresta(no_origem, no_destino, codigo, companhia, partida, chegada, preco, lugares);

            if (ret == -1) {
                fclose(f);
                return NULL;
            }
        }
    }

    fclose(f);
    return g;
}
