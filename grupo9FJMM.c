
// Grupo - 9
// Integrantes:
// Felipe Gabriel Senapeschi de Almeida - 817412
// João luis - 
// Mairon Cesar - 
// Matheus Borghes - 


#include <stdio.h>
#include <stdlib.h>

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
    struct AutorNode* prox;  
} AutorNode;

typedef struct Aresta {
    struct AutorNode* destino;
    PaperNode* papers;
    struct Aresta* prox;    
} Aresta;

// Dados da Aresta
typedef struct PaperNode {
    int PaperId;
    struct PaperNode* prox;
} PaperNode;

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

    if(arquivo_pesq == NULL){
        printf("Erro ao ler arquivo");
        return 1;
    };

    

    return 0;
}