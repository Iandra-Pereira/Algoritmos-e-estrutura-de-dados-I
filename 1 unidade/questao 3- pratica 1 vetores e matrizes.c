#include <stdio.h>
#include <stdlib.h>
int main() {
    char **frutas = NULL;
    float *precos = NULL;
    int quantidade, i;

    printf("Digite a quantidade de frutas: ");
    scanf("%d", &quantidade);

    for (i = 0; i < quantidade; i++) {
        frutas = realloc(frutas, (i + 1) * sizeof(char *));
        precos = realloc(precos, (i + 1) * sizeof(float));

        if (frutas == NULL || precos == NULL) {
            printf("Erro na alocacao de memoria!\n");
            return 1;
        }

        frutas[i] = realloc(NULL, 50 * sizeof(char));
        if (frutas[i] == NULL) {
            printf("Erro na alocacao de memoria!\n");
            return 1;
        }

        printf("\nDigite o nome da fruta: ");
        scanf("%49s", frutas[i]);
        printf("Digite o preco: R$ ");
        scanf("%f", &precos[i]);
    }

    printf("\nFRUTAS CADASTRADAS\n");
    for (i = 0; i < quantidade; i++) {
        printf("%s - R$ %.2f\n", frutas[i], precos[i]);
    }
    for (i = 0; i < quantidade; i++) {
        free(frutas[i]);
    }

    free(frutas);
    free(precos);
    return 0;
}