#include "../inc/minimax.h"
 
/* 
Descri��o: 
	retorna o maior valor associado a esta jogada
Par�metros:
	@board: tabuleiro do jogo
	@piecesWhite: vetor com ids, os quais representam as posi��es que possuem pe�as brancas
Retorno:
	valor da fun��o de avalia��o								
Observa��es:
	posi��es com vizinhos na mesma linha, ou seja, que tenham os vizinhos 5 e 0, ou 2 e 3, ou
	1 e 4, s�o posi��es que n�o s�o avaliadas. Dessa forma a avalia��o tende a come�ar das pe�as 
	que est�o nos extremos das linhas, al�m de evitar avalia��es duplicadas de linhas no tabuleiro.
*/
int evalFunctionMax(Position **board, int *pieces, int pieceColor) {
	int i, j, id;
	int enemyColor;
	int val; //a cada n�vel de encadeamento armazena o estado de cor da posi��o
	int evalValue = MINIMUM; 
	Position auxPosition1, auxPosition2;

	if(pieceColor == WHITE)
		enemyColor = BLACK;
	else if(pieceColor == BLACK)
		enemyColor = WHITE;	
	else {
		////printf("\nInvalid color!\n");
		return INVALID;
	}

	for(i = 0; i < MAX_PIECES_PER_PLAYER; i++) {
		
		id = pieces[i];
		//printf("\n1-id: %d. ", id);
		if(id == INVALID)
			break;
		auxPosition1 = board[(int)id/10][id%10]; //primeira posi��o

		//Testa se a posi��o (piecesWhite[i]) possui mais de um vizinho em linha
		if( (auxPosition1.neighbors[0] != INVALID && auxPosition1.neighbors[0] == pieceColor &&
			 auxPosition1.neighbors[5] != INVALID && auxPosition1.neighbors[5] == pieceColor) ||
			(auxPosition1.neighbors[1] != INVALID && auxPosition1.neighbors[1] == pieceColor &&
			 auxPosition1.neighbors[4] != INVALID && auxPosition1.neighbors[4] == pieceColor) ||
		 	(auxPosition1.neighbors[2] != INVALID && auxPosition1.neighbors[2] == pieceColor &&
			 auxPosition1.neighbors[3] != INVALID && auxPosition1.neighbors[3] == pieceColor) ) {
			continue;	
		}
		
		for(j = 0; j < NEIGHBORS; j++) {
			//Passo 1: Para cada posi��o testa se esta posi��o possui algum vizinho tamb�m branco (1-?)
			
			id = auxPosition1.neighbors[j];	
			//printf("\n\t2-id: %d. ", id);
			if(id == INVALID)
				continue;
			auxPosition2 = board[(int)id/10][id%10];	//segunda posi��o
			val = auxPosition2.color;
			
			if(val == pieceColor) {
				//Passo 2: primeiro vizinho � branco (2-?)
				id = auxPosition2.neighbors[j];	
				//printf("3-id: %d. ", id);
				if(id == INVALID)
					continue;
				auxPosition2 = board[(int)id/10][id%10];	//terceira posi��o
				val = auxPosition2.color;		
				
				if(val == pieceColor) { 
					//Passo 5: vizinho � branco (3-?)
					id = auxPosition2.neighbors[j];	
					//printf("4-id: %d.", id);
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
					val = auxPosition2.color;
					
					if(val == pieceColor) { 
						//Passo 8: vizinho � branco (4) => VIT�RIA	
						//printf("evalFunctionMax: Passo 8. MAX_WIN.\n");
						evalValue = min(evalValue, MAX_WIN);
						break; //finaliza o for					
					} else {
						//Passo 9: vizinho � vazio (3-vazio), preto (3-!) ou inv�lido (3) => DERROTA
						//printf("evalFunctionMax: Passo 9. MAX_LOSS.\n");
						evalValue = min(evalValue, MAX_LOSS);
					}
					
				} else if(val == NO_COLOR) {
					//Passo 6: vizinho � vazio (2-vazio-?)
					id = auxPosition2.neighbors[j];	
					//printf("4-id: %d.", id);
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
					val = auxPosition2.color;
					
					if(val == pieceColor) { 
						//Passo 10: vizinho � branco (2-vazio-1) => MAX_PESO 1
						//printf("evalFunctionMax: Passo 10. MAX_PESO1.\n");						
						evalValue = min(evalValue, MAX_PESO1);
					} else if(val == NO_COLOR) {
						//Passo 11: vizinho � vazio (2-vazio-vazio) => MAX_PESO 2
						//printf("evalFunctionMax: Passo 11. MAX_PESO2.\n");
						evalValue = min(evalValue, MAX_PESO2);
					} else if(val == enemyColor) {
						//Passo 12: vizinho � preto (2-vazio-!) => MAX_PESO 3
						//printf("evalFunctionMax: Passo 12. MAX_PESO3.\n");
						evalValue = min(evalValue, MAX_PESO3);
					} else {
						//terceiro vizinho � inv�lido (2-vazio). Fim do tabuleiro. => MAX_PESO 3
						//printf("evalFunctionMax: Passo 12-b. MAX_PESO3.\n");
						evalValue = min(evalValue, MAX_PESO3);
					}
					
				} else if(val == enemyColor) {
					//Passo 7: vizinho � preto (2-!) => DERROTA
					//printf("evalFunctionMax: Passo 7. MAX_LOSS.\n");	
					evalValue = min(evalValue, MAX_LOSS);
				} else {
					//segundo vizinho � inv�lido (2). Fim do tabuleiro. => DERROTA
					//printf("evalFunctionMax: Passo 7-b. MAX_LOSS.\n");	
					evalValue = min(evalValue, MAX_LOSS);
				}
				
			} else if(val == NO_COLOR) {
				//Passo 3: primeiro vizinho � vazio (1-vazio-?)
				id = auxPosition2.neighbors[j];	
				//printf("3-id: %d. ", id);
				if(id == INVALID)
					continue;
				auxPosition2 = board[(int)id/10][id%10];	//terceira posi��o
				val = auxPosition2.color;
				
				if(val == pieceColor) { 
					//Passo 13: vizinho � branco (1-vazio-1-?)
					id = auxPosition2.neighbors[j];	
					//printf("4-id: %d.", id);
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
					val = auxPosition2.color;
					
					if(val == pieceColor) { 
						//Passo 16: vizinho � branco (1-vazio-2) => MAX_PESO 1		
						//printf("evalFunctionMax: Passo 16. MAX_PESO1.\n");	
						evalValue = min(evalValue, MAX_PESO1);
					} else if(val == NO_COLOR) {
						//Passo 17: vizinho � vazio (1-vazio-1-vazio) => MAX_PESO 2
						//printf("evalFunctionMax: Passo 17. MAX_PESO2.\n");	
						evalValue = min(evalValue, MAX_PESO2);
					} else if(val == enemyColor) {
						//Passo 18: vizinho � preto (1-vazio-1-!) => MAX_PESO 3	
						//printf("evalFunctionMax: Passo 18. MAX_PESO3.\n");	
						evalValue = min(evalValue, MAX_PESO3);
					} else {
						//terceiro vizinho � inv�lido (1-vazio-1). Fim do tabuleiro. => MAX_PESO 3	
						//printf("evalFunctionMax: Passo 18-b. MAX_PESO3.\n");	
						evalValue = min(evalValue, MAX_PESO3);
					}
				
				} else if(val == NO_COLOR) {
					//Passo 14: vizinho � vazio (1-vazio-vazio-?)
					id = auxPosition2.neighbors[j];	
					//printf("4-id: %d.", id);
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
					val = auxPosition2.color;
					
					if(val == pieceColor) { 
						//Passo 19: vizinho � branco (1-vazio-vazio-1) => MAX_PESO 2
						//printf("evalFunctionMax: Passo 19. MAX_PESO4.\n");	
						evalValue = min(evalValue, MAX_PESO4);
					} else if(val == NO_COLOR) {
						//Passo 20: vizinho � vazio (1-vazio-vazio-vazio) => MAX_PESO 4
						//printf("evalFunctionMax: Passo 20. MAX_PESO4.\n");	
						evalValue = min(evalValue, MAX_PESO4);				
					} else if(val == enemyColor) {
						//Passo 21: vizinho � preto (1-vazio-vazio-!) => MAX_PESO 4
						//printf("evalFunctionMax: Passo 21. MAX_PESO4.\n");	
						evalValue = min(evalValue, MAX_PESO4);					
					} else {
						//terceiro vizinho � inv�lido (1-vazio-vazio). Fim do tabuleiro. => MAX_PESO 4
						//printf("evalFunctionMax: Passo 21-b. MAX_PESO4.\n");
						evalValue = min(evalValue, MAX_PESO4);
					}
				
				} else if(val == enemyColor) {
					//Passo 15: vizinho � preto (1-vazio-!) => MAX_PESO 4
					//printf("evalFunctionMax: Passo 15-b. MAX_PESO4.\n");	
					evalValue = min(evalValue, MAX_PESO4);
				} else {
					//segundo vizinho � inv�lido (2). Fim do tabuleiro. => DERROTA
					//printf("evalFunctionMax: Passo 15-b. MIN_LOSS.\n");	
					evalValue = min(evalValue, MAX_LOSS);
				}
				
			} else if(val == enemyColor) {
				//Passo 4: primeiro vizinho � preto (1-!)
				//printf("evalFunctionMax: Passo 4. MIN_LOSS.\n");	
				evalValue = min(evalValue, MAX_LOSS);
			} else {
				//primeiro vizinho � inv�lido (1). Fim do tabuleiro.
				//printf("evalFunctionMax: Passo 4. MIN_LOSS.\n");	
				evalValue = min(evalValue, MAX_LOSS);
			}
				
		}
		
		if(evalValue == MAX_WIN)
			break; //finaliza o for
	}	
	
	return evalValue; 
}


/*
Descri��o: 
	retorna o menor valor associado a esta jogada
Par�metros:
	@board: tabuleiro do jogo
	@piecesWhite: vetor com ids, os quais representam as posi��es que possuem pe�as brancas
Retorno:
	valor da fun��o de avalia��o								
Observa��es:
	posi��es com vizinhos na mesma linha, ou seja, que tenham os vizinhos 5 e 0, ou 2 e 3, ou
	1 e 4, s�o posi��es que n�o s�o avaliadas. Dessa forma a avalia��o tende a come�ar das pe�as 
	que est�o nos extremos das linhas, al�m de evitar avalia��es duplicadas de linhas no tabuleiro.
*/
int evalFunctionMin(Position **board, int *pieces, int pieceColor) {
	int i, j, id;
	int IAcolor;
	int val; //a cada n�vel de encadeamento armazena o estado de cor da posi��o
	int evalValue = MAXIMUM; 
	Position auxPosition1, auxPosition2;
	
	//printf("\nEsta no evalFunctionMin()."); 
	
	if(pieceColor == WHITE)
		IAcolor = BLACK;
	else if(pieceColor == BLACK)
		IAcolor = WHITE;	
	else {
		//////printf("\nInvalid color!\n");
		return INVALID;
	}
	
	for(i = 0; i < MAX_PIECES_PER_PLAYER; i++) {
		
		id = pieces[i];
		//printf("\n1-id: %d. ", id);
		if(id == INVALID)
			break;
		
		auxPosition1 = board[(int)id/10][id%10]; //primeira posi��o
		
		//Testa se a posi��o (piecesWhite[i]) possui mais de um vizinho em linha
		/*
		if( ((auxPosition1.neighbors[0] != INVALID && auxPosition1.neighbors[0] == pieceColor) &&
			 (auxPosition1.neighbors[5] != INVALID && auxPosition1.neighbors[5] == pieceColor)) ||
			((auxPosition1.neighbors[1] != INVALID && auxPosition1.neighbors[1] == pieceColor) &&
			 (auxPosition1.neighbors[4] != INVALID && auxPosition1.neighbors[4] == pieceColor)) ||
		 	((auxPosition1.neighbors[2] != INVALID && auxPosition1.neighbors[2] == pieceColor) &&
			 (auxPosition1.neighbors[3] != INVALID && auxPosition1.neighbors[3] == pieceColor)) ) {
			continue;	
		}
		*/
		
		for(j = 0; j < NEIGHBORS; j++) {
			//Passo 1: Para cada posi��o testa se esta posi��o possui algum vizinho tamb�m preto (1-?)
			id = auxPosition1.neighbors[j];	
			//printf("\n\t2-id: %d. ", id);
			if(id == INVALID)
				continue;
			auxPosition2 = board[(int)id/10][id%10];	//segunda posi��o
			val = auxPosition2.color;
			
			if(val == pieceColor) {
				//Passo 2: primeiro vizinho � preto (2-?)
				id = auxPosition2.neighbors[j];	
				//printf("3-id: %d. ", id);
				if(id == INVALID)
					continue;
				auxPosition2 = board[(int)id/10][id%10];	//terceira posi��o
				val = auxPosition2.color;		
				
				if(val == pieceColor) { 
					//Passo 5: vizinho � preto (3-?)
					id = auxPosition2.neighbors[j];	
					//printf("4-id: %d. ", id);
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
					val = auxPosition2.color;
					
					if(val == pieceColor) { 
						//Passo 8: vizinho � preto (4) => VIT�RIA
						//printf("evalFunctionMin: Passo 8. MIN_WIN.\n");	
						evalValue = max(evalValue, MIN_WIN);
						break; //finaliza o for					
					} else {
						//Passo 9: vizinho � vazio (3-vazio), branco (3-!) ou inv�lido (3) => DERROTA
						//printf("evalFunctionMin: Passo 9. MIN_LOSS.\n");
						return MIN_LOSS;
					}
					
				} else if(val == NO_COLOR) {
					//Passo 6: vizinho � vazio (2-vazio-?)
					id = auxPosition2.neighbors[j];	
					//printf("4-id: %d. ", id);
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
					val = auxPosition2.color;
					
					if(val == pieceColor) { 
						//Passo 10: vizinho � preto (2-vazio-1) => MIN_PESO 1
						//printf("evalFunctionMin: Passo 10. MIN_PESO1.\n");
						evalValue = max(evalValue, MIN_PESO1);
					} else if(val == NO_COLOR) {
						//Passo 11: vizinho � vazio (2-vazio-vazio) => MIN_PESO 2
						//printf("evalFunctionMin: Passo 11. MIN_PESO2.\n");
						evalValue = max(evalValue, MIN_PESO2);
					} else if(val == IAcolor) {
						//Passo 12: vizinho � branco (2-vazio-!) => MIN_PESO 3
						//printf("evalFunctionMin: Passo 12. MIN_PESO3.\n");						
						evalValue = max(evalValue, MIN_PESO3);
					} else {
						//terceiro vizinho � inv�lido (2-vazio). Fim do tabuleiro. => MIN_PESO 3
						//printf("evalFunctionMin: Passo 12-b. MIN_PESO3.\n");						
						evalValue = max(evalValue, MIN_PESO3);
					}
					
				} else if(val == IAcolor) {
					//Passo 7: vizinho � branco (2-!) => DERROTA
					//printf("evalFunctionMin: Passo 7. MIN_LOSS.\n");
					return MIN_LOSS;
				} else {
					//segundo vizinho � inv�lido (2). Fim do tabuleiro. => DERROTA
					//printf("evalFunctionMin: Passo 7-b. MIN_LOSS.\n");
					return MIN_LOSS;
				}
				
			} else if(val == NO_COLOR) {
				//Passo 3: primeiro vizinho � vazio (1-vazio-?)
				id = auxPosition2.neighbors[j];	
				//printf("3-id: %d. ", id);
				if(id == INVALID)
					continue;
				auxPosition2 = board[(int)id/10][id%10];	//terceira posi��o
				val = auxPosition2.color;
				
				if(val == pieceColor) { 
					//Passo 13: vizinho � preto (1-vazio-1-?)
					id = auxPosition2.neighbors[j];	
					//printf("4-id: %d. ", id);
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
					val = auxPosition2.color;
					
					if(val == pieceColor) { 
						//Passo 16: vizinho � preto (1-vazio-2) => MIN_PESO 1
						//printf("evalFunctionMin: Passo 16. MIN_PESO1.\n");		
						evalValue = max(evalValue, MIN_PESO1);
					} else if(val == NO_COLOR) {
						//Passo 17: vizinho � vazio (1-vazio-1-vazio) => MIN_PESO 2
						//printf("evalFunctionMin: Passo 17. MIN_PESO2.\n");								
						evalValue = max(evalValue, MIN_PESO2);
					} else if(val == IAcolor) {
						//Passo 18: vizinho � branco (1-vazio-1-!) => MIN_PESO 3	
						//printf("evalFunctionMin: Passo 18. MIN_PESO3.\n");								
						evalValue = max(evalValue, MIN_PESO3);
					} else {
						//terceiro vizinho � inv�lido (1-vazio-1). Fim do tabuleiro. => MIN_PESO 3	
						//printf("Passo 18-b. MIN_PESO3.\n");								
						evalValue = max(evalValue, MIN_PESO3);
					}
				
				} else if(val == NO_COLOR) {
					//Passo 14: vizinho � vazio (1-vazio-vazio-?)
					id = auxPosition2.neighbors[j];	
					//printf("4-id: %d. ", id);
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
					val = auxPosition2.color;
					
					if(val == pieceColor) { 
						//Passo 19: vizinho � preto (1-vazio-vazio-1) => MIN_PESO 2
						//printf("evalFunctionMin: Passo 19. MIN_PESO2.\n");		
						evalValue = max(evalValue, MIN_PESO2);
					} else if(val == NO_COLOR) {
						//Passo 20: vizinho � vazio (1-vazio-vazio-vazio) => MIN_PESO 4
						//printf("evalFunctionMin: Passo 20. MIN_PESO4.\n");		
						evalValue = max(evalValue, MIN_PESO4);				
					} else if(val == IAcolor) {
						//Passo 21: vizinho � branco (1-vazio-vazio-!) => MIN_PESO 4
						//printf("evalFunctionMin: Passo 21. MIN_PESO4.\n");								
						evalValue = max(evalValue, MIN_PESO4);					
					} else {
						//terceiro vizinho � inv�lido (1-vazio-vazio). Fim do tabuleiro. => MIN_PESO 4
						//printf("evalFunctionMin: Passo 21-b. MIN_PESO4.\n");								
						evalValue = max(evalValue, MIN_PESO4);
					}
				
				} else if(val == IAcolor) {
					//Passo 15: vizinho � branco (1-vazio-!) => MIN_PESO 4
					//printf("evalFunctionMin: Passo 15. MIN_PESO4.\n");							
					evalValue = max(evalValue, MIN_PESO4);
				} else {
					//segundo vizinho � inv�lido (2). Fim do tabuleiro. => DERROTA
					//printf("evalFunctionMin: Passo 15-b. MIN_LOSS.\n");							
					return MIN_LOSS;
				}
				
			} else if(val == IAcolor) {
				//Passo 4: primeiro vizinho � branco (1-!)
				//printf("evalFunctionMin: Passo 4. MIN_LOSS.\n");		
				return MIN_LOSS;
			} else {
				//primeiro vizinho � inv�lido (1). Fim do tabuleiro.
				//printf("evalFunctionMin: Passo 4. MIN_LOSS.\n");						
				return MIN_LOSS;
			}
		}
	
	}	
	
	return evalValue; 
}


/*
Descri��o: 
	v� se h� uma situa��o de vit�ria ou derrota no jogo.
Par�metros:
	@board: tabuleiro do jogo
	@pieces: vetor com ids, os quais representam as posi��es que possuem pe�as com alguma cor
	@pieceColor: cor da pe�a
Retorno:
	valor de vit�ria ou de derrota
*/
int evalFunction(Position **board, int *pieces, int pieceColor) {
	int i, j, id;
	Position auxPosition1, auxPosition2;

	for(i = 0; i < MAX_PIECES_PER_PLAYER; i++) {
		
		id = pieces[i];
		if(id == INVALID)
			break;
		auxPosition1 = board[(int)id/10][id%10]; //primeira posi��o

		//Testa se a posi��o (piecesWhite[i]) possui mais de um vizinho em linha
		//teste: auxPosition1.neighbors[i] != INVALID => testa se o vizinho i � uma posi��o v�lida
		/*
		if( (auxPosition1.neighbors[0] != INVALID && auxPosition1.neighbors[0] == pieceColor &&
			 auxPosition1.neighbors[5] != INVALID && auxPosition1.neighbors[5] == pieceColor) ||
			(auxPosition1.neighbors[1] != INVALID && auxPosition1.neighbors[1] == pieceColor &&
			 auxPosition1.neighbors[4] != INVALID && auxPosition1.neighbors[4] == pieceColor) ||
		 	(auxPosition1.neighbors[2] != INVALID && auxPosition1.neighbors[2] == pieceColor &&
			 auxPosition1.neighbors[3] != INVALID && auxPosition1.neighbors[3] == pieceColor) ) {
			continue;	
		}
		*/	

		for(j = 0; j < NEIGHBORS; j++) {
			//Passo 1: Para cada posi��o testa se esta posi��o possui algum vizinho tamb�m branco (1-?)
			
			id = auxPosition1.neighbors[j];	
			if(id == INVALID)
				continue;
			auxPosition2 = board[(int)id/10][id%10];	//segunda posi��o
			
			if(auxPosition2.color == pieceColor) {
				//Passo 2: primeiro vizinho � branco (2-?)
				id = auxPosition2.neighbors[j];	
				if(id == INVALID)
					continue;
				auxPosition2 = board[(int)id/10][id%10];	//terceira posi��o

				if(auxPosition2.color == pieceColor) { 
					//Passo 5: vizinho � branco (3-?)
					id = auxPosition2.neighbors[j];	
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
					
					if(auxPosition2.color == pieceColor) { 
						//Passo 8: vizinho � branco (4) => VIT�RIA	
						//printf("evalFunction: MAX_WIN. ");
						return MAX_WIN;
					} else {
						//Passo 9: vizinho � vazio (3-vazio), preto (3-!) ou inv�lido (3) => DERROTA
						//printf("evalFunction: MAX_LOSS. ");
						return MAX_LOSS;
					}	
				}				
			} 
		}
	}	
	
	return INVALID; 
}

/*
Descri��o:
	retorna o maior de 2 n�meros
Par�mentros:
   	@oldVal: valor corrente do minimax
   	@newVal: novo valor a ser comparado
Retorno:
   	o maior dos 2 n�meros
*/
int max(int oldVal, int newVal) {
	if(oldVal >= newVal)
    	return oldVal;
    else
    	return newVal;
}


/*
Descri��o:
	retorna o menor de 2 n�meros
Par�mentros:
   	@oldVal: valor corrente do minimax
   	@newVal: novo valor a ser comparado
Retorno:
   	o menor dos 2 n�meros
*/
int min(int oldVal, int newVal) {
	if(oldVal <= newVal)
    	return oldVal;
    else 
    	return newVal;
}


/*
Descri��o:
	retorna o maior de 2 n�meros
Par�mentros:
   	@oldVal: valor corrente do minimax
   	@newVal: novo valor a ser comparado
   	@oldX, oldY: coordenadas da posi��o da pr�xima jogada que o minimax escolheu
   	@newX, newY: novas coordenadas para a pr�xima jogada que o minimax escolheu
Retorno:
   	o maior dos 2 n�meros
*/
int maxMinimax(int oldVal, int newVal, int *oldX, int *oldY, int newX, int newY) {
	if(oldVal >= newVal) {
    	return oldVal;
    }
    else {
    	*oldX = newX;
    	*oldY = newY;
    	return newVal;
    }
}


/*
Descri��o: 
	fun��o auxiliar de seedMinimax. D� prosseguimento ao minimax, simulando as jogadas 
	futuras em c�pias do tabuleiro.
Par�metros:
	@board: tabuleiro s�o simuladas as jogadas(c�pia do tabuleiro)
	@depth: n�veis de ramifica��o
	@row: linha de uma jogada simulada
	@column: coluna de uma jogada simulada
	@color: cor que iniciar a jogada. Nas pr�ximas jogadas as cores se alternam sucessivamente.
	@function (MAXIMUM ou MINIMUM): diz se o jogador que inicia a jogada ser� MIN ou MAX. Nas 
	pr�ximas jogadas as fun��es MIN e MAX se alternam sucessivamente.
	@alpha e beta: para a poda.
Retorno:
	retorna o minimax desejado
*/
int expandTree(Round *round, int depth, int row, int column, int color, int function, int *alpha, int *beta) {
	int i, j, temp;
	int maxValue = MINIMUM; //colocar uma constante com valor baixo
	int minValue = MAXIMUM; //colocar uma constante com valor alto  
  	Round *copy;
  	int breakLoops = 0;

	copy = copyRound(round);
	updateRoundMinimax(copy, color, row, column);
	
	//avalia de houve derrota ou vit�ria
	//REVISAR -> S� ANALISA AS PE�AS DA IA
	if(color == BLACK) {
		temp = evalFunction(copy->board, copy->playerBlack.markedPieces, BLACK);
	}
	else {
		temp = evalFunction(copy->board, copy->playerWhite.markedPieces, WHITE);
	}
	
	if(temp == MAX_WIN || temp == MAX_LOSS) {
		free(copy);
		return temp;
	}
	
  	if(depth > 0) {
		//troca a cor do jogador para o pr�ximo n�vel da �rvore
		if(color == BLACK)
			color = WHITE;	
        else
			color = BLACK;

		//continua o minimax
		if(function == MAXIMUM) {
			for(i = 0;(i < BOARD_ROW) && (!breakLoops); i++) {
				for(j = 0;(j < BOARD_COLUMN) && (!breakLoops); j++) {
					if(copy->board[i][j].color == NO_COLOR) {
						//printf("\texpandTree: depth %d, IA joga em (%d,%d). ", depth, i, j);
						minValue = min(minValue, expandTree(copy, (depth - 1), i, j, color, MINIMUM, alpha, beta));	
						//printf("\nminValue: %d\n", minValue);
						
						if(minValue <= *alpha){
							breakLoops = 1;
						}
					}
				}	
			}
			//printf("\tminValue: %d\n", maxValue);
			free(copy);
			*beta = min(*beta, minValue);
			return maxValue;	
		}
		else {
			for(i = 0;(i < BOARD_ROW) && (!breakLoops); i++) {
				for(j = 0;(j < BOARD_COLUMN) && (!breakLoops); j++) {
					if(copy->board[i][j].color == NO_COLOR) {
						//printf("\texpandTree: depth %d, Humano joga em (%d,%d). ", depth, i, j);
						maxValue = max(maxValue, expandTree(copy, (depth - 1), i, j, color, MINIMUM, alpha, beta));
						//printf("\nmaxValue: %d\n", maxValue);
						
						if(maxValue >= *beta){
							breakLoops = 1;
						}
					}
				}	
			}
			free(copy);
			//printf("\tmaxValue: %d\n", maxValue);
			*alpha = max(*alpha, maxValue);
			return minValue;
		}

  	}
  	else {
  		if(function == MAXIMUM) {
  			printf("lala");
  			if(color == BLACK) {
				temp = evalFunctionMin(copy->board, copy->playerBlack.markedPieces, BLACK);
			}
		    else {
			    temp = evalFunctionMin(copy->board, copy->playerWhite.markedPieces, WHITE);
			}
			return temp;
    	}
        else {
        	printf("lele");
  			if(color == BLACK) {
  				temp = evalFunctionMax(copy->board, copy->playerBlack.markedPieces, BLACK);
  			}
		    else {
			    temp = evalFunctionMax(copy->board, copy->playerWhite.markedPieces, WHITE);
			}
			return temp;
        }

  	}
}


/*
Descri��o: 
	v� se h� uma jogada do tipo 2-vazio-1. Inicia avaliando as pe�as da IA e depois as pe�as do
	advers�rio.
Par�metros:
	@board: tabuleiro do jogo
	@piecesIA: vetor com ids, os quais representam as posi��es que possuem pe�as da IA
	@pieceColorIA: cor da pe�a da IA
	@piecesEnemy: vetor com ids, os quais representam as posi��es que possuem pe�as do inimigo
	@pieceColorEnemy: cor da pe�a do inimigo
	@x, y: pointeiros para armazenar a posi��o de onde h� a posi��o vazio da jogada 2-vazio-1,
	se ela existir.
Retorno:
	retorna FOUND se encontrar a jogada 2-vazio-1 ou ERROR caso contr�rio.
*/
int evalTwoEmptyOne(Position **board, int *piecesIA, int pieceColorIA, int *piecesEnemy, int pieceColorEnemy, int *x, int *y) {
	int i, j, k, id;
	int val; //a cada n�vel de encadeamento armazena o estado de cor da posi��o
	int pieceColor; 
	Position auxPosition1, auxPosition2;
	
	//printf("\nEsta no evalTwoEmptyOne()."); 

	for(k = 0; k < 2; k++) {
		if(k == 0)
			pieceColor = pieceColorIA;
		else if(k == 1)
			pieceColor = pieceColorEnemy;
	
		for(i = 0; i < MAX_PIECES_PER_PLAYER; i++) {
		
			if(k == 0)
				id = piecesIA[i];
			else if(k == 1)
				id = piecesEnemy[i];
			//printf("\n1-id: %d. ", id);
			if(id == INVALID)
				break;
		
			auxPosition1 = board[(int)id/10][id%10]; //primeira posi��o
		
			//Testa se a posi��o (piecesWhite[i]) possui mais de um vizinho em linha
			if( (auxPosition1.neighbors[0] != INVALID && auxPosition1.neighbors[0] == pieceColor &&


				 auxPosition1.neighbors[5] != INVALID && auxPosition1.neighbors[5] == pieceColor) ||
				(auxPosition1.neighbors[1] != INVALID && auxPosition1.neighbors[1] == pieceColor &&
				 auxPosition1.neighbors[4] != INVALID && auxPosition1.neighbors[4] == pieceColor) ||
			 	(auxPosition1.neighbors[2] != INVALID && auxPosition1.neighbors[2] == pieceColor &&
				 auxPosition1.neighbors[3] != INVALID && auxPosition1.neighbors[3] == pieceColor) ) {
				continue;	
			}
		
			for(j = 0; j < NEIGHBORS; j++) {
				//Passo 1: Para cada posi��o testa se esta posi��o possui algum vizinho tamb�m preto (1-?)
				id = auxPosition1.neighbors[j];	
				//printf("\n\t2-id: %d. ", id);
				if(id == INVALID)
					continue;
			
				*x = (int)id/10;
				*y = id%10;	
				auxPosition2 = board[*x][*y];	//segunda posi��o
				val = auxPosition2.color;
			
				if(val == pieceColor) {
					//Passo 2: primeiro vizinho � preto (2-?)
					id = auxPosition2.neighbors[j];	
					//printf("3-id: %d. ", id);
					if(id == INVALID)
						continue;
					
					*x = (int)id/10;
					*y = id%10;		
					auxPosition2 = board[*x][*y];	//terceira posi��o
					val = auxPosition2.color;		
				
					if(val == NO_COLOR) {
						//Passo 6: vizinho � vazio (2-vazio-?)
						id = auxPosition2.neighbors[j];	
						//printf("4-id: %d. ", id);
						if(id == INVALID)
							continue;
						auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
						val = auxPosition2.color;
					
						if(val == pieceColor) { 
							//Passo 10: vizinho � preto (2-vazio-1) => MIN_PESO 1
							//printf("evalFunctionMin: Passo 10. MIN_PESO1.\n");
							return FOUND;
						} 
					} 
				
				} else if(val == NO_COLOR) {
					//Passo 3: primeiro vizinho � vazio (1-vazio-?)
					id = auxPosition2.neighbors[j];	
					//printf("3-id: %d. ", id);
					if(id == INVALID)
						continue;
					auxPosition2 = board[(int)id/10][id%10];	//terceira posi��o
					val = auxPosition2.color;
				
					if(val == pieceColor) { 
						//Passo 13: vizinho � preto (1-vazio-1-?)
						id = auxPosition2.neighbors[j];	
						//printf("4-id: %d. ", id);
						if(id == INVALID)
							continue;
						auxPosition2 = board[(int)id/10][id%10];	//quarta posi��o
						val = auxPosition2.color;
					
						if(val == pieceColor) { 
							//Passo 16: vizinho � preto (1-vazio-2) => MIN_PESO 1
							//printf("evalFunctionMin: Passo 16. MIN_PESO1.\n");		
							return FOUND;
						} 
					}
				} 
			}
		}	
	}
	
	return ERROR; 	
}


/*
Descri��o:
	fun��o que ser� chamada para fazer o minimax. A cada n�vel de itera��o � simulada a jogada
	de um jogador diferente � retornado o valor correspondente do minimax.
Par�metros:
	@globalRound: tabuleiro onde s�o feitas as jogadas
	@depth: n�veis de ramifica��o
	@color: cor que iniciar a jogada. Nas pr�ximas jogadas as cores se alternam sucessivamente.
	@function (MAXIMUM ou MINIMUM): diz se o jogador que inicia a jogada ser� MIN ou MAX. Nas 
	pr�ximas jogadas as fun��es MIN e MAX se alternam sucessivamente.
*/
void seedMinimax(Round *globalRound, int depth, int color, int *x, int *y) {
	int i, j;
	int maxValue = MINIMUM;
  	int testTwoEmptyOne;
  	int alpha = MINIMUM;
	int beta = MAXIMUM;
  
   	//avalia se h� uma forma��o do tipo 2-vazio-1. Se houver coloca uma pe�a da IA ali	
  	if(color == BLACK)
  		testTwoEmptyOne = evalTwoEmptyOne(globalRound->board, 
  										  globalRound->playerBlack.markedPieces, BLACK,
  										  globalRound->playerWhite.markedPieces, WHITE,
  										  x, y);
	else
		testTwoEmptyOne = evalTwoEmptyOne(globalRound->board, 
										  globalRound->playerWhite.markedPieces, WHITE,
  										  globalRound->playerBlack.markedPieces, BLACK,
  										  x, y);
  
  	
  	if(testTwoEmptyOne == FOUND) {
  		//printf("There is a 2-empty-1 formation in (%d, %d).\n", *x, *y);
  		return;
  	}
  	else
  		//printf("evalTwoEmptyOne returned ERROR.\n");
  	
	for(i = 0; i < BOARD_ROW; i++) {
  		for(j = 0; j < BOARD_COLUMN; j++) {
  			if(globalRound->board[i][j].color == NO_COLOR) {
  				//printf("depth: %d\n", depth);
  				//printf("\tseedMinimax: IA joga em (%d,%d).\n", i, j);
	  				maxValue = maxMinimax(maxValue, expandTree(globalRound, (depth - 1), i, j, color, MINIMUM, &alpha, &beta), x, y, i, j);
  			}
  		}
  	}

	/*  	
	if(color == WHITE)
		//printf("\tPlayer WHITE, next move (%d, %d), Seed maxValue: %d\n", *x, *y, maxValue);
	else
		//printf("\tPlayer BLACK, next move (%d, %d), Seed maxValue: %d\n", *x, *y, maxValue);
	*/
}


/*
Descri��o:

Vari�veis Globais:
	@x, y: coordenadas da jogada da IA

Par�metros:
*/
void firstMove(Round *round) {
	int moves[20] = { 22, 23, 24, 35, 44, 46, 56, 66, 65, 64, 53, 42, 32, 33, 34, 35, 45, 55, 54, 43 };
	int id;

	srand( time(NULL) );
	do {
		id = moves[rand()%20];
		x = (int) id / 10;
		y = id % 10;
		if(round->board[x][y].color == NO_COLOR) {
			//printf("\nJogar na posicao (%d,%d).\n", x, y);
			break; 
		}
	} while(1);
}










