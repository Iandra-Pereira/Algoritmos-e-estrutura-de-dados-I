#include <stdio.h>
#include <stdlib.h>
int main() {
    int **matriz = NULL;
    int n, i, j;

    printf("Digite o tamanho da matriz: ");
    scanf("%d", &n);

    matriz = realloc(matriz, n * sizeof(int *));

    for (i = 0; i < n; i++) {
        matriz[i] = realloc(NULL, n * sizeof(int));
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            matriz[i][j] = rand() % 100 + 1;
        }
    }

    printf("\nMatriz:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%4d", matriz[i][j]);
        }
    }

    for (i = 0; i < n; i++) {
        free(matriz[i]);
    }

    free(matriz);

    return 0;
}