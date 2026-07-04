
// Grupo - 9
// Integrantes:
// Felipe Gabriel Senapeschi de Almeida - 817412
// João luis - 
// Mairon Cesar - 
// Matheus Borghes - 


#include <stdio.h>
#include <stdlib.h>

#define TAM 211 

typedef struct Autor {
    int id;
    char nome[100];
} Autor;

typedef struct Paper {
    int id;
    char titulo[150];
    int *autores;
} Paper;

// Vértice do grafo
typedef struct AutorNode {
    int AutorId;
    char nome[100];
    struct AutorNode* prox; 
    struct Aresta* arestas;
} AutorNode;

typedef struct AutorRef {
    AutorNode* autor;           
    struct AutorRef* prox;
} AutorRef;

// Dados da Aresta
typedef struct PaperNode {
    int PaperId;
    char titulo[150];
    struct PaperNode* prox;
    struct AutorRef;    
} PaperNode;

typedef struct Aresta {
    struct AutorNode* destino;
    PaperNode* papers;
    struct Aresta* prox;    
} Aresta;


AutorNode* hashAutores[TAM]; 
PaperNode* hashTitulos[TAM];

int main(){

    // Estrutura do Arquivo:
    // Nome do pesquisador "tab" Titulo

    // TODO: Passos Iniciais: 
    // - ✅ Ler Arquivo
    // - ✅ Definir estruturas(definições básicas de tabela hash e de nós para o grafo)

    // TODO: Passos estruturais
    // - Criar tabela hash para autores
    // - Criar tabela hash para papers
    // - Criar grafo.

    // TODO: Passos de analise

    FILE *arquivo_pesq = fopen("dadosPesquisadores.txt", "r");

    char linha[256];
    char titulo[200];
    char autor[100];

    if(arquivo_pesq == NULL){
        perror("Erro ao abrir arquivo");
        return 1;
    };


   while (fgets(linha, sizeof(linha), arquivo_pesq)) {
        sscanf(linha, "%99[^\t]\t%199[^\n]", autor, titulo);

        printf("Autor : %s\n", autor);
        printf("Titulo: %s\n\n", titulo);
    }
    
    

    return 0;
}