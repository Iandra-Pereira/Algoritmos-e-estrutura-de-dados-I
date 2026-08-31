#include <stdio.h>
#include <stdlib.h>
int main() {
    int **matriz = NULL;
    int linhas, colunas;
    int i, j;

    printf("Digite o numero de linhas: ");
    scanf("%d", &linhas);
    printf("Digite o numero de colunas: ");
    scanf("%d", &colunas);

    matriz = realloc(matriz, linhas * sizeof(int *));
    if (matriz == NULL) {
        printf("Erro na alocacao de memoria!\n");
        return 1;
    }
    for (i = 0; i < linhas; i++) {
        matriz[i] = realloc(matriz[i], colunas * sizeof(int));

        if (matriz[i] == NULL) {
            printf("Erro na alocacao de memoria!\n");
            return 1;
        }
    }

    printf("Digite os elementos da matriz: ");
    for (i = 0; i < linhas; i++) {
        for (j = 0; j < colunas; j++) {
            printf("matriz[%d][%d]: ", i, j);
            scanf("%d", &matriz[i][j]);
        }
    }

    printf("\nMatriz original:\n");
    for (i = 0; i < linhas; i++) {
        for (j = 0; j < colunas; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }

    printf("\nMatriz transposta:\n");
    for (j = 0; j < colunas; j++) {
        for (i = 0; i < linhas; i++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }

    for (i = 0; i < linhas; i++) {
        free(matriz[i]);
    }

    free(matriz);
    return 0;
}