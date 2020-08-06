/* TAD: Dados do nodo da pilha */
struct Tipo_Info
{
//       int cod;
//       char nome[40];
       float preco;    
};

typedef struct Tipo_Info TipoInfo;

/* TAD: Nodo da pilha */
struct Tipo_Pilha
{
       TipoInfo dado;
       struct Tipo_Pilha *elo;
};

typedef struct Tipo_Pilha TipoPilha;


/* Fun��es Exportadas */

/* Cria pilha vazia - Fun��o de inicializa��o da pilha */
TipoPilha* InicializaPilha (TipoPilha* Topo, int *cont);

/* Insere nodo no topo da pilha */
TipoPilha* InsereNodo (TipoPilha* Topo, TipoInfo Dado, int *cont);

/* Remove nodo do topo da lista */
int RemoveNodo (TipoPilha* Topo, int *cont);

/* Consulta nodo do topo da lista */
float ConsultaNodo (TipoPilha* Topo);

/* Destr�i pilha */
TipoPilha* DestroiPilha (TipoPilha* Topo, int *cont);

/* Imprime pilha */
void ImprimePilha (TipoPilha* Topo, int cont);
