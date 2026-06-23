
// Grupo - 9
// Integrantes:
// Felipe Gabriel Senapeschi de Almeida - 817412
// João luis - 
// Mairon Cesar - 
// Matheus Borghes - 


#include <stdio.h>
#include <stdlib.h>

int main(){

    // Estrutura do Arquivo:
    // Nome do pesquisador "tab" Titulo

    // TODO: Passos Iniciais: 
    // - Ler Arquivo
    // - Implementar grafo


    // Abre arquivo
    FILE *arquivo_pesq = fopen("dadosPesquisadores.txt", "r");

    // Valida a abertura do arquivo
    if(arquivo_pesq == NULL){
        printf("Erro ao ler arquivo");
        return 1;
    };

    

    return 0;
}