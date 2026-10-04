#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char nome[100];
    float preco;
} Fruta;

int main() {
    Fruta fruta;
    char continuar;

    FILE *arquivo = fopen("frutas.txt", "w");

    if (arquivo == NULL) {
        printf("ERRO!\n");
        return 1;
    }else{
      printf("SUCESSO!\n")
    }

    do {
        printf("Digite o nome da fruta: ");
        scanf(" %[^\n]", fruta.nome);

        printf("Digite o preco da fruta: ");
        scanf("%f", &fruta.preco);

        fprintf(arquivo, "%s, %f\n", fruta.nome, fruta.preco);

        printf("Deseja cadastrar outra fruta? (s/n): ");
        scanf(" %c", &continuar);

    } while (continuar == 's' || continuar == 'S');

    printf("Cadastro encerrado\n");

    fclose(arquivo);

    return 0;
}
