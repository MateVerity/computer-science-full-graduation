#ifndef MINIMAX_H
#define MINIMAX_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "round.h"

// -----------------------------------------------------------------------------
// Defini��es
// -----------------------------------------------------------------------------

#define DRAW_GAME	20
#define FOUND		9999
#define ERROR		-9999

//MAX
#define MAX_WIN		10
#define MAX_PESO1	8
#define MAX_PESO2	6
#define MAX_PESO3	4
#define MAX_PESO4	2
#define MAX_LOSS 	0
#define MINIMUM		-999

//MIN
#define MIN_WIN		0
#define MIN_PESO1	2
#define MIN_PESO2	4
#define MIN_PESO3	6
#define MIN_PESO4	8
#define MIN_LOSS 	10
#define MAXIMUM		999
 
//--------------------------------------------------------------------------
// Fun��es
//-------------------------------------------------------------------------- 

/* Avalia uma jogada na rodada MAX */
int evalFunctionMax(Position **, int *, int);

/* Avalia uma jogada na rodada MIN */
int evalFunctionMin(Position **, int *, int);

/* Retorna o maior valor dos dois */
int max(int, int);

/* Retorna o menor valor dos dois */
int min(int, int);

/* Analisa se a jogada � de vit�ria, derrota, empate ou se o jogo prossegue.
Chamada ap�s uma pe�a ter sido posicionada no tabuleiro */
int evalFunction(Position **, int *, int);

/* Retorna o maior valor dos dois e atualiza as coordenadas da nova jogada */
int maxMinimax(int, int, int *, int *, int, int);

/* V� se h� ua jogada do tipo 2-vazio-1 */
int evalTwoEmptyOne(Position **, int *, int, int *, int, int *, int *);

//void firstMove(Round *);

//D� prosseguimento ao minimax, simulando as jogadas futuras em c�pias do tabuleiro.
//int expandTree(Round *, int, int, int, int, int);

//Gera a �rvore do Minimax
//void seedMinimax(Round *, int, int, int, int *, int *);

#endif

