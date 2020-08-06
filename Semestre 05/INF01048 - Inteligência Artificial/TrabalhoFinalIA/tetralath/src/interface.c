#include "../inc/interface.h"

// -----------------------------------------------------------------------------
// Fun��es
// -----------------------------------------------------------------------------


/*
Descri��o: 
	realiza uma s�rie de inicializa��es para o jogo. Inicializa vari�veis
	globais (imagem do tabuleiro) e a glut
Par�metros:
	@filename: nome da imagem do tabuleiro
Vari�veis Globais:
	@boardPixels, @boardImg: para carregar a imagem do tabuleiro
	@piecePixels, @pieceImg: para carregar as imagens das pe�as
	@piecesPlayer: vetores que cont�m as pe�as que cada jogador colocou no tabuleiro
	@player: jogador atual
*/
void initInterface() {
	int i, j;
	
	//inicializa as listas globais de pe�as
	piecesPlayer[PLAYER_HUMAN] = NULL;
	piecesPlayer[PLAYER_PC] = NULL;
	
	//escolhe o primeiro jogador
	do {
		printf("Escolha o primeiro jogador:\n");
		printf("\t(0) Humano.\n");
		printf("\t(1) PC.\n");
		printf("Escolha: ");
		scanf("%d", &currentPlayer);
		
		if(currentPlayer < 0 || currentPlayer > 1)
			fprintf(stderr, "Entrada invalida.\n");
	} while(currentPlayer < 0 || currentPlayer > 1);
	firstPlayer = currentPlayer;

	//inicializa o ponteiro para o vetor de pixels da imagem do tabuleiro
	boardPixels = loadImage( "res/tabuleiro2.jpg", &boardImg, BOARD );
	
	//inicializa a imagem das pe�as de acordo com o jogador
	piecePixels[currentPlayer] = loadImage( "res/jedi.jpg", &pieceImg[currentPlayer], currentPlayer );
	if(currentPlayer == PLAYER_PC)
		piecePixels[PLAYER_HUMAN] = loadImage( "res/sith.jpg", &pieceImg[PLAYER_HUMAN], PLAYER_HUMAN );
	else
		piecePixels[PLAYER_PC] = loadImage( "res/sith.jpg", &pieceImg[PLAYER_PC], PLAYER_PC );

	//seta a glut
	setGlut();

	//espelha a imagem veticalmente, para que ela n�o fique de cabe�a para baixo
	verticalFlip(boardPixels, BOARD);
	verticalFlip(piecePixels[PLAYER_HUMAN], PLAYER_HUMAN);
	verticalFlip(piecePixels[PLAYER_PC], PLAYER_PC);	

	//inicializa a matriz de pe�as marcadas (para a interface)
	for(i = 0; i < BOARD_ROW; i++)
		for(j = 0; j < BOARD_COLUMN; j++)
			markedPieces[i][j] = 0;		
			
	//inicializa o estado do jogo
	gameState = GAME_RUNNING;		
}


/* 
Descri��o: 
	carrega uma imagem
Parameters:
	@filename: nome do arquivo que se quer carregar
	@img: ponteiro de ponteiros do tipo IplImage. Usado para armazenar 
	informa��es da imagem	
	@object: n�mero da imagem que estamos trabalhando
Vari�vel Globais:
	@width, @height, @channel: informa��es da imagem
Return0: 
	retorna o campo  imageData da estrutura IplImage, que representa as
	informa��es da imagem
*/
unsigned char* loadImage( char* filename, IplImage** img, int object ) {
	
	//estrutura de dados para a imagem
	*img = NULL;

	//carrega a imagem
	*img = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);

	if( *img == NULL ) {
		fprintf( stderr, "Cannot load file %s!\n", filename );
		exit(1);
	}

	//Informa��es da imagem
	width[object] = (*img)->width;
	height[object] = (*img)->height;
	channel[object] = (*img)->nChannels;
	
	/* o campo imageData da estrutura IplImg contem os pixels da imagem
	em um ponteiro unsigned char */
	return (unsigned char*)(*img)->imageData;
}


/*
Descri��o:
	espelha a imagem no eixo y (verticalmente)
Par�metros:
	@imagePixels: vetor de pixels da imagem
	@object: n�mero da imagem que estamos trabalhando
vari�veis Globais:
	@boardPixels: ponteiro que represeta a imagem do tabuleiro
	@width, @height, @channel: informa��es da imagem
*/
void verticalFlip(unsigned char *imagePixels, int object) {

	unsigned char *auxPixels;
	int i;
	
	//aloca mem�ria para a representa��o auxilixar dos pixels
	auxPixels = (unsigned char *) malloc(width[object] * height[object] * channel[object] * sizeof(unsigned char));
	
	if( auxPixels ) {
		for( i = 0; i < height[object]; i++ ) 
			memcpy( auxPixels + (i * width[object] * channel[object]), 
					imagePixels + ((height[object] - 1) * width[object] * channel[object]) - (i * width[object] * channel[object]), 
					width[object] * channel[object] );
	}
	
	//copia o conte�do de auxPixels para boardPixels
	memcpy( imagePixels, auxPixels, width[object] * height[object] * channel[object] );

	free( auxPixels );

	glutPostRedisplay();
}


// -----------------------------------------------------------------------------
// Fun��es da GLUT
// -----------------------------------------------------------------------------

/*
Descri��o:
	rotina de tratamento de evento, altera o tamanho da janela
Par�metros:
	@width, @height: resolu��o da tela
*/
void reshape(int width, int height) { 
	glViewport( 0, 0, (GLsizei) width, (GLsizei) height ); 
	glMatrixMode( GL_PROJECTION ); 
	glLoadIdentity(); 
	gluOrtho2D( 0.0, width, 0.0, height ); 
	glMatrixMode( GL_MODELVIEW ); 
	glLoadIdentity(); 
}



/*
Descri��o:
	rotina de tratamento de evento, desenha a imagem na janela
Vari�veis Globais:
	@width, @height: resolu��o da tela
	@boardPixels: ponteiro para representar a imagem do tabuleiro
	@piecesPlayerHuman: vetor de pares ordenados das pe�as pretas
	@piecesPlayerPC: vetor de pares ordenados das pe�as brancas
*/
void display() { 
	Par *paux;
	
	glClear( GL_COLOR_BUFFER_BIT ); 
	glRasterPos2i( 0, 0 ); 
	glDrawPixels( width[BOARD], height[BOARD], GL_BGR, GL_UNSIGNED_BYTE, boardPixels ); 

	//desenha as pe�as do jogador humano
	if(piecesPlayer[PLAYER_HUMAN] != NULL) {
		for(paux = piecesPlayer[PLAYER_HUMAN]; paux != NULL; paux = paux->nextPar) {
			drawPiece(paux, PLAYER_HUMAN);
		}
	}
	
	//desenha as pe�as do jogador PC
	if(piecesPlayer[PLAYER_PC] != NULL) {
		for(paux = piecesPlayer[PLAYER_PC]; paux != NULL; paux = paux->nextPar)
			drawPiece(paux, PLAYER_PC);
	}
	
	//desenha o texto que qual jogador deve jogar
	drawPlayerText();
	glutSwapBuffers();

	//testa se o jogo terminou
	if(gameState == GAME_END) {
		printf("ENTER para terminar.\n");
		getchar(); getchar(); getchar(); exit(0);
	}
}


/*
Descri��o:
	inicializa a GLUT na forma padr�o
Vari�veis Globais:
	@width, @height: resolu��o da tela
	@windows: manipulador de janelas da glut
*/
void setGlut() {
	
	/* aux and aux2 are passed to glutInit to make it initialize 
	glut in default way */
	char *aux = NULL; int aux2 = 1;
	glutInit( &aux2, &aux );

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); 
	glutInitWindowPosition(0, 0); 
	glutInitWindowSize(width[BOARD], height[BOARD]); 
	windows[MAIN] = glutCreateWindow("Tetralath"); 
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape); 
	glutMouseFunc(mouse);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
	glClearColor(0.0, 0.0, 0.0, 0);
}


/*
Descri��o:
	inicializa a GLUT na forma padr�o
Par�metros:
	@par: coordenadas para impress�o da pe�a
	@player: diz qual o jogador (m�quina/branco ou humano/preto)
*/
void drawPiece(Par *par, int player) {

	//setando posi��o de origem para come�ar a desenhar	
	glRasterPos2i(par->x, par->y);

	//desenha os pixels na tela
	glDrawPixels( width[player], height[player], GL_BGR, GL_UNSIGNED_BYTE, piecePixels[player] ); 
}


/*
Descri��o:
	desenha na tela a pe�a do jogador atual e o nome do jogador atual
	http://stackoverflow.com/questions/538661/how-do-i-draw-text-with-glut-opengl-in-c
Vari�veis Globais:
	@height, @width: dimens�es da imagem da pe�a e do tabuleiro
	@player: jogador atual
*/
void drawPlayerText() {
	char output[100];

	//setando posi��o de origem para come�ar a desenhar	
	glRasterPos2i(10, height[BOARD] - height[currentPlayer] - 5);

	//desenha os pixels na tela
	glDrawPixels( width[currentPlayer], height[currentPlayer], GL_BGR, GL_UNSIGNED_BYTE, piecePixels[currentPlayer] ); 
	
	//desenha o texto
  	glColor3f(0.0f, 0.0f, 0.0f); 
	glRasterPos2i(20 + width[currentPlayer], height[BOARD] - height[currentPlayer]/2 - 10);

	if(currentPlayer == PLAYER_HUMAN)
	  	glutBitmapString(GLUT_BITMAP_HELVETICA_18, "Player Human");		
	else {
		memset(output, '\0', 100);
		sprintf(output, "Player PC: jogar em (%d,%d)", x, y);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, output);	
	}
}




/*
Descri��o:
	ajusta as coordenadas de centro de uma posi��o para imprimir a 
	pe�a nesta posi��o
Par�metros:
	@x, @y: coordenadas do centro da posi��o	
Vari�veis Globais:
	@height[BOARD]: tamanho vertical da janela
Retorno: 
	vari�vel do tipo Par, como o par ordenado ajustado
	
Coordenadas de tela que a glDrawPixels v� (x,y): 
(0,4)	(1,4)	(2,4)	(3,4)	(4,4)
(0,3)	(1,3)	(2,3)	(3,3)	(4,3)
(0,2)	(1,2)	(2,2)	(3,2)	(4,2)
(0,1)	(1,1)	(2,1)	(3,1)	(4,1)
(0,0)	(1,0)	(2,0)	(3,0)	(4,0)

Coordenadas de tela que o clique do mouse v� (x,y):
(0,0)	(1,0)	(2,0)	(3,0)	(4,0)
(0,1)	(1,1)	(2,1)	(3,1)	(4,1)
(0,2)	(1,2)	(2,2)	(3,2)	(4,2)
(0,3)	(1,3)	(2,3)	(3,3)	(4,3)
(0,4)	(1,4)	(2,4)	(3,4)	(4,4)

Ent�o, dado (x,y) em coordenadas do clique do mouse, deveos transformar
para as coordenadas da glDrawPixels:

x = x;
y = maxY - y;

Agora temos nosso par ordenado em coordenadas da glDrawPixels. Mas ainda � 
preciso fazer uma nova manipula��o, pois para a glDrawPixels, esse par � o 
par de in�cio de desenho. N�o queremos come�ar a desenhar a pe�a do centro
de uma posi��o do tabuleiro em diante, ent�o � preciso pegar o par (x,y)
da borda inferior esquerda da posi��o do tabuleiro, para que a glDrawPixels
consiga desenhar a pe�a sobre a posi��o.	
*/
Par ajustPositionCoordinates(int x, int y) {
	Par par;

	//Ajustando as coordanadas para a impress�o	
	par.x = x - PIECE_OFFSET;
	par.y = height[BOARD] - y - PIECE_OFFSET;
	
	return par;
}


/*
Descri��o:
	insere um novo par de coordenadas na lista de pe�as
Par�metros:
	@par: coordenadas para inserir na lista
	@piecesPlayerX: lista de coordenadas de pe�as de um player
Retorno: 
	um ponteiro para o in�cio da lista de coordenadas de pe�as
*/
Par *insertPar(Par par, Par *piecesPlayerX) {

	Par *piecesAux = piecesPlayerX;
	Par *newPiece;
	
	if(piecesPlayerX == NULL) {
		//O primeiro elemento da lista ainda � vazio
		piecesPlayerX = (Par *) malloc(sizeof(Par));
		piecesPlayerX->x = par.x;
		piecesPlayerX->y = par.y;
		piecesPlayerX->nextPar = NULL; 	
	}
	else {
		//Inicializa a nova pe�a
		newPiece = (Par *) malloc(sizeof(Par)); 	
		newPiece->x = par.x;
		newPiece->y = par.y;
		newPiece->nextPar = NULL;
	
		//Procura o final da lista
		while(piecesAux->nextPar != NULL) {
			piecesAux = piecesAux->nextPar;
		}			
		
		//Nova pe�a adicionada � lista
		piecesAux->nextPar = newPiece;
	}
	
	return piecesPlayerX;
}


/*
Descri��o:
	troca o jogador. esta fun��o � chamada dentro da fun��o 
	ajustPositionCoordinates, pois assim garantimos o jogador ir�
	trocar apenas se o clique foi v�lido (selecionar uma posi��o)
Vari�veis Globais:
	@player: jogador atual
*/
void changePlayer() {

	if(currentPlayer == PLAYER_HUMAN) {
		currentPlayer = PLAYER_PC;
		//printf("currentPlayer: Player_pc\n");
	}
	else {
		currentPlayer = PLAYER_HUMAN;
		//printf("currentPlayer: Player_human\n");
	}
}


/*
Descri��o:
	imprime a lista de coordenadas de pe�as de um jogador
Par�metros:
	@piecesPlayerX: lista de coordenadas de pe�as de um player
*/
void printPieceList(Par *piecesPlayerX) {

	Par *piecesAux = piecesPlayerX;
	int i;	
		
	i = 0;	
	while(piecesAux != NULL) {
		printf("Elemento %d:\n", i);
		printf("\tx: %d\n", piecesAux->x);
		printf("\ty: %d\n", piecesAux->y);
	
		piecesAux = piecesAux->nextPar;
		i++;
	}	
}


/*
Descri��o:
	rotina de tratamento de evento, captura o clique do mouse
Par�metros:
	@button: bot�o do mouse que foi clicado (direito ou esquerdo)
	@state: estado do bot�o do mouse, se est� clicado ou n�o
	@x, y: coordenadas do clique
Vari�veis Globais:
	@player: jogador atual
*/
void mouse(int button, int state, int x, int y) {
	int iniCount, finalCount; //para contar se alguma posi��o foi marcada	
	int color;

	//s� deixa o jogador humano ter posi��es marcadas pelo clique do mouse
	if(currentPlayer != PLAYER_HUMAN)
		return;

	//quantidade de pe�as marcadas inicial
	iniCount = countZeros(markedPieces);
	
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
	
		if(globalRound->playerWhite.player == currentPlayer)
			color = WHITE;
		else
			color = BLACK;
	
		//printf("(x:%d, y:%d)\n", x, y);
		if(BETWEEN(y, 30, 230)) { //up
			if(BETWEEN(x, 35, 255)) { //left
				/* Pe�as deste quadrante
						 (0,0) (0,1) (0,2)
					  (1,0) (1,1) (1,2)
				   (2,0) (2,1) (2,2) (2,3)	
				(3,0) (3,1) (3,2) (3,3)
				*/
				if(IN(x, y, POSITION_X_INI + 4*POSITION_X_OFFSET, POSITION_Y_INI) && !markedPieces[0][0]) { //(0,0)
					markedPieces[0][0] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 4*POSITION_X_OFFSET, 
												POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 0, 0);
				}
				else if(IN(x, y, POSITION_X_INI + 6*POSITION_X_OFFSET, POSITION_Y_INI) && !markedPieces[0][1]) { //(0,1)
					markedPieces[0][1] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 6*POSITION_X_OFFSET, 
												POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 0, 1);
				}
				else if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI) && !markedPieces[0][2]) { //(0,2)
					markedPieces[0][2] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET, 
												POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 0, 2);
				}
				else if(IN(x, y, POSITION_X_INI + 3*POSITION_X_OFFSET, POSITION_Y_INI + POSITION_Y_OFFSET) && !markedPieces[1][0]) { //(1,0)
					markedPieces[1][0] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 3*POSITION_X_OFFSET, 
												POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 1, 0);
				}
				else if(IN(x, y, POSITION_X_INI + 5*POSITION_X_OFFSET, POSITION_Y_INI + POSITION_Y_OFFSET) && !markedPieces[1][1]) { //(1,1)
					markedPieces[1][1] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 5*POSITION_X_OFFSET, 
												POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 1, 1);
				}
				else if(IN(x, y, POSITION_X_INI + 7*POSITION_X_OFFSET, POSITION_Y_INI + POSITION_Y_OFFSET) && !markedPieces[1][2]) { //(1,2)
					markedPieces[1][2] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 7*POSITION_X_OFFSET, 
												POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 1, 2);
				}
				else if(IN(x, y, POSITION_X_INI + 2*POSITION_X_OFFSET, POSITION_Y_INI + 2*POSITION_Y_OFFSET) && !markedPieces[2][0]) { //(2,0)
					markedPieces[2][0] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 2*POSITION_X_OFFSET, 
												POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 2, 0);
				}
				else if(IN(x, y, POSITION_X_INI + 4*POSITION_X_OFFSET, POSITION_Y_INI + 2*POSITION_Y_OFFSET) && !markedPieces[2][1]) { //(2,1)
					markedPieces[2][1] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 4*POSITION_X_OFFSET, 
												POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 2, 1);
				}
				else if(IN(x, y, POSITION_X_INI + 6*POSITION_X_OFFSET, POSITION_Y_INI + 2*POSITION_Y_OFFSET) && !markedPieces[2][2]) { //(2,2)
					markedPieces[2][2] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 6*POSITION_X_OFFSET, 
												POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);	
					updateRound(globalRound, color, 2, 2);
				}
				else if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI + 2*POSITION_Y_OFFSET) && !markedPieces[2][3]) { //(2,3)
					markedPieces[2][3] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET, 
												POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);						
					updateRound(globalRound, color, 2, 3);
				}
				else if(IN(x, y, POSITION_X_INI + POSITION_X_OFFSET, POSITION_Y_INI + 3*POSITION_Y_OFFSET) && !markedPieces[3][0]) { //(3,0)
					markedPieces[3][0] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + POSITION_X_OFFSET, 
												POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 3, 0);
				}
				else if(IN(x, y, POSITION_X_INI + 3*POSITION_X_OFFSET, POSITION_Y_INI + 3*POSITION_Y_OFFSET) && !markedPieces[3][1]) { //(3,1)
					markedPieces[3][1] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 3*POSITION_X_OFFSET, 
												POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 3, 1);
				}
				else if(IN(x, y, POSITION_X_INI + 5*POSITION_X_OFFSET, POSITION_Y_INI + 3*POSITION_Y_OFFSET) && !markedPieces[3][2]) { //(3,2)
					markedPieces[3][2] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 5*POSITION_X_OFFSET, 
												POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);	
					updateRound(globalRound, color, 3, 2);
				}
				else if(IN(x, y, POSITION_X_INI + 7*POSITION_X_OFFSET, POSITION_Y_INI + 3*POSITION_Y_OFFSET) && !markedPieces[3][3]) { //(3,3)
					markedPieces[3][3] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 7*POSITION_X_OFFSET, 
												POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);														
					updateRound(globalRound, color, 3, 3);
				}
			}
			if(BETWEEN(x, 256, 500)) { //right
				/* Pe�as deste quadrante
				(0,2) (0,3) (0,4)
				   (1,3) (1,4) (1,5)
				(2,3) (2,4) (2,5) (2,6) 	
				   (3,4) (3,5) (3,6) (3,7)
				*/
				if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI) && !markedPieces[0][2]) { //(0,2)
					markedPieces[0][2] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET, 
												POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 0, 2);
				}
				else if(IN(x, y, POSITION_X_INI + 10*POSITION_X_OFFSET, POSITION_Y_INI) && !markedPieces[0][3]) { //(0,3)
					markedPieces[0][3] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 10*POSITION_X_OFFSET - POSITION_X_AJUST, 
												POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 0, 3);
				}
				else if(IN(x, y, POSITION_X_INI + 12*POSITION_X_OFFSET, POSITION_Y_INI) && !markedPieces[0][4]) { //(0,4)
					markedPieces[0][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 12*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 0, 4);
				}
				else if(IN(x, y, POSITION_X_INI + 9*POSITION_X_OFFSET, POSITION_Y_INI + POSITION_Y_OFFSET) && !markedPieces[1][3]) { //(1,3)
					markedPieces[1][3] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 9*POSITION_X_OFFSET, 
												POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 1, 3);
				}
				else if(IN(x, y, POSITION_X_INI + 11*POSITION_X_OFFSET, POSITION_Y_INI + POSITION_Y_OFFSET) && !markedPieces[1][4]) { //(1,4)
					markedPieces[1][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 11*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 1, 4);
				}
				else if(IN(x, y, POSITION_X_INI + 13*POSITION_X_OFFSET, POSITION_Y_INI + POSITION_Y_OFFSET) && !markedPieces[1][5]) { //(1,5)
					markedPieces[1][5] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 13*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);	
					updateRound(globalRound, color, 1, 5);
				}
				else if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI + 2*POSITION_Y_OFFSET) && !markedPieces[2][3]) { //(2,3)
					markedPieces[2][3] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET, 
												POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);						
					updateRound(globalRound, color, 2, 3);
				}
				else if(IN(x, y, POSITION_X_INI + 10*POSITION_X_OFFSET, POSITION_Y_INI + 2*POSITION_Y_OFFSET) && !markedPieces[2][4]) { //(2,4)
					markedPieces[2][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 10*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 2, 4);
				}
				else if(IN(x, y, POSITION_X_INI + 12*POSITION_X_OFFSET, POSITION_Y_INI + 2*POSITION_Y_OFFSET) && !markedPieces[2][5]) { //(2,5)
					markedPieces[2][5] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 12*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 2, 5);
				}
				else if(IN(x, y, POSITION_X_INI + 14*POSITION_X_OFFSET, POSITION_Y_INI + 2*POSITION_Y_OFFSET) && !markedPieces[2][6]) { //(2,6)
					markedPieces[2][6] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 14*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);	
					updateRound(globalRound, color, 2, 6);
				}
				else if(IN(x, y, POSITION_X_INI + 9*POSITION_X_OFFSET, POSITION_Y_INI + 3*POSITION_Y_OFFSET) && !markedPieces[3][4]) { //(3,4)
					markedPieces[3][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 9*POSITION_X_OFFSET - POSITION_X_AJUST, 
												POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 3, 4);
				}
				else if(IN(x, y, POSITION_X_INI + 11*POSITION_X_OFFSET, POSITION_Y_INI + 3*POSITION_Y_OFFSET) && !markedPieces[3][5]) { //(3,5)
					markedPieces[3][5] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 11*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 3, 5);
				}
				else if(IN(x, y, POSITION_X_INI + 13*POSITION_X_OFFSET, POSITION_Y_INI + 3*POSITION_Y_OFFSET) && !markedPieces[3][6]) { //(3,6)
					markedPieces[3][6] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 13*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 3, 6);
				}
				else if(IN(x, y, POSITION_X_INI + 15*POSITION_X_OFFSET, POSITION_Y_INI + 3*POSITION_Y_OFFSET) && !markedPieces[3][7]) { //(3,7)
					markedPieces[3][7] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 15*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);	


					updateRound(globalRound, color, 3, 7);	
				}
			}
		}
		else if(BETWEEN(y, 235, 480)) { //down
			if(BETWEEN(x, 35, 255)) { //left
				/* Pe�as deste quadrante
				(4,0) (4,1) (4,2) (4,3) (4,4)
				   (5,1) (5,2) (5,3) (5,4)	

					  (6,2) (6,3) (6,4) (6,5)
						 (7,3) (7,4) (7,5)
							(8,4) (8,5) (8,6)
				*/
				if(IN(x, y, POSITION_X_INI, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][0]) { //(4,0)
					markedPieces[4][0] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 0);	
				}
				else if(IN(x, y, POSITION_X_INI + 2*POSITION_X_OFFSET, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][1]) { //(4,1)
					markedPieces[4][1] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 2*POSITION_X_OFFSET, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 1);	
				}
				else if(IN(x, y, POSITION_X_INI + 4*POSITION_X_OFFSET, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][2]) { //(4,2)
					markedPieces[4][2] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 4*POSITION_X_OFFSET, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 2);	
				}
				else if(IN(x, y, POSITION_X_INI + 6*POSITION_X_OFFSET, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][3]) { //(4,3)
					markedPieces[4][3] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 6*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 3);	
				}
				else if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][4]) { //(4,4)
					markedPieces[4][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 4);	
				}
				else if(IN(x, y, POSITION_X_INI + POSITION_X_OFFSET, POSITION_Y_INI + 5*POSITION_Y_OFFSET) && !markedPieces[5][1]) { //(5,1)
					markedPieces[5][1] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + POSITION_X_OFFSET, 
												POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 5, 1);	
				}
				else if(IN(x, y, POSITION_X_INI + 3*POSITION_X_OFFSET, POSITION_Y_INI + 5*POSITION_Y_OFFSET) && !markedPieces[5][2]) { //(5,2)
					markedPieces[5][2] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 3*POSITION_X_OFFSET, 
												POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 5, 2);	
				}
				else if(IN(x, y, POSITION_X_INI + 5*POSITION_X_OFFSET, POSITION_Y_INI + 5*POSITION_Y_OFFSET) && !markedPieces[5][3]) { //(5,3)
					markedPieces[5][3] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 5*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 5, 3);	
				}
				else if(IN(x, y, POSITION_X_INI + 7*POSITION_X_OFFSET, POSITION_Y_INI + 5*POSITION_Y_OFFSET) && !markedPieces[5][4]) { //(5,4)
					markedPieces[5][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 7*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);											
					updateRound(globalRound, color, 5, 4);	
				}
				else if(IN(x, y, POSITION_X_INI + 2*POSITION_X_OFFSET, POSITION_Y_INI + 6*POSITION_Y_OFFSET) && !markedPieces[6][2]) { //(6,2)
					markedPieces[6][2] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 2*POSITION_X_OFFSET - POSITION_X_AJUST, 
												POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 6, 2);	
				}
				else if(IN(x, y, POSITION_X_INI + 4*POSITION_X_OFFSET, POSITION_Y_INI + 6*POSITION_Y_OFFSET) && !markedPieces[6][3]) { //(6,3)
					markedPieces[6][3] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 4*POSITION_X_OFFSET - POSITION_X_AJUST, 
												POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 6, 3);	
				}
				else if(IN(x, y, POSITION_X_INI + 6*POSITION_X_OFFSET, POSITION_Y_INI + 6*POSITION_Y_OFFSET) && !markedPieces[6][4]) { //(6,4)
					markedPieces[6][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 6*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);		
					updateRound(globalRound, color, 6, 4);	
				}
				else if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI + 6*POSITION_Y_OFFSET) && !markedPieces[6][5]) { //(6,5)
					markedPieces[6][5] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 6, 5);	
				}
				else if(IN(x, y, POSITION_X_INI + 3*POSITION_X_OFFSET, POSITION_Y_INI + 7*POSITION_Y_OFFSET) && !markedPieces[7][3]) { //(7,3)
					markedPieces[7][3] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 3*POSITION_X_OFFSET - POSITION_X_AJUST, 
												POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);			
					updateRound(globalRound, color, 7, 3);	
				}								
				else if(IN(x, y, POSITION_X_INI + 5*POSITION_X_OFFSET, POSITION_Y_INI + 7*POSITION_Y_OFFSET) && !markedPieces[7][4]) { //(7,4)
					markedPieces[7][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 5*POSITION_X_OFFSET - POSITION_X_AJUST, 
												POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 7, 4);	
				}
				else if(IN(x, y, POSITION_X_INI + 7*POSITION_X_OFFSET, POSITION_Y_INI + 7*POSITION_Y_OFFSET) && !markedPieces[7][5]) { //(7,5)
					markedPieces[7][5] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 7*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 7, 5);	
				}
				else if(IN(x, y, POSITION_X_INI + 4*POSITION_X_OFFSET, POSITION_Y_INI + 8*POSITION_Y_OFFSET) && !markedPieces[8][4]) { //(8,4)
					markedPieces[8][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 4*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 8, 4);	
				}
				else if(IN(x, y, POSITION_X_INI + 6*POSITION_X_OFFSET, POSITION_Y_INI + 8*POSITION_Y_OFFSET) && !markedPieces[8][5]) { //(8,5)
					markedPieces[8][5] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 6*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);	
					updateRound(globalRound, color, 8, 5);	
				}
				else if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI + 8*POSITION_Y_OFFSET) && !markedPieces[8][6]) { //(8,6)
					markedPieces[8][6] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);																			
					updateRound(globalRound, color, 8, 6);	
				}
			}
			if(BETWEEN(x, 256, 500)) { //right
				/* Pe�as deste quadrante
				(4,4) (4,5) (4,6) (4,7) (4,8)
				   (5,5) (5,6) (5,7) (5,8)
				(6,5) (6,6) (6,7) (6,8) 	
				   (7,6) (7,7) (7,8)
				(8,6) (8,7) (8,8)   
				*/
				if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][4]) { //(4,4)
					markedPieces[4][4] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 4);	
				}
				else if(IN(x, y, POSITION_X_INI + 10*POSITION_X_OFFSET, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][5]) { //(4,5)
					markedPieces[4][5] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 10*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 5);	
				}
				else if(IN(x, y, POSITION_X_INI + 12*POSITION_X_OFFSET, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][6]) { //(4,6)
					markedPieces[4][6] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 12*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 6);	
				}
				else if(IN(x, y, POSITION_X_INI + 14*POSITION_X_OFFSET, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][7]) { //(4,7)
					markedPieces[4][7] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 14*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 7);	
				}
				else if(IN(x, y, POSITION_X_INI + 16*POSITION_X_OFFSET, POSITION_Y_INI + 4*POSITION_Y_OFFSET) && !markedPieces[4][8]) { //(4,8)
					markedPieces[4][8] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 16*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 4, 8);	
				}
				else if(IN(x, y, POSITION_X_INI + 9*POSITION_X_OFFSET, POSITION_Y_INI + 5*POSITION_Y_OFFSET) && !markedPieces[5][5]) { //(5,5)
					markedPieces[5][5] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 9*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 5, 5);	
				}
				else if(IN(x, y, POSITION_X_INI + 11*POSITION_X_OFFSET, POSITION_Y_INI + 5*POSITION_Y_OFFSET) && !markedPieces[5][6]) { //(5,6)
					markedPieces[5][6] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 11*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 5, 6);	
				}
				else if(IN(x, y, POSITION_X_INI + 13*POSITION_X_OFFSET, POSITION_Y_INI + 5*POSITION_Y_OFFSET) && !markedPieces[5][7]) { //(5,7)
					markedPieces[5][7] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 13*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 5, 7);	
				}
				else if(IN(x, y, POSITION_X_INI + 15*POSITION_X_OFFSET, POSITION_Y_INI + 5*POSITION_Y_OFFSET) && !markedPieces[5][8]) { //(5,8)
					markedPieces[5][8] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 15*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);											
					updateRound(globalRound, color, 5, 8);	
				}
				else if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI + 6*POSITION_Y_OFFSET) && !markedPieces[6][5]) { //(6,5)
					markedPieces[6][5] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
												POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 6, 5);	
				}
				else if(IN(x, y, POSITION_X_INI + 10*POSITION_X_OFFSET, POSITION_Y_INI + 6*POSITION_Y_OFFSET) && !markedPieces[6][6]) { //(6,6)
					markedPieces[6][6] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 10*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 6, 6);	
				}
				else if(IN(x, y, POSITION_X_INI + 12*POSITION_X_OFFSET, POSITION_Y_INI + 6*POSITION_Y_OFFSET) && !markedPieces[6][7]) { //(6,7)
					markedPieces[6][7] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 12*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);		
					updateRound(globalRound, color, 6, 7);	
				}
				else if(IN(x, y, POSITION_X_INI + 14*POSITION_X_OFFSET, POSITION_Y_INI + 6*POSITION_Y_OFFSET) && !markedPieces[6][8]) { //(6,8)
					markedPieces[6][8] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 14*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 6, 8);
				}
				else if(IN(x, y, POSITION_X_INI + 9*POSITION_X_OFFSET, POSITION_Y_INI + 7*POSITION_Y_OFFSET) && !markedPieces[7][6]) { //(7,6)
					markedPieces[7][6] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 9*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);											
					updateRound(globalRound, color, 7, 6);
				}
				else if(IN(x, y, POSITION_X_INI + 11*POSITION_X_OFFSET, POSITION_Y_INI + 7*POSITION_Y_OFFSET) && !markedPieces[7][7]) { //(7,7)
					markedPieces[7][7] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 11*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 7, 7);
				}
				else if(IN(x, y, POSITION_X_INI + 13*POSITION_X_OFFSET, POSITION_Y_INI + 7*POSITION_Y_OFFSET) && !markedPieces[7][8]) { //(7,8)
					markedPieces[7][8] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 13*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 7, 8);
				}
				else if(IN(x, y, POSITION_X_INI + 8*POSITION_X_OFFSET, POSITION_Y_INI + 8*POSITION_Y_OFFSET) && !markedPieces[8][6]) { //(8,6)
					markedPieces[8][6] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 8*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
					updateRound(globalRound, color, 8, 6);
				}
				else if(IN(x, y, POSITION_X_INI + 10*POSITION_X_OFFSET, POSITION_Y_INI + 8*POSITION_Y_OFFSET) && !markedPieces[8][7]) { //(8,7)
					markedPieces[8][7] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 10*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);	
					updateRound(globalRound, color, 8, 7);
				}
				else if(IN(x, y, POSITION_X_INI + 12*POSITION_X_OFFSET, POSITION_Y_INI + 8*POSITION_Y_OFFSET) && !markedPieces[8][8]) { //(8,8)
					markedPieces[8][8] = 1;
					piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 12*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);		
					updateRound(globalRound, color, 8, 8);
				}
			}
		}
	}	

	finalCount = countZeros(markedPieces);
	if(finalCount < iniCount) {
		checkFinalGame(finalCount);		
		changePlayer();
		activeMinimaxForPlayerPC();
	}

	glutPostRedisplay();
}


/*
Descri��o:
	Testa se o jogo chegou ao fim.		
Vari�veis Globais:
	@gameState: indica se o jogo chegou ao fim ou n�o.	
*/
void checkFinalGame(int finalCount) {
	int gameResult;
	char playerIA[10] = {"IA"};
	char playerHuman[10] = {"Humano"};

	gameResult = evalFunction(globalRound->board, globalRound->playerWhite.markedPieces, WHITE);
	if(gameResult == MAX_WIN && globalRound->playerWhite.player == PLAYER_PC) {
		printf("%s ganhou.\n", playerIA);
		gameState = GAME_END;
	}
	else if(gameResult == MAX_LOSS && globalRound->playerWhite.player == PLAYER_PC) {
		printf("%s ganhou.\n", playerHuman);
		gameState = GAME_END;
	}
	else if(gameResult == MAX_WIN && globalRound->playerWhite.player == PLAYER_HUMAN) {
		printf("%s ganhou.\n", playerHuman);
		gameState = GAME_END;
	}
	else if(gameResult == MAX_LOSS && globalRound->playerWhite.player == PLAYER_HUMAN) {
		printf("%s ganhou.\n", playerIA);
		gameState = GAME_END;
	}

	gameResult = evalFunction(globalRound->board, globalRound->playerBlack.markedPieces, BLACK);
	if(gameResult == MAX_WIN && globalRound->playerBlack.player == PLAYER_PC) {
		printf("%s ganhou.\n", playerIA);
		gameState = GAME_END;
	}
	else if(gameResult == MAX_LOSS && globalRound->playerBlack.player == PLAYER_PC) {
		printf("%s ganhou.\n", playerHuman);
		gameState = GAME_END;
	}
	else if(gameResult == MAX_WIN && globalRound->playerBlack.player == PLAYER_HUMAN) {
		printf("%s ganhou.\n", playerHuman);
		gameState = GAME_END;
	}
	else if(gameResult == MAX_LOSS && globalRound->playerBlack.player == PLAYER_HUMAN) {
		printf("%s ganhou.\n", playerIA);
		gameState = GAME_END;
	}
}


/*
Descri��o:
	Marca a jogada da IA no tabuleiro, dados os �ndices da posi��o.
Par�metros:
	@x, y: �ndices da posi��o na matriz do tabuleiro.
	@color: cor da pe�a da IA.
Vari�veis Globais:
	@markedPieces: matriz que indica quais as posi��es do tabuleiro est�o marcadas.
	@globalRound: estrutura da rodada do jogo
	@currentPlayer: jogador atual
	@piecesPlayer: matriz com uma lista de pares de coordenadas para desenhar as pe�as
	de cada jogador.
*/
void markIAMove(int x, int y, int color) {
	int finalCount;

	markedPieces[x][y] = 1;
	
	if(x == 0) {
		if(y == 0) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 4*POSITION_X_OFFSET, 
											POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 0, 0);
		}
		else if(y == 1) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 6*POSITION_X_OFFSET, 
											POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 0, 1);
		}
		else if(y == 2) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 8*POSITION_X_OFFSET, 
											POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 0, 2);
		}
		else if(y == 3) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 10*POSITION_X_OFFSET - POSITION_X_AJUST, 
											POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 0, 3);
		}
		else if(y == 4) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 12*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 0, 4);		
		}
	}
	else if(x == 1) {
		if(y == 0) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 3*POSITION_X_OFFSET, 
											POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 1, 0);
		}
		else if(y == 1) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 5*POSITION_X_OFFSET, 
											POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 1, 1);
		}
		else if(y == 2) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 7*POSITION_X_OFFSET, 
											POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 1, 2);
		}		
		else if(y == 3) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 9*POSITION_X_OFFSET, 
											POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 1, 3);
		}
		else if(y == 4) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 11*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 1, 4);
		}
		else if(y == 5) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 13*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
											POSITION_Y_INI + POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);	
			updateRound(globalRound, color, 1, 5);
		}
	}
	else if(x == 2) {
		if(y == 0) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 2*POSITION_X_OFFSET, 
											POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 2, 0);	
		}
		else if(y == 1) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 4*POSITION_X_OFFSET, 
											POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 2, 1);	
		}		
		else if(y == 2) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 6*POSITION_X_OFFSET, 
											POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 2, 2);	
		}	
		else if(y == 3) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 8*POSITION_X_OFFSET, 
											POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 2, 3);	
		}			
		else if(y == 4) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 10*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 2, 4);
		}
		else if(y == 5) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 12*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
											POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 2, 5);
		}
		else if(y == 6) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 14*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 2*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);	
			updateRound(globalRound, color, 2, 6);
		}			
	}
	else if(x == 3) {
		if(y == 0) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + POSITION_X_OFFSET, 
											POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 3, 0);
		}
		else if(y == 1) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 3*POSITION_X_OFFSET, 
											POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 3, 1);	
		}
		else if(y == 2) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 5*POSITION_X_OFFSET, 
											POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);	
			updateRound(globalRound, color, 3, 2);
		}
		else if(y == 3) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 7*POSITION_X_OFFSET, 
											POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);														
			updateRound(globalRound, color, 3, 3);
		}
		else if(y == 4) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 9*POSITION_X_OFFSET - POSITION_X_AJUST, 
											POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 3, 4);
		}
		else if(y == 5) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 11*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 3, 5);
		}
		else if(y == 6) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 13*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
											POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 3, 6);
		}
		else if(y == 7) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 15*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 3*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);	
			updateRound(globalRound, color, 3, 7);	
		}		
	}
	else if(x == 4) {
		if(y == 0) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI, 
											POSITION_Y_INI + 4*POSITION_Y_OFFSET), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 4, 0);	
		}
		else if(y == 1) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 2*POSITION_X_OFFSET, 
											POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 4, 1);	
		}
		else if(y == 2) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 4*POSITION_X_OFFSET, 
											POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 4, 2);
		}
		else if(y == 3) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 6*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 4, 3);	
		}
		else if(y == 4) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 8*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 4, 4);	
		}
		else if(y == 5) {
				piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 10*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
											POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
				updateRound(globalRound, color, 4, 5);	
		}
		else if(y == 6) {
				piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 12*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
												piecesPlayer[currentPlayer]);
				updateRound(globalRound, color, 4, 6);	
		}
		else if(y == 7) {
				piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 14*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
												piecesPlayer[currentPlayer]);
				updateRound(globalRound, color, 4, 7);	
		}
		else if(y == 8) {
				piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
												POSITION_X_INI + 16*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
												POSITION_Y_INI + 4*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
												piecesPlayer[currentPlayer]);
				updateRound(globalRound, color, 4, 8);	
		}		
	}
	else if(x == 5) {
		if(y == 1) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + POSITION_X_OFFSET, 
											POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 5, 1);
		}
		else if(y == 2) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 3*POSITION_X_OFFSET, 
											POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 5, 2);	
		}
		else if(y == 3) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 5*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 5, 3);	
		}
		else if(y == 4) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 7*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);											
			updateRound(globalRound, color, 5, 4);	
		}
		else if(y == 5) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 9*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
											POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 5, 5);	
		}
		else if(y == 6) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 11*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
											POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 5, 6);	
		}
		else if(y == 7) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 13*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 5, 7);	
		}
		else if(y == 8) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 15*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 5*POSITION_Y_OFFSET - POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);											
			updateRound(globalRound, color, 5, 8);	
		}		
	}
	else if(x == 6) {
		if(y == 2) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 2*POSITION_X_OFFSET - POSITION_X_AJUST, 
											POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 6, 2);	
		}
		else if(y == 3) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 4*POSITION_X_OFFSET - POSITION_X_AJUST, 
											POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 6, 3);	
		}
		else if(y == 4) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 6*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);		
			updateRound(globalRound, color, 6, 4);	
		}
		else if(y == 5) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 8*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 6, 5);	
		}
		else if(y == 6) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 10*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
											POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 6, 6);	
		}
		else if(y == 7) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 12*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);		
			updateRound(globalRound, color, 6, 7);	
		}
		else if(y == 8) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 14*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 6*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 6, 8);
		}		
	}
	else if(x == 7) {
		if(y == 3) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 3*POSITION_X_OFFSET - POSITION_X_AJUST, 
											POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);			
			updateRound(globalRound, color, 7, 3);	
		}
		else if(y == 4) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 5*POSITION_X_OFFSET - POSITION_X_AJUST, 
											POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 7, 4);	
		}
		else if(y == 5) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 7*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 7, 5);	
		}
		else if(y == 6) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 9*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
											POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);											
			updateRound(globalRound, color, 7, 6);
		}
		else if(y == 7) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 11*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 7, 7);
		}
		else if(y == 8) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 13*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 7*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 7, 8);
		}
	}
	else if(x == 8)	{
		if(y == 4) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 4*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);
			updateRound(globalRound, color, 8, 4);		
		}
		else if(y == 5) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 6*POSITION_X_OFFSET - 2*POSITION_X_AJUST, 
											POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);	
			updateRound(globalRound, color, 8, 5);
		}
		else if(y == 6) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 8*POSITION_X_OFFSET - 3*POSITION_X_AJUST, 
											POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);																			
			updateRound(globalRound, color, 8, 6);	
		}
		else if(y == 7) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 10*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);	
			updateRound(globalRound, color, 8, 7);
		}
		else if(y == 8) {
			piecesPlayer[currentPlayer] = insertPar(ajustPositionCoordinates(
											POSITION_X_INI + 12*POSITION_X_OFFSET - 4*POSITION_X_AJUST, 
											POSITION_Y_INI + 8*POSITION_Y_OFFSET - 2*POSITION_Y_AJUST), 
											piecesPlayer[currentPlayer]);		
			updateRound(globalRound, color, 8, 8);
		}
	}
	
	finalCount = countZeros(markedPieces);
	checkFinalGame(finalCount);
	glutPostRedisplay();	
	
	changePlayer();
}


/*
Descri��o:
	Prepara o programa para calcular a jogada da IA. E posteriormente
	marca a jogada da IA no tabuleiro.
Vari�veis Globais:
	@x, y: coordenadas para a jogada da IA
*/
void activeMinimaxForPlayerPC() {
	int depth = 2;	
	int color;
		
	//Prepara argumentos para a fun��o do minimax
	if(globalRound->playerWhite.player == PLAYER_PC) {
		color = WHITE;
	}
	else {
		color = BLACK;
	}
	
	if(countZeros(markedPieces) == 81 || countZeros(markedPieces) == 80)
		firstMove(globalRound);
	else {
		//printf("Calculando jogada da IA.\n");
		seedMinimax(globalRound, depth, color, &x, &y);
		printf("Minimax: IA deve jogar em (%d,%d).\n", x, y);
	}

	markIAMove(x, y, color);
}


/*
Descri��o:
	Conta quantas posi��es n�o marcadas existem na matriz
Par�metros:
	@markedPieces: matriz com as posi��es do tabuleiro. Indica se uma
	determinada posi��o est� ou n�o marcada.
Retorno:
	O n�mero de posi��es n�o marcadas. (vazias)
*/
int countZeros(int markedPieces[BOARD_ROW][BOARD_COLUMN]) {
	int i, j, count = 0;

	for(i = 0; i < BOARD_ROW; i++)
		for(j = 0; j < BOARD_COLUMN; j++)
			if(markedPieces[i][j] == 0)
				count++;

	return count;
}









