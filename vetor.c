#include <stdio.h>
int main (void){
    int vetor [5];
    int contador;
    printf("Digite os números dos vetores: ");
    for (contador=0; contador<5; contador++){
    scanf("%d", &vetor[contador]);
    }
    printf("FIM DO PROGRAMA!");
    return 0;
}