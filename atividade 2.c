#include "raylib.h"
#include <stdlib.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600

typedef enum {
    MOEDA_BRONZE,
    MOEDA_PRATA,
    MOEDA_OURO,
    MOEDA_DIAMANTE
} TipoMoeda;

typedef struct {
    Vector2 pos;
    float raio;
    TipoMoeda tipo;
    int valor;
    bool coletada;
    float tempoColeta;
} Moeda;


Color corDaMoeda(TipoMoeda tipo)
{
    switch (tipo)
    {
        case MOEDA_BRONZE:
            return (Color){160, 90, 40, 255};

        case MOEDA_PRATA:
            return (Color){190, 190, 190, 255};

        case MOEDA_OURO:
            return GOLD;

        case MOEDA_DIAMANTE:
            return (Color){0, 220, 255, 255};

        default:
            return WHITE;
    }
}


int valorDaMoeda(TipoMoeda tipo)
{
    switch (tipo)
    {
        case MOEDA_BRONZE:
            return 5;

        case MOEDA_PRATA:
            return 10;

        case MOEDA_OURO:
            return 25;

        case MOEDA_DIAMANTE:
            return 50;

        default:
            return 0;
    }
}


Moeda *criarMoedas(int quantidade)
{
    Moeda *moedas = malloc(quantidade * sizeof(Moeda));

    for (int i = 0; i < quantidade; i++)
    {
        Moeda *m = moedas + i;

        m->pos = (Vector2){
            GetRandomValue(30, LARGURA_JANELA - 30),
            GetRandomValue(30, ALTURA_JANELA - 30)
        };

        m->raio = 10.0f;

        m->tipo = (TipoMoeda)GetRandomValue(
            MOEDA_BRONZE,
            MOEDA_OURO
        );

        int sorteio = GetRandomValue(0, 9);

        if (sorteio == 0)
        {
            m->tipo = MOEDA_DIAMANTE;
        }

        m->valor = valorDaMoeda(m->tipo);

        m->coletada = false;

        m->tempoColeta = 0;
    }

    return moedas;
}


bool tentarColetar(
    Moeda *m,
    Vector2 posJogador,
    float raioJogador
)
{
    if (m->coletada)
    {
        return false;
    }

    float dx = m->pos.x - posJogador.x;
    float dy = m->pos.y - posJogador.y;

    float distancia = dx * dx + dy * dy;

    float somaRaios =
        (m->raio + raioJogador) *
        (m->raio + raioJogador);

    if (distancia <= somaRaios)
    {
        m->coletada = true;
        m->tempoColeta = GetTime();

        return true;
    }

    return false;
}


void atualizarMoedas(Moeda *moedas, int quantidade)
{
    for (int i = 0; i < quantidade; i++)
    {
        Moeda *m = moedas + i;

        if (m->coletada &&
            GetTime() - m->tempoColeta >= 3.0f)
        {
            m->pos = (Vector2){
                GetRandomValue(30, LARGURA_JANELA - 30),
                GetRandomValue(30, ALTURA_JANELA - 30)
            };

            m->coletada = false;
        }
    }
}


int main(void)
{
    InitWindow(
        LARGURA_JANELA,
        ALTURA_JANELA,
        "Jogo das Moedas"
    );

    SetTargetFPS(60);

    int quantidadeMoedas = 10;

    Moeda *moedas = criarMoedas(quantidadeMoedas);

    Vector2 posJogador = {
        LARGURA_JANELA / 2,
        ALTURA_JANELA / 2
    };

    float raioJogador = 15;

    int moedasColetadas = 0;
    int pontuacao = 0;

    while (!WindowShouldClose())
    {
        /* Movimento */

        if (IsKeyDown(KEY_RIGHT))
            posJogador.x += 4;

        if (IsKeyDown(KEY_LEFT))
            posJogador.x -= 4;

        if (IsKeyDown(KEY_UP))
            posJogador.y -= 4;

        if (IsKeyDown(KEY_DOWN))
            posJogador.y += 4;


        for (int i = 0; i < quantidadeMoedas; i++)
        {
            if (tentarColetar(
                &moedas[i],
                posJogador,
                raioJogador
            ))
            {
                moedasColetadas++;
                pontuacao += moedas[i].valor;
            }
        }

        atualizarMoedas(
            moedas,
            quantidadeMoedas
        );

        BeginDrawing();

        ClearBackground(RAYWHITE);


        for (int i = 0; i < quantidadeMoedas; i++)
        {
            Moeda *m = moedas + i;

            if (!m->coletada)
            {
                DrawCircleV(
                    m->pos,
                    m->raio,
                    corDaMoeda(m->tipo)
                );
            }
        }


        DrawCircleV(
            posJogador,
            raioJogador,
            BLUE
        );


        DrawText(
            TextFormat(
                "Moedas coletadas: %d",
                moedasColetadas
            ),
            10,
            10,
            20,
            BLACK
        );

        DrawText(
            TextFormat(
                "Pontuacao: %d",
                pontuacao
            ),
            10,
            40,
            20,
            BLACK
        );

        DrawText(
            TextFormat(
                "Quantidade de moedas: %d",
                quantidadeMoedas
            ),
            10,
            70,
            20,
            BLACK
        );


        EndDrawing();
    }


    free(moedas);

    CloseWindow();

    return 0;
}
