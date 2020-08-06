/*

1)Especificar um TAD arvore que contenha as seguintes fun��es:

�Fun��o para inser��o de nodos em uma ABP que armazene inteiros
�Fun��o para imprimir os nodos usando o caminhamento pr�-fixado � esquerda
�Fun��o que recebe um ponteiro para um nodo e calcula a sua altura.

*/

//Estrutura de um nodo
struct TNodoA
{
       int info;
       struct TNodoA *esq; //valores menores que o valor da raiz
       struct TNodoA *dir; //valores maiores que o valor da raiz
};
typedef struct TNodoA pNodoA;

pNodoA* InicializaABP (pNodoA *raiz);
pNodoA* InsereNodoABP (pNodoA *raiz, int val);
void ImprimeABP (pNodoA *raiz);
int AlturaNodoABP (pNodoA *raiznodo);
pNodoA* ConsultaABP (pNodoA *raiz, int val);
int ContaNodosABP (pNodoA *raiz, int ch);
pNodoA* AchaPai (pNodoA *raiz, int ch);
pNodoA* ExcluiNodoABP (pNodoA *raiz, int ch);
