#include <stdio.h>
#define MAX 3

typedef struct pessoa{
    char nome[50];
    int idade;
    char cpf[20];
    char rg[20];
    char telefone[20];
} Pessoa;

void preenche(Pessoa * p){
    printf("Digite o nome: ");
    scanf(" %[^\n]", p->nome);
    printf("Digite a idade: ");
    scanf("%d", &p->idade);
    printf("Digite o CPF: ");
    scanf(" %[^\n]", p->cpf);
    printf("Digite o RG: ");
    scanf(" %[^\n]", p->rg);
    printf("Digite o telefone: ");
    scanf(" %[^\n]", p->telefone);
}

void imprime(Pessoa * p){
    printf("Nome: %s\n", p->nome);
    printf("Idade: %d\n", p->idade);
    printf("CPF: %s\n", p->cpf);
    printf("RG: %s\n", p->rg);
    printf("Telefone: %s\n", p->telefone);
}

int main (void){
    Pessoa p[MAX];

    for(int contador = 0; contador < MAX; contador++){
        preenche(&p[contador]);
    }
    for(int contador = 0; contador < MAX; contador++){
        imprime(&p[contador]);
    }
    return 0;
}