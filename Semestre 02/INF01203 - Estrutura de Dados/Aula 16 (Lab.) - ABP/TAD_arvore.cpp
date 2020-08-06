#include <stdio.h>
#include <stdlib.h>
#include "TAD_arvore.h"

/* Fun��es Exportadas para TAD_arvore.h */

//Fun��o que inicializa a ABP, ou seja, o endere�o da raiz � NULL
pNodoA* InicializaABP(pNodoA *raiz)
{
        return NULL;
}

//Fun��o que Insere um nodo em uma ABP, valor inteiro
pNodoA* InsereNodoABP(pNodoA *raiz, int val)
{
    if (raiz == NULL)
    {
        raiz = (pNodoA*) malloc(sizeof(pNodoA));
        raiz->info = val;
        raiz->esq = NULL;
        raiz->dir = NULL;
    }
    else
        if (val < (raiz->info))
           raiz->esq = InsereNodoABP(raiz->esq, val);
        if (val > (raiz->info))
            raiz->dir = InsereNodoABP(raiz->dir, val);
    return raiz;
}

//Fun��o que imprime os nodos de uma ABP, usando caminhamento pr�-fixado
void ImprimeABP(pNodoA *raiz)
{
     if (raiz)
     {
        printf ("%d\n", raiz->info);
        ImprimeABP(raiz->esq);    
        ImprimeABP(raiz->dir);     
        printf ("\n");
     }
}

//Fun��o que determina a altura de um determinado nodo
int AlturaNodoABP(pNodoA *raiznodo)
{
    if (!raiznodo)
       return 0;
    else   
    {    
       if (AlturaNodoABP(raiznodo->esq) > AlturaNodoABP(raiznodo->dir))
          return AlturaNodoABP(raiznodo->esq) + 1;  
       else 
          return AlturaNodoABP(raiznodo->dir) + 1;      
    }
        
}

//Fun��o que consulta se um determinado nodo est� na ABP
pNodoA* ConsultaABP(pNodoA *raiz, int val) 
{
    while (raiz!=NULL)
    {
        if (raiz->info == val)
           return raiz; //achou retorna o ponteiro para o nodo
        else
            if (raiz->info > val)
               raiz = raiz->esq;
            if (raiz->info < val)
               raiz = raiz->dir;
    }
    return NULL; //n�o achou, retorna null
}

//Fun��o que conta o n�mero d nodos de uma ABP
int ContaNodosABP (pNodoA *raiz, int ch)
{
        if (raiz == NULL)
           return 0;
        else 
           return (1 + ContaNodosABP (raiz->esq, ch) + ContaNodosABP (raiz->esq, ch));
}

//Dado um valor, achar o pai do nodo que cont�m esse valor
pNodoA* AchaPai (pNodoA *raiz, int ch)
{
        if (raiz == NULL)
           return NULL;
        if (raiz->esq != NULL)
           if (raiz->esq->info == ch)
              return raiz;
        if (raiz->dir != NULL)
           if (raiz->dir->info == ch)
              return raiz;
        if (ch > raiz->info)
           return AchaPai (raiz->dir, ch);
        if (ch < raiz->info)
           return AchaPai (raiz->esq, ch);
        return NULL;
}

/*
Exclus�o - Tratar:
a)  Quando o n� a ser exclu�do n�o contenha filhos:  
O n� simplesmente � removido.  
b)  Quando o n� a ser exclu�do contenha somente um dos filhos:  
O pai do n� a ser exclu�do passa a apontar para este filho e o n� � removido. 
c)  Quando o n� a ser exclu�do contenha os dois filhos:  
Busca-se o maior elemento da sub-�rvore da esquerda (a partir da raiz da sub-�rvore esquerda caminha-se sempre 
para a direita at� achar um n� cujo filho � direita � nulo).  
Transfere-se a informa��o deste n� para o n� a ser removido e remove-se este novo n�, que cair� no caso (a) ou (b). 
Vamos deixar a cargo do aluno, como exerc�cio, a implementa��o desta rotina.  
*/

pNodoA* ExcluiNodoABP (pNodoA *raiz, int ch)
{
}







