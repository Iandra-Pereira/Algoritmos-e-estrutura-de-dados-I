#include <stdio.h>
#include <stdlib.h>

int main(void){
    FILE * arq;
    arq = fopen("arquivo.txt", "w");
    if(arq == NULL){
        printf("Erro ao abrir o arquivo!\n");
        exit(1);
    }else{
        printf("Arquivo aberto com sucesso!\n");
    }

    int id;
    char nome[50];
    float salario;
    printf("Digite o ID do funcionário: ");
    scanf("%d", &id);   
    printf("Digite o nome do funcionário: ");
    scanf(" %[^\n]", nome);
    printf("Digite o salário do funcionário: ");
    scanf("%f", &salario);

    fprintf(arq, "%d %s %.2f\n", id
