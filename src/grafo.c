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

// * tipos de pesquisa para dijkstra
#define MAIS_RAPIDO 0
#define MENOS_TRANSBORDOS 1

// * mensagens de erro
#define REALLOC_ERROR_MSG "\n[ERRO] - Falha ao alocar memória. - realloc\n"
#define MALLOC_ERROR_MSG "\n[ERRO] - Falha ao alocar memória. - malloc/calloc\n"
#define FILE_ERROR_MSG "\n[ERRO] - Falha ao abrir ficheiro\n"

void heapify_up(heap *h, int index);
void heapify_down(heap *h, int index);
void heap_atualiza_prioridade(heap *h, no_grafo *no, double prioridade);

static int check_ptr(void *ptr, const char *msg, const char *origem) {
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

    no_grafo *no_para_remover = g->nos[g->tamanho - 1];

    for (int node = 0; node < g->tamanho; node++)
        for (int aresta = 0; aresta < g->nos[node]->tamanho; aresta++)
            if (g->nos[node]->arestas[aresta]->destino == no_para_remover) {
                aresta_vetor_apaga(g->nos[node], aresta);
                aresta--;
            }

    g->tamanho--;
    g->nos[g->tamanho] = NULL;

    no_grafo **novo_vetor_nos = (no_grafo **)realloc(g->nos, g->tamanho * sizeof(g->nos[0]));
    if (!check_ptr(novo_vetor_nos, REALLOC_ERROR_MSG, "grafo.c - no_remove() - g->nos realloc"))
        g->nos = novo_vetor_nos;

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
    if (g->nos) {
        for (int i = 0; i < g->tamanho; i++)
            no_apaga(g->nos[i]);
        free(g->nos);
    }
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

int compare_time(data a, data b) {
    return (a.tm_year - b.tm_year) * 100 + (a.tm_mon - b.tm_mon) * 10 + (a.tm_mday - b.tm_mday);
}

void vetor_voos_insere(no_grafo **nos_com_voo, int *capacidade, int *n_voos, no_grafo *node) {
    for (int i = 0; i < (*n_voos); i++)
        if (nos_com_voo[i] == node)
            return;

    if (n_voos == capacidade) {
        nos_com_voo = realloc(nos_com_voo, ((*capacidade) * 2) * sizeof(*n_voos));
        //TODO: check_ptr também?
        (*capacidade) *= 2;
    }

    nos_com_voo[*n_voos] = node;
    (*n_voos)++;
}

no_grafo **pesquisa_avancada(grafo *g, char *destino, data chegada, double preco_max, int *n) {
    if (!g || !destino || preco_max <= 0 || !n) return NULL;  //? chegada?

    //tamanho inicial do vetor de retorno
    int encontrados_size = 50;
    int n_econtrados = 0;
    no_grafo **voos_encontrados = calloc(encontrados_size, sizeof(*voos_encontrados));
    if (check_ptr(voos_encontrados, MALLOC_ERROR_MSG, "grafo.c - pesquisa_avancada() - voos_encontrados"))
        return NULL;

    aresta_grafo *aresta_atual = NULL;

    for (int node = 0; node < g->tamanho; node++)
        for (int aresta = 0; aresta < g->nos[node]->tamanho; aresta++) {
            aresta_atual = g->nos[node]->arestas[aresta];

            if (!strcmp(aresta_atual->destino->cidade, destino) &&
                !compare_time(aresta_atual->chegada, chegada) &&
                aresta_atual->preco <= preco_max)

                vetor_voos_insere(voos_encontrados, &encontrados_size, &n_econtrados, g->nos[node]);
        }

    voos_encontrados = realloc(voos_encontrados, n_econtrados * sizeof(*voos_encontrados));
    if (n_econtrados)
        if (check_ptr(voos_encontrados, REALLOC_ERROR_MSG, "grafo.c - pesquisa_avancada() - voos_encontrados"))
            return NULL;
    *n = n_econtrados;

    return voos_encontrados;
}

void alogoritmo_dijkstra(grafo *g, no_grafo *origem, no_grafo *destino, data partida, const int TIPO_PESQUISA) {
    heap *h = heap_nova(g->tamanho);

    const double INFINITY = __DBL_MAX__;

    origem->dataatualizada = &partida;
    origem->p_acumulado = 0;
    origem->anterior = NULL;

    heap_insere(h, origem, origem->p_acumulado);

    for (int i = 0; i < g->tamanho; i++) {
        if (g->nos[i] == origem)
            continue;

        g->nos[i]->p_acumulado = INFINITY;
        g->nos[i]->dataatualizada = NULL;
        g->nos[i]->anterior = NULL;

        heap_insere(h, g->nos[i], g->nos[i]->p_acumulado);
    }

    no_grafo *no_atual = NULL;
    aresta_grafo *aresta_atual = NULL;
    int i = 0;
    while (h->tamanho) {
        no_atual = heap_remove(h);  //remove nó de menor prioridade
        i++;
        for (int aresta = 0; aresta < no_atual->tamanho; aresta++) {
            aresta_atual = no_atual->arestas[aresta];

            if (!no_atual->dataatualizada && !no_atual->anterior)
                break;

            if (compare_time(aresta_atual->partida, *no_atual->dataatualizada) >= 0 &&
                (!aresta_atual->destino->dataatualizada ||
                 compare_time(*aresta_atual->destino->dataatualizada, aresta_atual->chegada))) {
                aresta_atual->destino->anterior = no_atual;
                aresta_atual->destino->dataatualizada = &aresta_atual->chegada;

                if (TIPO_PESQUISA == MENOS_TRANSBORDOS)
                    aresta_atual->destino->p_acumulado += 1;
                else
                    aresta_atual->destino->p_acumulado = (double)mktime(aresta_atual->destino->dataatualizada);

                heap_atualiza_prioridade(h, aresta_atual->destino, aresta_atual->destino->p_acumulado);
            }
        }
        if (no_atual == destino)
            break;
    }

    heap_apaga(h);
}

no_grafo **dijkstra(grafo *g, char *origem, char *destino, data partida, int *n, const int TIPO_PESQUISA) {
    static no_grafo *origem_last = NULL;
    static int pesquisa_last = -1;

    no_grafo *no_origem = encontra_no(g, origem);
    no_grafo *no_destino = encontra_no(g, destino);

    if (pesquisa_last != TIPO_PESQUISA) {
        pesquisa_last = TIPO_PESQUISA;
        origem_last = no_origem;
        alogoritmo_dijkstra(g, no_origem, no_destino, partida, TIPO_PESQUISA);
    }

    if (origem_last && strcmp(origem_last->cidade, origem)) {
        origem_last = no_origem;
        alogoritmo_dijkstra(g, no_origem, no_destino, partida, TIPO_PESQUISA);
    }

    if (!no_destino->anterior)
        alogoritmo_dijkstra(g, no_origem, no_destino, partida, TIPO_PESQUISA);

    int caminho_size = 1;

    no_grafo *no_atual = no_destino;

    while (no_atual != no_origem) {
        no_atual = no_atual->anterior;
        caminho_size++;
        if (!no_atual)
            break;
    }

    no_grafo **caminho = (no_grafo **)malloc(caminho_size * sizeof(*caminho));
    if (check_ptr(caminho, MALLOC_ERROR_MSG, "grafo.c - trajeto_mais_rapido - caminho")) return NULL;

    no_grafo *auxiliar = no_destino;
    for (int i = caminho_size - 1; i >= 0; i--) {
        caminho[i] = auxiliar;
        auxiliar = auxiliar->anterior;
    }

    *n = caminho_size;
    return caminho;
}

no_grafo **trajeto_mais_rapido(grafo *g, char *origem, char *destino, data partida, int *n) {
    if (!g || !origem || !destino || !n) return NULL;
    return dijkstra(g, origem, destino, partida, n, MAIS_RAPIDO);
}

no_grafo **menos_transbordos(grafo *g, char *origem, char *destino, data partida, int *n) {
    if (!g || !origem || !destino || !n) return NULL;
    return dijkstra(g, origem, destino, partida, n, MENOS_TRANSBORDOS);
}

aresta_grafo **atualiza_lugares(char *ficheiro, grafo *g, int *n) {
    if (!g || !ficheiro || !n) return NULL;

    FILE *f_in = fopen(ficheiro, "r");
    if (check_ptr(f_in, FILE_ERROR_MSG, "grafo.c - atualiza_lugares() - fopen"))
        return NULL;

    char codigo[15] = {0};
    int lugares, aresta_pos;

    int vec_size = 10, n_removidas = 0;
    aresta_grafo **arestas_removidas = (aresta_grafo **)calloc(vec_size, sizeof(*arestas_removidas));
    if (check_ptr(arestas_removidas, MALLOC_ERROR_MSG, "grafo.c - atualiza_lugares() - arestas_removidas"))
        return NULL;

    no_grafo *no_atual = NULL;
    while (fscanf(f_in, "%15[^,]%*c %d ", codigo, &lugares) == 2) {
        no_atual = encontra_voo(g, codigo, &aresta_pos);
        if (!no_atual)
            continue;

        if (lugares) {
            no_atual->arestas[aresta_pos]->lugares = lugares;
            continue;
        }

        if (n_removidas >= vec_size) {
            vec_size *= 2;
            arestas_removidas = (aresta_grafo **)realloc(arestas_removidas, vec_size * sizeof(*arestas_removidas));
        }

        arestas_removidas[n_removidas] = no_atual->arestas[aresta_pos];
        n_removidas++;

        no_atual->arestas[aresta_pos] = NULL;
        for (int i = aresta_pos; i < no_atual->tamanho - 1; i++)
            aresta_grafo_swap(&no_atual->arestas[i], &no_atual->arestas[i + 1]);

        no_atual->tamanho--;
        no_atual->arestas = (aresta_grafo **)realloc(no_atual->arestas, no_atual->tamanho * sizeof(*no_atual->arestas));
    }

    fclose(f_in);
    *n = n_removidas;
    return arestas_removidas;
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

// ************************************************************************************ //
// ************************** FUNÇÕES AUXILIARES PARA A HEAP ************************** //
// ************************************************************************************ //
#define RAIZ (1)
#define PAI(x) (x / 2)
#define FILHO_ESQ(x) (x * 2)

static int menor_que(elemento *e1, elemento *e2) {
    if (e1 == NULL || e2 == NULL)
        return 0;

    return e1->prioridade < e2->prioridade;
}

void heapify_up(heap *h, int index) {
    elemento *x = h->elementos[index];

    while (index > 1 && menor_que(x, h->elementos[PAI(index)])) {
        h->elementos[index] = h->elementos[PAI(index)];
        index = PAI(index);
    }

    h->elementos[index] = x;
}

void heapify_down(heap *h, int index) {
    elemento *x = h->elementos[index];

    while (TRUE) {
        int k = FILHO_ESQ(index);
        if (k >= h->capacidade)
            break;

        if (k + 1 < h->capacidade && menor_que(h->elementos[k + 1], h->elementos[k]))
            ++k;  // filho direito

        if (!menor_que(h->elementos[k], x))
            break;

        h->elementos[index] = h->elementos[k];
        index = k;
    }

    h->elementos[index] = x;
}

void heap_atualiza_prioridade(heap *h, no_grafo *no, double prioridade) {
    int index = -1;

    for (int i = 0; i < h->capacidade; i++)
        if (h->elementos[i] && h->elementos[i]->no == no) {
            index = i;
            break;
        }

    if (index == -1)
        return;

    h->elementos[index]->prioridade = prioridade;

    heapify_up(h, index);
    heapify_down(h, index);
}

#undef RAIZ
#undef PAI
#undef FILHO_ESQ
// ************************************************************************************ //
// ************************** FUNÇÕES AUXILIARES PARA A HEAP ************************** //
// ************************************************************************************ //

#undef TRUE
#undef FALSE

#undef REALLOC_ERROR_MSG
#undef MALLOC_ERROR_MSG
#undef FILE_ERROR_MSG

#undef MAIS_RAPIDO
#undef MENOS_TRANSBORDOS
