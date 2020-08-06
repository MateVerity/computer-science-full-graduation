/*
1)Especificar um TAD arvore que contenha as seguintes fun��es:

�Fun��o para inser��o de nodos em uma ABP que armazene inteiros
�Fun��o para imprimir os nodos usando o caminhamento pr�-fixado � esquerda
�Fun��o que recebe um ponteiro para um nodo e calcula a sua altura.

2) Criar um programa principal para testar as fun��es do TAD:

3) Submeter o trabalho pelo Moodle e mostrar � professora at� o final da aula. 

*/

#include <stdio.h>
#include <stdlib.h>
#include "TAD_arvore.h"

int main ()
{
    //ver altura de alguns nodos inseridos na �rvore
    
    pNodoA *raiz;
    int altura;
    
    //Inicializa��o da ABP
    raiz = InicializaABP(raiz); //raiz = NULL
    
    //Inser��o de Dados na ABP
    raiz = InsereNodoABP(raiz, 6);
    raiz = InsereNodoABP(raiz, 4);
    raiz = InsereNodoABP(raiz, 3);
    raiz = InsereNodoABP(raiz, 1);    
    raiz = InsereNodoABP(raiz, 8);    
    raiz = InsereNodoABP(raiz, 7);
            
    //Imprime a ABP  
    printf ("----- Imprimindo a ABP -----\n\n");      
    ImprimeABP(raiz);
    printf ("----- Impressao concluida -----\n");
    
    //Determina a altura de um nodo
    altura = AlturaNodoABP(ConsultaABP(raiz,1)); //deve dar 1
    printf ("Altura do nodo de valor 1: %d\n", altura);    
    altura = AlturaNodoABP(ConsultaABP(raiz,4)); //deve dar 3
    printf ("Altura do nodo de valor 4: %d\n", altura);    
    altura = AlturaNodoABP(ConsultaABP(raiz,7)); //deve dar 1
    printf ("Altura do nodo de valor 7: %d\n", altura);    
    altura = AlturaNodoABP(ConsultaABP(raiz,6)); //deve dar 4
    printf ("Altura do nodo de valor 6: %d\n", altura);   
  
    system ("pause");
    return 0;
}
