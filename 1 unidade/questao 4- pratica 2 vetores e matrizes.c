#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char **nomes = NULL;
    int *idades = NULL;
    int quantidade, i;

    printf("Digite o numero de pessoas: ");
    scanf("%d", &quantidade);
    getchar();

    if (quantidade <= 0 || quantidade > 100) {
        printf("Quantidade invalida! Digite um numero entre 1 e 100.\n");
        return 1;
    }

    nomes = realloc(nomes, quantidade * sizeof(char *));
    idades = realloc(idades, quantidade * sizeof(int));

    if (nomes == NULL || idades == NULL) {
        printf("Erro na alocacao de memoria!\n");
        return 1;
    }

    for (i = 0; i < quantidade; i++) {
        nomes[i] = realloc(NULL, 100 * sizeof(char));

        if (nomes[i] == NULL) {
            printf("Erro na alocacao de memoria!\n");
            return 1;
        }

        printf("\nDigite o nome completo da pessoa %d: ", i + 1);
        fgets(nomes[i], 100, stdin);

        printf("Digite a idade: ");
        scanf("%d", &idades[i]);
    }

    printf("\nPESSOAS CADASTRADAS\n");

    for (i = 0; i < quantidade; i++) {
        printf("%s\n", nomes[i]);
    }

    for (i = 0; i < quantidade; i++) {
        free(nomes[i]);
    }

    free(nomes);
    free(idades);
    return 0;
}