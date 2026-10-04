#include <stdio.h>
#include "calculadora.h"

int main(void) {
    int v1, v2;
    printf("Digite dois números inteiros: ");
    scanf("%d %d", &v1, &v2);
    int soma_v= soma(v1, v2);
    printf("Soma: %d\n", soma_v);
    return 0;
}