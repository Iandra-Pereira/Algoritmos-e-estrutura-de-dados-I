#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float nota1,nota2,nota3;
    char nome[50];
} Aluno;

int main (void){
    int N;
    printf("Digite o numero de alunos: ");
    scanf("%d",&N);

    Aluno *alunos = (Aluno*) malloc(N * sizeof(Aluno));
    if (alunos == NULL) {
        printf("ERRO\n");
        return 1;
    }else{
        printf("SUCESSO\n");
    }

    int i;
    for(i=0;i<N;i++){
        printf("Digite o nome do aluno %d: ");
        scanf(" %[^\n]", alunos[i].nome);
        printf("Digite a nota 1 do aluno %d: ", i+1);
        scanf("%f", &alunos[i].nota1);
        printf("Digite a nota 2 do aluno %d: ", i+1);
        scanf("%f", &alunos[i].nota2);
        printf("Digite a nota 3 do aluno %d: ", i+1);
        scanf("%f", &alunos[i].nota3);
    }

    FILE * arq;
    arq = fopen("alunos.txt", "w");
    if(arq == NULL){
        printf("ERRO\n");
        return 1;0
    }else{
        printf("SUCESSO\n");
    }

    for(i=0;i<N;i++){
        fprintf(arq, "Nome: %s\n", alunos[i].nome);
        fprintf(arq, "Nota 1: %f\n", alunos[i].nota1);
        fprintf(arq, "Nota 2: %f\n", alunos[i].nota2);
        fprintf(arq, "Nota 3: %f\n", alunos[i].nota3);
    }
  
    fclose(arq);
    free(alunos);
    return 0;
}
