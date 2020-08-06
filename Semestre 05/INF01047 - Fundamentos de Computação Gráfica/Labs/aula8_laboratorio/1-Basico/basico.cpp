/* 
Cadeira de FCG
Programa: Basico.cpp
Atualizado em 2007

a) Estude o programa Basico.cpp e tente entender sua organiza��o. Observe com 
aten��o os seguintes aspectos:
Linguagem C: defini��o e chamada de fun��es
OpenGL: include  e chamadas de fun��es gl<..>
Estrutura b�sica:
    - fun��o principal, onde a comunica��o com a OpenGL � estabelecida e s�o 
    disparadas as inicializa��es necess�rias;
    - fun��o �RenderScene�, onde ser�o inclu�das as fun��es de desenho;
    - fun��o de inicializa��o �SetupRC�, para conter as inicializa��es relativas 
    � exibi��o.
    
b) Altere a cor de fundo da janela para verde, modificando os argumentos da 
chamada da fun��o
   
glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

Os valores especificados s�o valores de RGBA (Red-Green-Blue-Alpha) que variam 
de 0 a 1, do tipo float. N�o se preocupe com o Alpha, pois ele ser� usado para 
efeitos de transpar�ncia e combina��o de imagens, posteriormente.
*/

#include <GL/glut.h>
/* 
include de definicoes das funcoes da glut 
glut.h inclui gl.h, que contem os headers de funcoes da OpenGL propriamente dita
glut.h inclui tambem definicoes necessarias para o uso de OpenGl nos diversos 
ambientes Windows
*/


/* 
Fun��o de callback de desenho
Executada sempre que eh necessario re-exibir a imagem
*/
void RenderScene(void){
	glClear(GL_COLOR_BUFFER_BIT); // Limpa a janela com a cor especificada como cor de fundo    
    // funcoes OpenGL para desenho
    glFlush(); // For�a a execu��o das instru��es do pipeline OpenGL, para desenhar a imagem
}


/* Inicializa aspectos do rendering */
void SetupRC(void){
    //glClearColor(0.0f, 1.0f, 0.0f, 1.0f);  // Cor de fundo da janela - verde
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);  // Cor de fundo da janela - azul
    //glClearColor(1.0f, 0.0f, 0.0f, 1.0f);  // Cor de fundo da janela - vermelho
}

/* 
Parte principal - ponto de in�cio de execu��o
Cria janela 
Inicializa aspectos relacionados a janela e a geracao da imagem
Especifica a funcao de callback de desenho
*/
int main(int argc, char **argv){
    	glutInit(&argc, argv);	

    // Indica que deve ser usado um unico buffer para armazenamento da imagem e representacao de cores RGB
    	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
    // Cria uma janela com o titulo especificado
	glutCreateWindow("Programa Basico OpenGL");

    // Especifica para a OpenGL que funcao deve ser chamada para geracao da imagem
	glutDisplayFunc(RenderScene);

    // Executa a inicializacao de parametros de exibicao
	SetupRC();

    // Dispara a &quot;maquina de estados&quot; de OpenGL 
	glutMainLoop();
	
	return 0;
}

