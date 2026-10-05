#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

#define LARGURA_JANELA 1000
#define ALTURA_JANELA 700
#define PONTUACAO_MAX 500
#define QUANTIDADE 20

typedef struct {
    char nome[16];
    int pontuacao;
} Placar;

Placar *criarPlacares(int quantidade) {
    Placar *placares = (Placar *)malloc(quantidade * sizeof(Placar));

    if (placares == NULL) {
        return NULL;
    }

    for (int i = 0; i < quantidade; i++) {
        Placar *p = placares + i;

        TextCopy(p->nome, TextFormat("J%02d", i + 1));
        p->pontuacao = GetRandomValue(10, PONTUACAO_MAX);
    }

    return placares;
}

void ordenarBubbleSort(Placar *vetor, int n, long *comparacoes, long *trocas) {
    *comparacoes = 0;
    *trocas = 0;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            (*comparacoes)++;

            if (vetor[j].pontuacao > vetor[j + 1].pontuacao) {
                Placar temp = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = temp;

                (*trocas)++;
            }
        }
    }
}

void ordenarInsertionSort(Placar *vetor, int n, long *comparacoes, long *trocas) {
    *comparacoes = 0;
    *trocas = 0;

    for (int i = 1; i < n; i++) {
        Placar chave = vetor[i];
        int j = i - 1;

        while (j >= 0) {
            (*comparacoes)++;

            if (vetor[j].pontuacao > chave.pontuacao) {
                vetor[j + 1] = vetor[j];
                (*trocas)++;
                j--;
            } else {
                break;
            }
        }

        vetor[j + 1] = chave;
    }
}

int buscaSequencial(Placar *vetor, int n, int alvo, long *comparacoes) {
    *comparacoes = 0;

    for (int i = 0; i < n; i++) {
        (*comparacoes)++;

        if (vetor[i].pontuacao == alvo) {
            return i;
        }
    }

    return -1;
}

int buscaBinaria(Placar *vetor, int n, int alvo, long *comparacoes) {
    *comparacoes = 0;

    int inicio = 0;
    int fim = n - 1;

    while (inicio <= fim) {
        (*comparacoes)++;

        int meio = (inicio + fim) / 2;

        if (vetor[meio].pontuacao == alvo) {
            return meio;
        }

        if (vetor[meio].pontuacao < alvo) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    return -1;
}

void desenharPlacares(Placar *vetor, int n) {
    int larguraBarra = 35;
    int espacamento = 12;
    int baseY = 600;

    for (int i = 0; i < n; i++) {
        int altura = (vetor[i].pontuacao * 400) / PONTUACAO_MAX;

        int x = 30 + i * (larguraBarra + espacamento);
        int y = baseY - altura;

        DrawRectangle(
            x,
            y,
            larguraBarra,
            altura,
            BLUE
        );

        DrawText(
            vetor[i].nome,
            x,
            baseY + 10,
            14,
            BLACK
        );

        DrawText(
            TextFormat("%d", vetor[i].pontuacao),
            x,
            y - 20,
            14,
            BLACK
        );
    }
}

int main(void) {
    InitWindow(
        LARGURA_JANELA,
        ALTURA_JANELA,
        "Atividade 7"
    );

    SetTargetFPS(60);

    Placar *vetor = criarPlacares(QUANTIDADE);

    if (vetor == NULL) {
        CloseWindow();
        return 1;
    }

    int quantidade = QUANTIDADE;

    long comparacoesBubble = 0;
    long trocasBubble = 0;

    long comparacoesInsertion = 0;
    long trocasInsertion = 0;

    long comparacoesSequencial = 0;
    long comparacoesBinaria = 0;

    double tempoBubble = 0.0;

    int resultadoSequencial = -1;
    int resultadoBinaria = -1;

    int alvo = 0;

    bool ordenado = false;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_B)) {
            tempoBubble = 0.0;

            double inicio = GetTime();

            ordenarBubbleSort(
                vetor,
                quantidade,
                &comparacoesBubble,
                &trocasBubble
            );

            double fim = GetTime();

            tempoBubble = (fim - inicio) * 1000.0;

            ordenado = true;

            resultadoSequencial = -1;
            resultadoBinaria = -1;
        }

        if (IsKeyPressed(KEY_I)) {
            ordenarInsertionSort(
                vetor,
                quantidade,
                &comparacoesInsertion,
                &trocasInsertion
            );

            ordenado = true;

            resultadoSequencial = -1;
            resultadoBinaria = -1;
        }

        if (IsKeyPressed(KEY_S)) {
            alvo = GetRandomValue(10, PONTUACAO_MAX);

            resultadoSequencial = buscaSequencial(
                vetor,
                quantidade,
                alvo,
                &comparacoesSequencial
            );
        }

        if (IsKeyPressed(KEY_W)) {
            if (ordenado) {
                alvo = GetRandomValue(10, PONTUACAO_MAX);

                resultadoBinaria = buscaBinaria(
                    vetor,
                    quantidade,
                    alvo,
                    &comparacoesBinaria
                );
            }
        }

        if (IsKeyPressed(KEY_R)) {
            for (int i = 0; i < quantidade; i++) {
                vetor[i].pontuacao =
                    GetRandomValue(10, PONTUACAO_MAX);
            }

            comparacoesBubble = 0;
            trocasBubble = 0;

            comparacoesInsertion = 0;
            trocasInsertion = 0;

            comparacoesSequencial = 0;
            comparacoesBinaria = 0;

            tempoBubble = 0.0;

            resultadoSequencial = -1;
            resultadoBinaria = -1;

            ordenado = false;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(
            "Atividade 7 - Complexidade de Algoritmos",
            20,
            20,
            28,
            BLACK
        );

        DrawText(
            "B: Bubble Sort",
            20,
            60,
            18,
            DARKGRAY
        );

        DrawText(
            "I: Insertion Sort",
            20,
            85,
            18,
            DARKGRAY
        );

        DrawText(
            "S: Busca Sequencial",
            20,
            110,
            18,
            DARKGRAY
        );

        DrawText(
            "W: Busca Binaria",
            20,
            135,
            18,
            DARKGRAY
        );

        DrawText(
            "R: Novo vetor",
            20,
            160,
            18,
            DARKGRAY
        );

        DrawText(
            TextFormat(
                "Bubble: %ld comparacoes | %ld trocas | %.3f ms",
                comparacoesBubble,
                trocasBubble,
                tempoBubble
            ),
            400,
            60,
            18,
            BLUE
        );

        DrawText(
            TextFormat(
                "Insertion: %ld comparacoes | %ld trocas",
                comparacoesInsertion,
                trocasInsertion
            ),
            400,
            90,
            18,
            RED
        );

        DrawText(
            TextFormat(
                "Sequencial: %ld comparacoes",
                comparacoesSequencial
            ),
            400,
            120,
            18,
            DARKGREEN
        );

        DrawText(
            TextFormat(
                "Binaria: %ld comparacoes",
                comparacoesBinaria
            ),
            400,
            150,
            18,
            PURPLE
        );

        if (resultadoSequencial >= 0) {
            DrawText(
                TextFormat(
                    "Sequencial encontrou %d na posicao %d",
                    alvo,
                    resultadoSequencial
                ),
                20,
                190,
                18,
                DARKGREEN
            );
        } else if (comparacoesSequencial > 0) {
            DrawText(
                TextFormat(
                    "Sequencial: %d nao encontrado",
                    alvo
                ),
                20,
                190,
                18,
                RED
            );
        }

        if (resultadoBinaria >= 0) {
            DrawText(
                TextFormat(
                    "Binaria encontrou %d na posicao %d",
                    alvo,
                    resultadoBinaria
                ),
                20,
                215,
                18,
                PURPLE
            );
        } else if (comparacoesBinaria > 0) {
            DrawText(
                TextFormat(
                    "Binaria: %d nao encontrado",
                    alvo
                ),
                20,
                215,
                18,
                RED
            );
        }

        if (ordenado) {
            DrawText(
                "Vetor ordenado",
                750,
                20,
                20,
                DARKGREEN
            );
        } else {
            DrawText(
                "Vetor nao ordenado",
                750,
                20,
                20,
                RED
            );
        }

        desenharPlacares(vetor, quantidade);

        EndDrawing();
    }

    free(vetor);

    CloseWindow();

    return 0;
}
