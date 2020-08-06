struct TipoFila
{
       int info;
       TipoFila* prox;
};

//Retorna NULL
TipoFila* cria_fila(void);

//Insere um elemento em uma LSE
TipoFila* insere (TipoFila *l, int dado);

//Consulta se o nodo est� na LSE. Se estiver retorna 1, caso contr�rio retorna 0
int consulta (TipoFila *l, int dado);

//Caminhamento por Abrang�ncia
void cam_abrangencia (TipoFila *l, int dado, int mat[8][8]);

//Caminhamento por Profundidade
void cam_profundidade (TipoFila *l, int dado, int mat[8][8]); 

int vazia (TipoFila* l);
int remove (TipoFila **l);
void imprime(TipoFila* l);


