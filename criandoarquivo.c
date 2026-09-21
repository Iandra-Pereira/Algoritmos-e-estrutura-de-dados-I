int main (void) {
FILE * arq;
char linha[100];
arq = fopen("arquivo.txt", "r");
if (arq == NULL) {
    printf("Erro ao abrir o arquivo!");
    return 1;
}
else{
    printf("Arquivo aberto!");
}

while (!feof(arq)) {
    //fgets(linha, 100, arq);
    fscanf(arq, "%s", linha);
    printf("Linha lida: %s\n", linha);
}

fclose(arq);
return 0;
}
