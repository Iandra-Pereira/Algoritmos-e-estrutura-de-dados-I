#include <stdio.h>
#include <stdlib.h>
#include "aluno.h"

void cadastrarAluno(char * nome, int * mat){
    printf("Digite o nome do aluno: ");
    scanf(" %[^\\n]", nome);
    printf("Digite a matricula do aluno: ");
    scanf("%d", mat);
}

void imprimirAluno(char ** nome, int mat){
    printf("Nome do aluno: %s\n", *nome);
    printf("Matricula do aluno: %d\n", mat);
}

int main(void){
    char nome[50];
    int mat;
    cadastrarAluno(&nome, &mat);
    imprimirAluno(&nome, mat);

    return 0;
}