#include <stdio.h>
#include <stdlib.h>
int main (void){
    int i, tamanho=10;
    int *vetor= (int *) malloc (tamanho * sizeof(int));
    if (vetor==0){
        printf("Erro ao alocar memoria");
        exit(1);
    }
    else{
        printf("O vetor foi alocado!\n");
    }
    for (i=0; i<tamanho; i++){
        vetor [i]= i;
        printf("%d \t", vetor [i]);
    }
}