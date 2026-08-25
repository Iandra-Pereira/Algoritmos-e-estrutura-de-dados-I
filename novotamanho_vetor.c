#include <stdio.h>
#include <stdlib.h>
int main(void){
    int tamanho=5, novo_tamanho, contador;
    int *vetor= (int *) malloc (tamanho * sizeof(int));
    printf("Entre com o novo tamanho do vetor: ");
    scanf("%d", &novo_tamanho);
    vetor= (int *) malloc (novo_tamanho * sizeof(int));
    if (vetor ==0){
        printf("Erro ao alocar na memoria");
    }
    else{
        printf("O vetor foi alocado!\n");
    }
    printf("Digite os números do vetor: ");
    for (contador=0; contador < novo_tamanho; contador++){
        scanf("%d", &vetor[contador]);
    }
    return 0;
}