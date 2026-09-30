#include <stdio.h>
#include <stdlib.h>
#define MAX 2

typedef struct pessoa {
    char nome[50];
    int documento;
    int idade;
} Pessoa;

void preenche(Pessoa *f) {
    printf("Digite o nome: ");
    scanf(" %[^\n]", f->nome);
    printf("Digite o documento: ");
    scanf("%d", &f->documento);
    printf("Digite a idade: ");
    scanf("%d", &f->idade);
}

void imprime(Pessoa *f) {
    printf("Nome: %s\n", f->nome);
    printf("Documento: %d\n", f->documento);
    printf("Idade: %d\n", f->idade);
}

void altera_idade(Pessoa *f) {
    int nova_idade;
    printf("Digite a nova idade: ");
    scanf("%d", &nova_idade);
    f->idade = nova_idade;
}

void maior_menor(Pessoa *f) {
    int maior = 0;
    int menor = 0;
    for (int i = 1; i < MAX; i++) {
        if (f[i].idade > f[maior].idade) {
            maior = i;
        }
        if (f[i].idade < f[menor].idade) {
            menor = i;
        }
    }

    printf("\nPessoa com maior idade:\n");
    printf("Nome: %s\n", f[maior].nome);

    printf("\nPessoa com menor idade:\n");
    printf("Nome: %s\n", f[menor].nome);
}

int main(void) {
    Pessoa *f = malloc(MAX * sizeof(Pessoa));
    if (f == NULL) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    for (int i = 0; i < MAX; i++) {
        printf("\nPessoa %d:\n", i + 1);
        preenche(&f[i]);
    }

    maior_menor(f);
    free(f);
    return 0;
}