
class Memory{
  
	//Sistema de mem�ria.
	
	//Possuia a MMU (memory management unit) 
	//com seus registradores e m�todos de acesso.
	private int[] word;
	private boolean[] partitions;
	private static final int SZ = 1024;
	
	public Memory(){
		//8 parti��es
	    partitions = new boolean[]{true, true, true, true, true, true, true, true};
	    
	    word = new int[SZ];
    }
	
	//jump0address, (int)'J', (int)'P', (int)'A', zero)
	public synchronized void init(int add, int a, int b, int c, int d){
		
		//faz shift para a esquerda dos bits de a em 24 posi��es
		//faz shift para a esquerda dos bits de b em 16 posi��es
		//faz shift para a esquerda dos bits de c em 8 posi��es 
		word[add] = (a << 24) + (b << 16) + (c << 8) + d;
    }
	
	//devolve palavra lida da mem�ria - adress � endere�o f�sico
	public synchronized int read(int address){
		return word[address];
    }

	//escreve informa��o na mem�ria - adree � endere�o f�sico
	public synchronized void write(int address, int data){
		word[address] = data;
    }
	
	//Dado um endere�o l�gico, a base da parti��o e o limite da parti��o, retorna o endere�o f�sico
	public synchronized int getPhysAddr(int logicalAddress, int base, int limit){
		int add = logicalAddress + base;
		if(add < base || add >= limit){
			return -1;
		}else {
			return add;
		}
	}
	
	//Libera uma parti��o
	public void freePartition(int base){
		partitions[base/128] = true;
	}
	
	//Devolve a base d parti��o dispon�vel
	public synchronized int getPartitionBase(){
		for(int i = 0; i < 8; i++){
			if(partitions[i]){
				partitions[i] = false;
				return i*128;
			}
		}
		
		System.err.printf("Memory: No more partitions\n");
		return -1;
	}
}