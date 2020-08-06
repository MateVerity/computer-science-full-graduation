#include <stdio.h>
#include <stdlib.h>
#include "Aula09_Pilhas_TAD.h"

/* Fun��es Exportadas */

/* Cria pilha vazia - Fun��o de inicializa��o da pilha */
TipoPilha* InicializaPilha (TipoPilha* Topo, int* pcont)
{
           *pcont = 0;
           return NULL;
}

/* Insere nodo no topo da pilha */
TipoPilha* InsereNodo (TipoPilha* Topo, TipoInfo Dado, int* pcont)
{
           *pcont = *pcont + 1;          
           TipoPilha *novo; //novo elemento
           TipoPilha *ptaux = Topo; //ptaux aponta para o mesmo endere�o de "Topo"
           
           /*aloca um novo nodo */
           novo = (TipoPilha*) malloc(sizeof(TipoPilha)); //a parte "(TipoPilha*)" � usada em para programas em .c,
                                                          //para .cpp n�o � necess�rio
           novo->elo = NULL; //garante que o elemento criado n�o aponte para nada 
           
           /*insere a informa��o no novo nodo*/
           novo->dado = Dado; //o campo "dado" recebe os valores de Dado, previamente preenchido fora da fun��o (mudar isso)

           /*encaeia o elemento*/
           novo->elo = Topo;
           Topo = novo; //Topo aponta para o elemento criado 
           return Topo;
}

/* Remove nodo do topo da lista */
/* Retorna 0 se a pilha estiver vazia, ou 1 se a remo��o der certo */
/*int RemoveNodo (TipoPilha **Topo, TipoInfo *Dado)
{
           TipoPilha *ptaux;
           if (Topo == NULL)
              return 0; //pilha vazia
           else
           {
               *Dado = (*Topo)->dado; // devolve o valor do topo
               ptaux = *Topo; //guarda o endere�o do topo
               *Topo = (*Topo)->elo; //o pr�ximo passa a ser o topo
               free (ptaux); //libera o que estava no topo
               ptaux = NULL;
               return 1;    
           }
}*/

int RemoveNodo (TipoPilha* Topo, int* pcont)
{
           *pcont = *pcont - 1;    
           TipoPilha* ptaux;
           if (Topo == NULL)
              return 0; //pilha vazia
           else
           {
 //              *Dado = (*Topo)->dado; // devolve o valor do topo
               ptaux = Topo; //guarda o endere�o do topo
               Topo = Topo->elo; //o pr�ximo passa a ser o topo
               free (ptaux); //libera o que estava no topo
               //printf ("ptaux = %d, conteudo ptaux = %f, Topo = %d, Nodo topo = %f", ptaux, ptaux->dado.preco, Topo, Topo->dado.preco);
               ptaux = NULL;
               return 1;    
           }
}


/* Consulta nodo do topo da lista */
float ConsultaNodo (TipoPilha* Topo)
{
           if (Topo == NULL)
              return 0;
           else
              return Topo->dado.preco;
}

/* Destr�i pilha */
TipoPilha* DestroiPilha (TipoPilha* Topo, int* pcont)
{
            *pcont = 0;
            TipoPilha *ptaux;
            while (Topo != NULL)
            {
                  ptaux = Topo; //guarda o endere�o do topo
                  Topo = Topo->elo; //o pr�ximo passa a ser o topo 
                  free (ptaux); //libera o que estava no topo   
            }  
            return NULL;
}

void ImprimePilha (TipoPilha* Topo, int cont)
{
     TipoPilha *ptaux1;
     TipoPilha *ptaux2;
     
     ptaux1 = Topo;
     printf ("topo = %d, cont topo = %f\n", Topo, Topo->dado.preco);
     ptaux1 = Topo->elo;
     printf ("elo = %d, cont elo = %f\n", ptaux1, ptaux1->dado.preco);
          ptaux1 = Topo;
     for (; ptaux1 != NULL; cont--)
     {
           ptaux2 = ptaux1; //guarda o endere�o do topo
           printf ("(%d) %f\n", cont, ptaux2->dado.preco);
           ptaux1 = ptaux1->elo; //o pr�ximo passa a ser o topo 
     }      
}
