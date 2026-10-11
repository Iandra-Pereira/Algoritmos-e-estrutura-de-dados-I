#include "raylib.h"
#include "entidade.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA,
               "Atividade 8B - Reutilizacao do Modulo");

    SetTargetFPS(60);

    Entidade *jogador = entidadeCriar(
        ENTIDADE_JOGADOR, (Vector2){200, 300}
    );

    Entidade *inimigo = entidadeCriar(
        ENTIDADE_INIMIGO, (Vector2){400, 300}
    );

    Entidade *item = entidadeCriar(
        ENTIDADE_ITEM, (Vector2){600, 300}
    );

    if (jogador == NULL || inimigo == NULL || item == NULL) {
        free(jogador);
        free(inimigo);
        free(item);

        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        entidadeDesenhar(jogador);
        entidadeDesenhar(inimigo);
        entidadeDesenhar(item);

        DrawText("Jogador", 165, 340, 20, DARKGRAY);
        DrawText("Inimigo", 365, 340, 20, DARKGRAY);
        DrawText("Item", 580, 340, 20, DARKGRAY);

        DrawText("Exercicio 2 - Reutilizacao do modulo",
                 150, 30, 24, BLACK);

        DrawText("Pressione ESC para sair",
                 10, ALTURA_JANELA - 30, 16, GRAY);

        EndDrawing();
    }

    free(jogador);
    free(inimigo);
    free(item);

    CloseWindow();

    return 0;
}
