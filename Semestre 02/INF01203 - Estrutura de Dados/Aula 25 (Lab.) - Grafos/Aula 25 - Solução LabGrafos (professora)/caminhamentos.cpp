#include <stdio.h>
#include <stdlib.h>
#include "caminhamentos.h"

void visita (int vis[max+1], int v) //v: v�rtice visitado
{
     printf ("%d\n", v);
     vis[v] = true;
}

//Depth First Search - Cainhamento por Profundidade
void DFS (int grafo[max+1][max+1], int v, int vis[max+1]) //v: v�rtice visitado
{
    int w;
    visita (vis, v); //insere no vetor de v�rtices visitados o v�rtice visitado de valor "v"
    for (w = 1; w <= max ; w++) //percorre os v�rtices adjacentes a v
      if ((grafo[v][w] == 1) && (vis[w] == false))  //encontra um v�rtice adjacente a "v" que ainda nao tenha sido visitado
         DFS (grafo, w, vis); //chamada recursiva para o v�rtice adjacente a v
}

void BFS (int grafo[max+1][max+1], int v, int vis[max+1]) //v: v�rtice visitado
{
    int w;
    TipoFila *FV;
    visita (vis, v); 
    
    FV = cria_fila();
    FV = insere(FV, v); //insere v�rtice "v" na lista � qual FV aponta
    while (!vazia(FV)) //retorna 0: FV == NULL, retorna 1: FV != NULL
    {
       v = remove (&FV); //remove o primeiro elemento da lista � qual FV aponta e retorna o valor nodo removido
       for (w = 1; w <= max; w++) //percorre os v�rtices adjacentes a v
          if ((grafo[v][w] == 1) && (vis[w] == false))  //encontra um v�rtice adjacente a v que ainda nao tenha sido visitado
          {
              FV = insere(FV,w);
              visita (vis,w);
           }
   
    }
}
