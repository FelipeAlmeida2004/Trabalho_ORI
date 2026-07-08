// Grupo - 9
// Integrantes:
// Felipe Gabriel Senapeschi de Almeida - 817412
// João Luís Ramos Defendi Vignoli - 832962
// Mairon Cesar de Paula Faria - 832103
// Matheus Borghes Garcia Leal - 833036

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_AUTOR 350003    
#define TAM_PAPER 700001 

// Estruturas Básicas
typedef struct Autor {
    int id;
    char nome[100];
} Autor;

typedef struct Paper {
    int id;
    char titulo[150];
    int *autores;
} Paper;

// Estruturas das Tabelas Hash
typedef struct HashAutores {
    int TABLE_SIZE, qtd;
    struct AutorNode** autores;
} HashAutores;

typedef struct HashArtigos {
    int TABLE_SIZE;
    int qtd;
    struct PaperNode** artigos;
} HashArtigos;

// Vértice do grafo (Pesquisador)
typedef struct AutorNode {
    int id;
    char nome[100];
    struct AutorNode* prox; 
    struct Aresta* arestas; // Lista de conexões
} AutorNode;

// Lista de autores de um mesmo artigo
typedef struct AutorRef {
    AutorNode* autor;           
    struct AutorRef* prox;
} AutorRef;

// Nó da lista encadeada de artigos (Hash de Artigos)
typedef struct PaperNode {
    char titulo[150];
    struct PaperNode* prox;
    struct AutorRef* autores; // Autores vinculados a este título   
} PaperNode;

// Aresta do grafo (Conexão entre dois autores através de artigos)
typedef struct Aresta {
    struct AutorNode* destino;
    PaperNode* papers; // Artigos publicados em conjunto
    struct Aresta* prox;    
} Aresta;

// Variáveis Globais
AutorNode* hashAutores[TAM_AUTOR]; 
PaperNode* hashTitulos[TAM_PAPER];
AutorNode* indicePesquisadores[TAM_AUTOR]; 
int totalPesquisadores = 0;

// Protótipos das funções
HashAutores* criaHash(int TABLE_SIZE);
HashArtigos* criaHashArtigos(int TABLE_SIZE);
AutorNode* buscarAutor(const char* nome, HashAutores* tabela_autores );
void listarColaboradores(const char *nome, HashAutores* tabela_autores);
int contaGrau(AutorNode* autor);
int maiorGrau(HashAutores* tabela_autores);
float grauMedio(HashAutores* tabela_autores);
int chaveDivisao(int chave, int TABLE_SIZE);
int valorString(char *str);
AutorNode* insereHash_EncadeamentoSeparado(HashAutores* tabela_autores, struct AutorNode autor);
void adicionarAresta(AutorNode* origem, AutorNode* destino, char* titulo);
void construirGrafo(HashArtigos* tabela_artigos);
int inserirArtigo(HashArtigos* tabela_artigos, char* titulo, AutorNode* autorAtual);

int main(){
    FILE *arquivo_pesq = fopen("dadosPesquisadores.txt", "r");

    if (arquivo_pesq == NULL) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    HashAutores *tabela_autores = criaHash(TAM_AUTOR);
    HashArtigos *tabela_artigos = criaHashArtigos(TAM_PAPER);

    char linha[512];
    char autor[100];
    char titulo[150];

    // Lógica principal: Lê o arquivo e popula as tabelas Hash de Autores e Artigos
    while (fgets(linha, sizeof(linha), arquivo_pesq)) {

        if (sscanf(linha, "%99[^\t]\t%149[^\n]", autor, titulo) != 2)
            continue;

        AutorNode novoAutor;
        strcpy(novoAutor.nome, autor);
        novoAutor.arestas = NULL;
        novoAutor.prox = NULL;

        // Insere nas tabelas de forma que evite duplicatas
        AutorNode *autorAtual = insereHash_EncadeamentoSeparado(tabela_autores, novoAutor);
        inserirArtigo(tabela_artigos, titulo, autorAtual);
    }

    fclose(arquivo_pesq);

    // Usa a Hash de Artigos para ligar os colaborador no grafo
    construirGrafo(tabela_artigos);

    printf("=====================================\n");
    printf("Total de pesquisadores: %d\n", totalPesquisadores);
    printf("Maior grau: %d\n", maiorGrau(tabela_autores));
    printf("Grau medio: %.2f\n", grauMedio(tabela_autores));
    printf("=====================================\n");

    char nome[100];
    printf("\nDigite o nome de um pesquisador: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';

    printf("\nColaboradores de %s:\n", nome);
    listarColaboradores(nome, tabela_autores);

    return 0;
}

// Inicializa Hash Autores
HashAutores* criaHash(int TABLE_SIZE) {
    HashAutores* tabela_autores = (HashAutores*) malloc(sizeof(HashAutores));
    if(tabela_autores != NULL) {
        int i;
        tabela_autores->TABLE_SIZE = TABLE_SIZE;
        tabela_autores->autores = (struct AutorNode**)
            malloc(TABLE_SIZE * sizeof(tabela_autores->autores));
        if(tabela_autores->autores == NULL) {
            free(tabela_autores);
            return NULL;
        }
        
        tabela_autores->qtd = 0;
        for(i = 0; i < tabela_autores->TABLE_SIZE; i++)
            tabela_autores->autores[i] = NULL;
    }
    return tabela_autores;
}

int chaveDivisao(int chave, int TABLE_SIZE){
    return (chave & 0x7FFFFFFF) % TABLE_SIZE;
}

int valorString(char *str) {
    int i, valor = 7;
    int tam = strlen(str);
    for(i = 0; i < tam; i++)
        valor = 31 * valor + (int) str[i];
    return valor;
}

// Insere autor na Hash e retorna o ponteiro do nó se o autor já existir
AutorNode* insereHash_EncadeamentoSeparado(HashAutores* tabela_autores, struct AutorNode autor) {
    if(tabela_autores == NULL || tabela_autores->qtd == tabela_autores->TABLE_SIZE) {
        return 0;
    }

    int chave_int = valorString(autor.nome); 
    int pos = chaveDivisao(chave_int, tabela_autores->TABLE_SIZE);

    AutorNode* atual = tabela_autores->autores[pos];
    while (atual != NULL) {
        if (strcmp(atual->nome, autor.nome) == 0) {
            return atual; 
        }
        atual = atual->prox;
    }

    struct AutorNode* novo = (struct AutorNode*) malloc(sizeof(struct AutorNode));
    if(novo == NULL) return 0;

    strcpy(novo->nome, autor.nome);
    novo->arestas = NULL;
    novo->id = totalPesquisadores; 
    
    indicePesquisadores[totalPesquisadores] = novo;
    totalPesquisadores++; 

    // Encadeamento separado (insere no início da lista da posição Hash)
    novo->prox = tabela_autores->autores[pos];
    tabela_autores->autores[pos] = novo;
    tabela_autores->qtd++;
    
    return novo;
}

AutorNode* buscarAutor(const char* nome, HashAutores* tabela_autores){    
    for(int i = 0; i < tabela_autores->TABLE_SIZE; i++){
        AutorNode *atual = tabela_autores->autores[i];
        while(atual != NULL){
            if(strcmp(atual->nome, nome) == 0){
                return atual;
            }
            atual = atual->prox;
        }
    }
    return NULL;
}

void listarColaboradores(const char* nome, HashAutores* tabela_autores) {
    AutorNode* autor = buscarAutor(nome, tabela_autores);
    if(autor == NULL){
        printf("Autor não encontrado!");
        return;
    }
    
    // Varre as arestas (colaboradores) ligados ao autor pesquisado
    for(Aresta *aresta = autor->arestas; aresta != NULL; aresta = aresta->prox){ 
        if(aresta->destino != NULL){
            printf("- %s\n", aresta->destino->nome);
        }
    }
}

// O Grau é o número de colaboradores diretos (tamanho da lista de arestas)
int contaGrau(AutorNode* autor){
    int grau = 0;
    Aresta* aresta = autor->arestas;

    while (aresta != NULL){
        grau++;
        aresta = aresta->prox;
    }
    return grau;
}

// Procura qual autor (vértice) possui o maior grau na rede
int maiorGrau(HashAutores* tabela_autores){
    int maior = 0;
    AutorNode* autor = NULL;

    for(int i = 0; i < tabela_autores->TABLE_SIZE; i++){
        AutorNode* atual = tabela_autores->autores[i];

        while(atual != NULL){
            int grau = contaGrau(atual);
            if(grau > maior){
                maior = grau;
                autor = atual;
            }
            atual = atual->prox;
        }
    }

    if (autor != NULL) {
        printf("Maior grau: %d (autor: %s)\n", maior, autor->nome);
    }
    return maior;
}

// Média de conexões por pesquisador
float grauMedio(HashAutores* tabela_autores){
    long soma = 0;
    long totalAutores = 0;

    for(int i = 0; i < tabela_autores->TABLE_SIZE; i++){
        AutorNode* atual = tabela_autores->autores[i];

        while(atual != NULL){
            soma += contaGrau(atual);
            totalAutores++;
            atual = atual->prox;
        }
    }

    if (totalAutores == 0) return 0.0f;
    return (float) soma / totalAutores;
}

// Inicializa Hash Artigos
HashArtigos* criaHashArtigos(int TABLE_SIZE) {
    HashArtigos* tabela_artigos = (HashArtigos*) malloc(sizeof(HashArtigos));
    if(tabela_artigos != NULL) {
        int i;
        tabela_artigos->TABLE_SIZE = TABLE_SIZE;
        tabela_artigos->artigos = (struct PaperNode**)
            malloc(TABLE_SIZE * sizeof(tabela_artigos->artigos));
        if(tabela_artigos->artigos == NULL) {
            free(tabela_artigos);
            return NULL;
        }
        
        tabela_artigos->qtd = 0;
        for(i = 0; i < tabela_artigos->TABLE_SIZE; i++)
            tabela_artigos->artigos[i] = NULL;
    }
    return tabela_artigos;
}

// Associa o autor ao artigo. Se o artigo já existir, junta o autor na lista de colaboradores daquele título
int inserirArtigo(HashArtigos* tabela_artigos, char* titulo, AutorNode* autorAtual) {
    if (tabela_artigos == NULL || titulo == NULL || autorAtual == NULL) return 0;

    int chave_int = valorString(titulo);
    int pos = chaveDivisao(chave_int, tabela_artigos->TABLE_SIZE);
    PaperNode* atual = tabela_artigos->artigos[pos];

    // Verifica se o artigo já existe para agrupar coautores
    while (atual != NULL) {
        if (strcmp(atual->titulo, titulo) == 0) {
            AutorRef* novaRef = (AutorRef*) malloc(sizeof(AutorRef));
            if (novaRef == NULL) return 0;
            novaRef->autor = autorAtual;
            novaRef->prox = atual->autores;
            atual->autores = novaRef;
            return 1;
        }
        atual = atual->prox;
    }

    // Se é um artigo novo, cria a sua respectiva estrutura
    PaperNode* novoArtigo = (PaperNode*) malloc(sizeof(PaperNode));
    if (novoArtigo == NULL) return 0;

    strcpy(novoArtigo->titulo, titulo);
    AutorRef* primeiraRef = (AutorRef*) malloc(sizeof(AutorRef));
    if (primeiraRef == NULL) {
        free(novoArtigo);
        return 0;
    }
    
    primeiraRef->autor = autorAtual;
    primeiraRef->prox = NULL;
    novoArtigo->autores = primeiraRef;
    novoArtigo->prox = tabela_artigos->artigos[pos];
    tabela_artigos->artigos[pos] = novoArtigo;
    tabela_artigos->qtd++;

    return 1;
}

// Cria a ligação de um vértice de Origem para um de Destino e vincula a causa (Título do Artigo)
void adicionarAresta(AutorNode* origem, AutorNode* destino, char* titulo) {
    Aresta* atual = origem->arestas;
    
    // Se a aresta entre os dois já existir, apenas insere mais um artigo na ligação
    while (atual != NULL) {
        if (atual->destino == destino) {
            PaperNode* novoTitulo = (PaperNode*) malloc(sizeof(PaperNode));
            strcpy(novoTitulo->titulo, titulo);
            novoTitulo->autores = NULL; 
            novoTitulo->prox = atual->papers;
            atual->papers = novoTitulo;
            return;
        }
        atual = atual->prox;
    }

    // Se nunca colaboraram antes, cria a primeira aresta
    Aresta* novaAresta = (Aresta*) malloc(sizeof(Aresta));
    novaAresta->destino = destino;

    PaperNode* novoTitulo = (PaperNode*) malloc(sizeof(PaperNode));
    strcpy(novoTitulo->titulo, titulo);
    novoTitulo->autores = NULL;
    novoTitulo->prox = NULL;

    novaAresta->papers = novoTitulo;
    novaAresta->prox = origem->arestas;
    origem->arestas = novaAresta;
}

// Percorre os agrupamentos na Hash de Artigos para formar arestas (cliques) no grafo
void construirGrafo(HashArtigos* tabela_artigos) {
    if (tabela_artigos == NULL) return;

    for (int i = 0; i < tabela_artigos->TABLE_SIZE; i++) {
        PaperNode* artigoAtual = tabela_artigos->artigos[i];

        while (artigoAtual != NULL) {
            
            // Se houver mais de um autor no mesmo artigo, cria ligações entre todos eles
            if (artigoAtual->autores != NULL && artigoAtual->autores->prox != NULL) {
                AutorRef* autor1 = artigoAtual->autores;
                
                while (autor1 != NULL) {
                    AutorRef* autor2 = autor1->prox;
                    
                    while (autor2 != NULL) {  
                        // Grafo não-direcionado: cria ida e volta                      
                        adicionarAresta(autor1->autor, autor2->autor, artigoAtual->titulo);
                        adicionarAresta(autor2->autor, autor1->autor, artigoAtual->titulo);
                        autor2 = autor2->prox;
                    }
                    autor1 = autor1->prox;
                }
            }
            artigoAtual = artigoAtual->prox;
        }
    }
}