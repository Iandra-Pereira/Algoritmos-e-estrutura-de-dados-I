#include <stdio.h>
#include "geometria.h"

int main(void) {
    float base, altura, raio;
    printf("Digite a base e a altura do retângulo: ");
    scanf("%f %f", &base, &altura);
    float area_ret = area_retangulo(base, altura);
    printf("Área do retângulo: %f\n", area_ret);

    printf("Digite a base e a altura do triângulo: ");
    scanf("%f %f", &base, &altura);
    float area_tri = area_triangulo(base, altura);
    printf("Área do triângulo: %f\n", area_tri);

    printf("Digite o raio do círculo: ");
    scanf("%f", &raio);
    float area_circ = area_circulo(raio);
    printf("Área do círculo: %f\n", area_circ);

    return 0;
}