#include <stdio.h>
#include "conversor.h"

int main(void) {
    float metros;
    printf("Digite um valor em metros: ");
    scanf("%f", &metros);
    float centimetros = metros_cetimetros(metros);
    float milimetros = metros_milimetros(metros);
    float kilometros = metros_kilometros(metros);

    printf("%f metros equivalem a %f centímetros, %f milímetros e %f quilômetros.\n", metros, centimetros, milimetros, kilometros);
    return 0;
}