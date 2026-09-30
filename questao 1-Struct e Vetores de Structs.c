#include <stdio.h>
#include <stdlib.h>
#define MAX 2

typedef struct funcionario {
    char nome[50];
    float salario;
    int identificador;
    char cargo[20];
} Funcionario;

void preenche(Funcionario *f) {
    printf("Digite o nome: ");
    scanf(" %49[^\n]", f->nome);
    printf("Digite o identificador: ");
    scanf("%d", &f->identificador);
    printf("Digite o salario: ");
    scanf("%f", &f->salario);
    printf("Digite o cargo: ");
    scanf(" %19[^\n]", f->cargo);
}

void imprime(Funcionario *f) {
    printf("Nome: %s\n", f->nome);
    printf("Identificador: %d\n", f->identificador);
    printf("Salario: %f\n", f->salario);
    printf("Cargo: %s\n", f->cargo);
}

void altera_salario(Funcionario *funcionario) {
    float novo_salario;
    printf("Digite o novo salario: ");
    scanf("%f", &novo_salario);
    funcionario->salario = novo_salario;
}

void maior_menor(Funcionario *f) {
    int maior = 0;
    int menor = 0;
    for (int i = 1; i < MAX; i++) {
        if (f[i].salario > f[maior].salario) {
            maior = i;
        }
        if (f[i].salario < f[menor].salario) {
            menor = i;
        }
    }

    printf("\nFuncionario com maior salario:\n");
    printf("Cargo: %s\n", f[maior].cargo);
    printf("Salario: %f\n", f[maior].salario);

    printf("\nFuncionario com menor salario:\n");
    printf("Cargo: %s\n", f[menor].cargo);
    printf("Salario: %f\n", f[menor].salario);
}

int main(void) {
    Funcionario *f = malloc(MAX * sizeof(Funcionario));
    if (f == NULL) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    for (int i = 0; i < MAX; i++) {
        printf("\nFuncionario %d:\n", i + 1);
        preenche(&f[i]);
    }

    maior_menor(f);
    free(f);
    return 0;
}