#include "raylib.h"
#include <stdlib.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600

typedef struct{
    Vector2 pos;
    Vector2 vel;
    float raio;
    Color cor;
} Bola;

Bola *criarBolas(int quantidade){
    Bola *bolas = (Bola *)malloc( quantidade * sizeof(Bola));

    if (bolas == NULL) {
        return NULL;
    }

    for (int i = 0; i < quantidade; i++){
        Bola *b = bolas + i;
        b->pos.x = GetRandomValue(
            50,
            LARGURA_JANELA - 50
        );

        b->pos.y = GetRandomValue(
            50,
            ALTURA_JANELA - 50
        );

        b->vel.x = GetRandomValue(-3, 3);
        b->vel.y = GetRandomValue(-3, 3);

        if (b->vel.x == 0){
            b->vel.x = 2;
        }

        if (b->vel.y == 0) {
            b->vel.y = 2;
        }

        b->raio = 12;

        b->cor = (Color){
            GetRandomValue(50, 255),
            GetRandomValue(50, 255),
            GetRandomValue(50, 255),
            255
        };
    }

    return bolas;
}

void atualizarBola(Bola *b){
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;

    if (
        b->pos.x - b->raio < 0 ||
        b->pos.x + b->raio > LARGURA_JANELA
    )
    {
        b->vel.x *= -1;
    }

    if (
        b->pos.y - b->raio < 0 ||
        b->pos.y + b->raio > ALTURA_JANELA
    )
    {
        b->vel.y *= -1;
    }
}

int main(void){
    InitWindow(
        LARGURA_JANELA,
        ALTURA_JANELA,
        "Exercicio 1 - Realloc"
    );

    SetTargetFPS(60);

    int quantidadeBolas = 5;

    Bola *bolas = criarBolas(quantidadeBolas);

    if (bolas == NULL) {
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose()){
        if (IsKeyPressed(KEY_SPACE))
        {
            quantidadeBolas++;

            Bola *temp = (Bola *)realloc(bolas,quantidadeBolas * sizeof(Bola));

            if (temp != NULL) {
                bolas = temp;

                Bola *b = bolas + quantidadeBolas - 1;

                b->pos.x = GetRandomValue(
                    50,
                    LARGURA_JANELA - 50
                );

                b->pos.y = GetRandomValue(
                    50,
                    ALTURA_JANELA - 50
                );

                b->vel.x = GetRandomValue(-3, 3);
                b->vel.y = GetRandomValue(-3, 3);

                if (b->vel.x == 0) {
                    b->vel.x = 2;
                }

                if (b->vel.y == 0) {
                    b->vel.y = 2;
                }

                b->raio = 12;

                b->cor = (Color){
                    GetRandomValue(50, 255),
                    GetRandomValue(50, 255),
                    GetRandomValue(50, 255),
                    255
                };
            }
            else
            {
                quantidadeBolas--;
            }
        }

        if (
            IsKeyPressed(KEY_BACKSPACE) &&
            quantidadeBolas > 1
        )
        {
            quantidadeBolas--;

            Bola *temp = (Bola *)realloc(
                bolas,
                quantidadeBolas * sizeof(Bola)
            );

            if (temp != NULL)
            {
                bolas = temp;
            }
            else
            {
                quantidadeBolas++;
            }
        }

        for (int i = 0; i < quantidadeBolas; i++)
        {
            atualizarBola(bolas + i);
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < quantidadeBolas; i++)
        {
            DrawCircleV(
                bolas[i].pos,
                bolas[i].raio,
                bolas[i].cor
            );
        }

        DrawText(
            TextFormat(
                "Bolas: %d",
                quantidadeBolas
            ),
            10,
            10,
            20,
            BLACK
        );

        DrawText(
            "ESPACO: adicionar",
            10,
            40,
            20,
            BLACK
        );

        DrawText(
            "BACKSPACE: remover",
            10,
            70,
            20,
            BLACK
        );

        EndDrawing();
    }

    free(bolas);

    CloseWindow();

    return 0;
}
