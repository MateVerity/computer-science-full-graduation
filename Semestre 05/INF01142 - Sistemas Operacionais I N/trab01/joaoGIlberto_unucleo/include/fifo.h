#include <stdlib.h>
#include <stdio.h>

//Fila de PCB
#include "pcb.h"
//Define PCB como elemento da fila
typedef PCB TipoInfo;
	
//TipoNo � o tipo de um elemento da fila (Lista simplesmente encadeada)
typedef struct s_TipoNo {
	TipoInfo info;
	struct s_TipoNo *prox;
} TipoNo;

//Fila: ponteiro para um n� (in�cio da fila)
typedef TipoNo* TipoFila;

/*------------------ROTINAS PARA MANIPULA��O DE FILAS------------------------*/
//NovaFila retorna uma fila vazia
TipoFila NovaFila();

//Insere no fim da fila
void InsereFila(TipoFila* Fila, TipoInfo dado);

//Retorna o elemento do fim da fila
TipoFila FimDaFila(TipoFila Fila);

//Retira o dado do in�cio da fila. OBS: retorna um PONTEIRO para o dado
TipoInfo* RetiraFila(TipoFila* Fila);

//Libera a mem�ria usada pela fila
void DestroiFila(TipoFila* Fila);
