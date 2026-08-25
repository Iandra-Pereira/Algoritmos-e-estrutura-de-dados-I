#include <stdio.h>
int main (void){
    int num;
    int *ponteiro;
    printf("Digite um número: ");
    scanf("%d", &num);
    ponteiro= &num;
    printf("O número digitado foi %d, e o seu endereço é: %p", num, ponteiro);
    return 0;
}