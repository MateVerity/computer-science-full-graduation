    
    #define Vertex int /* V�rtices s�o representados por objetos do tipo Vertex.  Os v�rtices de um digrafo s�o 0,1,..,V-1. */
    #define Edge Arc /* Um objeto do tipo Edge representa uma aresta com pontas v e w. */
    #define EDGE ARC /* A fun��o EDGE recebe dois v�rtices v e w e devolve uma aresta com pontas v e w. */
    
	/* Essa mesma estrutura ser� usada para representar grafos, mas nesse caso escreveremos "graph" e "Graph" no lugar de "digraph" e "Digraph" */
    #define graph digraph
    #define Graph Digraph
	
	#define maxCST 2
    
    /* Um objeto do tipo Arc representa um arco com ponta inicial v e ponta final w. */
    typedef struct { 
       Vertex v; 
       Vertex w; 
    } Arc;

    /* A fun��o ARC recebe dois v�rtices v e w e devolve um arco com ponta inicial v e ponta final w. */
    Arc ARC (Vertex v, Vertex w) {
       Arc e;
       e.v = v;
       e.w = w;
       return e;
    }

    /* A estrutura digraph representa um digrafo. O campo adj � um ponteiro para o vetor de listas de adjac�ncia do digrafo, o campo V cont�m o n�mero de v�rtices e o campo A cont�m o n�mero de arcos do digrafo. */
    struct digraph {
       int V; 
       int A; 
       link *adj; 
    };

    /* Um objeto do tipo Digraph cont�m o endere�o de um digraph. */
    typedef struct digraph *Digraph;

    /* A lista de adjac�ncia de um v�rtice v � composta por n�s do tipo node. Um link � um ponteiro para um node. Cada n� da lista cont�m um vizinho w de v e o endere�o do n� seguinte da lista. */
    typedef struct node *link;
    struct node { 
       Vertex w; 
       link next; 
    };

    /* A fun��o NEW recebe um v�rtice w e o endere�o next de um n� e devolve um novo n� x com x.w = w e x.next = next. */
    link NEW (Vertex w, link next) { 
       link x = malloc(sizeof *x);
       x->w = w; 
       x->next = next;     
       return x;                         
    }

	
    void bruteforceKruskal (int *mat)
    { 
       Vertex id[maxV], v, w;
       for (v = 0; v < G->V; v++) id[v] = v;
       while (1) {
          double mincst = maxCST;
          Vertex v0, w0;
          for (v = 0; v < G->V; v++) 
             for (w = 0; w < G->V; w++) 
                if (G->adj[v][w] < mincst && id[v] != id[w]) 
                   mincst = G->adj[v0=v][w0=w];
          if (mincst == maxCST) return; 
          printf("%d-%d\n", v0, w0);
          for (v = 0; v < G->V; v++) 
             if (id[v] == id[w0])
                id[v] = id[v0];
       }
    }

    	
	
	