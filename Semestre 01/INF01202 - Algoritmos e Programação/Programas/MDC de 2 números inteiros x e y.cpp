/*
  Name: Jo�o Luiz Grave Gross
  Exerc�cio: Fixa��o 7.1
  Date: 21/03/09 16:56
  Description: Fazer um programa em C que leia um inteiro N, maior que -1, 
  e determine o m�ximo divisor comum de 2 n�meros inteiros x e y. 

    Como fazer:
    1�) dividimos o n�mero maior pelo n�mero menor;
            48 / 30 = 1 (com resto 18)

    2�) dividimos o divisor 30, que � divisor da divis�o anterior, por 18, que � 
    o resto da divis�o anterior, e assim sucessivamente;
            30 / 18 = 1 (com resto 12)

            18 / 12 = 1 (com resto 6)

            12 / 6 = 2 (com resto zero - divis�o exata)

    3�) O divisor da divis�o exata � 6. Ent�o m.d.c.(48,30) = 6.

   Objetivo: Determinar o m�ximo divisor comum de 2 n�meros inteiros x e y
*/

#include<stdlib.h>
#include<stdio.h>

int main ()
{
    int x, y, num, aux1, aux2, flag=1;
    printf ("Digite dois valores: ");
    scanf ("%i%i",&x,&y);
    if (x > y)
    {                                   
       aux1 = x % y;                     //calcula o resto da divis�o              
       num = y;                          //y ser� o numerador da primeira divis�o do while
    }
    if (y > x)
    {
       aux1 = y % x;                     //calcula o resto da divis�o
       num = x;                          //x ser� o numerador da primeira divis�o do while
    }
    if (x == y)
    {  
       num = x;
       flag = 0;
    }
    if (aux1 == 0)
       flag = 0;
    while (flag)
    {
       aux2 = num % aux1;                //aux2 = 30 % 18 = 12
       num = aux1;                       //num = 18
       aux1 = aux2;                      //aux1 = 12 -> pr�xima divis�o -> aux2 = 18 % 12
       if (aux2 == 0)
          flag = 0;                 
    }
    printf ("Maximo divisor comum: %d\n\n",num);
    system ("pause");
    return 0;
}
