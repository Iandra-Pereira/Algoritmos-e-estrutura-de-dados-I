//QUESTÃO 1- PRATICA 2 DE VETORES E MATRIZES
#include <stdio.h>
#include <stdlib.h>
int main(void) {
    int tamanho, i;
    printf("Digite o tamanho do vetor: ");
    scanf("%d", &tamanho);
    int *vetor= (int *) malloc (tamanho * sizeof(int));

    if (vetor == NULL) {
        printf("Erro na alocacao de memoria!\n");
        return 1;
    }

    printf("Digite os elementos do vetor:\n");
    for (i = 0; i < tamanho; i++) {
        printf("vetor[%d]: ", i);
        scanf("%d", &vetor[i]);
    }

    printf("Vetor na ordem inversa:\n");
    for (i = tamanho - 1; i >= 0; i--) {
        printf("%d ", vetor[i]);
    }

    free(vetor);
    return 0;
}
