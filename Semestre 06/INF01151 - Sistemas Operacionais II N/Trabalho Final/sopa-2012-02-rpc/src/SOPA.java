// Sistema Operacional Para Avaliacao - SOPA

import java.awt.event.*;

public class SOPA{
	public static void main(String args[]){
	    
	    IntCtrl i = new IntCtrl();
	    
	    //Cria janela do console
	    Console cw = new Console();
	    cw.addWindowListener(
		    new WindowAdapter(){
				public void windowClosing (WindowEvent e){
					System.exit(0);
				}
			});
	    
	    cw.setInterruptionController(i);
	    
	    //Cria objetos
	    
	    //Mem�ria criada e inicializada
	    Memory m = new Memory();
	    
	    //Disco criado, inicializado e indexado como 0. Disco cria sem�foro
	    //inicialmente em 0. Arquivo de texto do disco � lido e armazenado em 
	    //uma estrutura.
	    Disk d = new Disk(i, m);
	    
	    //Kernel � criado. Reconhece o controlador de interrup��es, mem�ria,
	    //console e disco
	    Kernel k = new Kernel(i, m, cw, d);
	    
	    //Timer criado e inicializado com contagem em zero. Se mant�m parado. 
	    Timer t = new Timer();
	    
	    //Processador criado. Kernel reconhece sua exist�ncia e ja cria um processo
	    //dummy para rodar neste processador, caso n�o tenha outros processos.
	    Processor p = new Processor(i, m, t, k);
	       
	    //Inicia todas as threads
	    
	    p.start(); //fica rodando at� que ocorra uma interrup��o 
	    t.start(); //come�a contagem de tempo
	    d.start(); //
    }
}






