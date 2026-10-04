#include <stdio.h>
#include <stdlib.h>

int main (void) {
FILE * arq;
arq = fopen("arquivo.txt", "w");
if (arq == NULL) {
    printf("Erro ao abrir o arquivo!");
    return 1;
}
else{
    printf("Arquivo aberto!");
}
fputc('C', arq);


fclose(arq);
return 0;
}