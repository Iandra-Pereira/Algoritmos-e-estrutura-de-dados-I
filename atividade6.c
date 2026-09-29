#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define RAIO_JOGADOR 20.0f
#define MAX_ENTIDADES 30
#define TOTAL_INIMIGOS 5
#define TOTAL_ITENS 6
#define ARQUIVO_PLACAR "placar.txt"
#define ARQUIVO_SAVE "save.bin"

typedef enum {
    ENTIDADE_JOGADOR,
    ENTIDADE_INIMIGO,
    ENTIDADE_ITEM
} TipoEntidade;

typedef union {
    int dano;
    int valor;
} ExtraEntidade;

typedef struct {
    TipoEntidade tipo;
    Vector2 pos;
    float raio;
    int vida;
    Color cor;
    ExtraEntidade extra;
} Entidade;

Entidade *vetorEntidades[MAX_ENTIDADES];
int totalEntidades = 0;

Entidade *criarEntidade(TipoEntidade tipo, Vector2 pos)
{
    Entidade *e = (Entidade *)malloc(sizeof(Entidade));

    if (e == NULL)
        return NULL;

    e->tipo = tipo;
    e->pos = pos;

    e->raio =
        (tipo == ENTIDADE_JOGADOR) ? RAIO_JOGADOR :
        (tipo == ENTIDADE_INIMIGO) ? 15.0f : 8.0f;

    switch (tipo)
    {
        case ENTIDADE_JOGADOR:
            e->vida = 100;
            e->cor = BLUE;
            break;

        case ENTIDADE_INIMIGO:
            e->vida = 40;
            e->cor = MAROON;
            e->extra.dano = GetRandomValue(5, 15);
            break;

        case ENTIDADE_ITEM:
            e->vida = 1;
            e->cor = GOLD;
            e->extra.valor = GetRandomValue(5, 20);
            break;
    }

    return e;
}

void adicionarEntidade(Entidade *e)
{
    if (e == NULL || totalEntidades >= MAX_ENTIDADES)
        return;

    vetorEntidades[totalEntidades] = e;
    totalEntidades++;
}

void removerEntidade(int indice)
{
    if (indice < 0 || indice >= totalEntidades)
        return;

    free(vetorEntidades[indice]);

    vetorEntidades[indice] =
        vetorEntidades[totalEntidades - 1];

    totalEntidades--;
}

void liberarTodasEntidades(void)
{
    for (int i = 0; i < totalEntidades; i++)
    {
        free(vetorEntidades[i]);
        vetorEntidades[i] = NULL;
    }

    totalEntidades = 0;
}

bool colidiu(Entidade *a, Entidade *b)
{
    float dx = a->pos.x - b->pos.x;
    float dy = a->pos.y - b->pos.y;

    float distancia =
        sqrtf(dx * dx + dy * dy);

    return distancia <=
           (a->raio + b->raio);
}

void ordenarPorDistancia(Entidade *jogador)
{
    for (int i = 1; i < totalEntidades; i++)
    {
        int indiceMaisProximo = i;

        float dx =
            vetorEntidades[i]->pos.x -
            jogador->pos.x;

        float dy =
            vetorEntidades[i]->pos.y -
            jogador->pos.y;

        float menorDistancia =
            dx * dx + dy * dy;

        for (int j = i + 1; j < totalEntidades; j++)
        {
            float dxAtual =
                vetorEntidades[j]->pos.x -
                jogador->pos.x;

            float dyAtual =
                vetorEntidades[j]->pos.y -
                jogador->pos.y;

            float distanciaAtual =
                dxAtual * dxAtual +
                dyAtual * dyAtual;

            if (distanciaAtual < menorDistancia)
            {
                menorDistancia = distanciaAtual;
                indiceMaisProximo = j;
            }
        }

        if (indiceMaisProximo != i)
        {
            Entidade *tmp =
                vetorEntidades[i];

            vetorEntidades[i] =
                vetorEntidades[indiceMaisProximo];

            vetorEntidades[indiceMaisProximo] =
                tmp;
        }
    }
}

void desenharEntidade(Entidade *e)
{
    DrawCircleV(
        e->pos,
        e->raio,
        e->cor
    );

    if (e->tipo == ENTIDADE_INIMIGO)
    {
        DrawText(
            TextFormat("%d", e->vida),
            e->pos.x - 8,
            e->pos.y - 26,
            14,
            BLACK
        );
    }
}

void salvarPlacarTexto(char *nomeJogador, int pontuacao)
{
    FILE *arquivo =
        fopen(ARQUIVO_PLACAR, "a");

    if (arquivo == NULL)
        return;

    fprintf(
        arquivo,
        "%s %d\n",
        nomeJogador,
        pontuacao
    );

    fclose(arquivo);
}

int lerMelhorPontuacao(void)
{
    FILE *arquivo =
        fopen(ARQUIVO_PLACAR, "r");

    if (arquivo == NULL)
        return 0;

    char nomeLido[16];
    int valor;
    int melhor = 0;

    while (
        fscanf(
            arquivo,
            "%15s %d",
            nomeLido,
            &valor
        ) == 2
    )
    {
        if (valor > melhor)
            melhor = valor;
    }

    fclose(arquivo);

    return melhor;
}

bool salvarJogoBinario(void)
{
    FILE *arquivo =
        fopen(ARQUIVO_SAVE, "wb");

    if (arquivo == NULL)
        return false;

    fwrite(
        &totalEntidades,
        sizeof(int),
        1,
        arquivo
    );

    for (int i = 0; i < totalEntidades; i++)
    {
        fwrite(
            vetorEntidades[i],
            sizeof(Entidade),
            1,
            arquivo
        );
    }

    fclose(arquivo);

    return true;
}

bool carregarJogoBinario(void)
{
    FILE *arquivo =
        fopen(ARQUIVO_SAVE, "rb");

    if (arquivo == NULL)
        return false;

    int totalSalvo = 0;

    if (
        fread(
            &totalSalvo,
            sizeof(int),
            1,
            arquivo
        ) != 1
    )
    {
        fclose(arquivo);
        return false;
    }

    if (totalSalvo < 1 ||
        totalSalvo > MAX_ENTIDADES)
    {
        fclose(arquivo);
        return false;
    }

    liberarTodasEntidades();

    for (int i = 0; i < totalSalvo; i++)
    {
        Entidade *e =
            (Entidade *)malloc(sizeof(Entidade));

        if (e == NULL)
        {
            fclose(arquivo);
            liberarTodasEntidades();
            return false;
        }

        if (
            fread(
                e,
                sizeof(Entidade),
                1,
                arquivo
            ) != 1
        )
        {
            free(e);
            fclose(arquivo);
            liberarTodasEntidades();
            return false;
        }

        adicionarEntidade(e);
    }

    fclose(arquivo);

    return true;
}

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(
        LARGURA_JANELA,
        ALTURA_JANELA,
        "Atividade 6 - Arquivos"
    );

    SetTargetFPS(60);

    char nomeJogador[16] = "Jogador";

    Entidade *jogador =
        criarEntidade(
            ENTIDADE_JOGADOR,
            (Vector2){
                LARGURA_JANELA / 2.0f,
                ALTURA_JANELA / 2.0f
            }
        );

    adicionarEntidade(jogador);

    for (int i = 0; i < TOTAL_INIMIGOS; i++)
    {
        Vector2 pos = {
            GetRandomValue(
                30,
                LARGURA_JANELA - 30
            ),
            GetRandomValue(
                30,
                ALTURA_JANELA - 30
            )
        };

        adicionarEntidade(
            criarEntidade(
                ENTIDADE_INIMIGO,
                pos
            )
        );
    }

    for (int i = 0; i < TOTAL_ITENS; i++)
    {
        Vector2 pos = {
            GetRandomValue(
                30,
                LARGURA_JANELA - 30
            ),
            GetRandomValue(
                30,
                ALTURA_JANELA - 30
            )
        };

        adicionarEntidade(
            criarEntidade(
                ENTIDADE_ITEM,
                pos
            )
        );
    }

    int pontuacao = 0;
    int melhorPontuacao = lerMelhorPontuacao();

    char mensagem[64] = "";
    float tempoMensagem = 0;

    while (!WindowShouldClose())
    {
        float vel =
            250.0f * GetFrameTime();

        if (IsKeyDown(KEY_RIGHT))
            jogador->pos.x += vel;

        if (IsKeyDown(KEY_LEFT))
            jogador->pos.x -= vel;

        if (IsKeyDown(KEY_UP))
            jogador->pos.y -= vel;

        if (IsKeyDown(KEY_DOWN))
            jogador->pos.y += vel;

        if (jogador->pos.x < jogador->raio)
            jogador->pos.x = jogador->raio;

        if (jogador->pos.x >
            LARGURA_JANELA - jogador->raio)
        {
            jogador->pos.x =
                LARGURA_JANELA - jogador->raio;
        }

        if (jogador->pos.y < jogador->raio)
            jogador->pos.y = jogador->raio;

        if (jogador->pos.y >
            ALTURA_JANELA - jogador->raio)
        {
            jogador->pos.y =
                ALTURA_JANELA - jogador->raio;
        }

        if (IsKeyPressed(KEY_N))
        {
            if (totalEntidades < MAX_ENTIDADES)
            {
                Vector2 pos = {
                    GetRandomValue(
                        30,
                        LARGURA_JANELA - 30
                    ),
                    GetRandomValue(
                        30,
                        ALTURA_JANELA - 30
                    )
                };

                adicionarEntidade(
                    criarEntidade(
                        ENTIDADE_ITEM,
                        pos
                    )
                );
            }
        }

        for (int i = 1; i < totalEntidades; i++)
        {
            Entidade *e =
                vetorEntidades[i];

            if (!colidiu(jogador, e))
                continue;

            if (e->tipo == ENTIDADE_ITEM)
            {
                pontuacao += e->extra.valor;

                removerEntidade(i);

                i--;
            }
            else if (e->tipo == ENTIDADE_INIMIGO)
            {
                jogador->vida -=
                    e->extra.dano;

                if (jogador->vida < 0)
                    jogador->vida = 0;
            }
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            for (int i = 1; i < totalEntidades; i++)
            {
                Entidade *e =
                    vetorEntidades[i];

                if (e->tipo != ENTIDADE_INIMIGO)
                    continue;

                if (!colidiu(jogador, e))
                {
                    e->vida -= 20;

                    if (e->vida <= 0)
                    {
                        removerEntidade(i);
                    }

                    break;
                }
            }
        }

        if (IsKeyPressed(KEY_F5))
        {
            salvarPlacarTexto(
                nomeJogador,
                pontuacao
            );

            if (pontuacao > melhorPontuacao)
                melhorPontuacao = pontuacao;

            strcpy(
                mensagem,
                "Placar salvo!"
            );

            tempoMensagem = 2.0f;
        }

        if (IsKeyPressed(KEY_F6))
        {
            if (salvarJogoBinario())
            {
                strcpy(
                    mensagem,
                    "Jogo salvo!"
                );
            }
            else
            {
                strcpy(
                    mensagem,
                    "Erro ao salvar!"
                );
            }

            tempoMensagem = 2.0f;
        }

        if (IsKeyPressed(KEY_F9))
        {
            if (carregarJogoBinario())
            {
                jogador =
                    vetorEntidades[0];

                strcpy(
                    mensagem,
                    "Jogo carregado!"
                );
            }
            else
            {
                strcpy(
                    mensagem,
                    "Nenhum save encontrado!"
                );
            }

            tempoMensagem = 2.0f;
        }

        if (IsKeyPressed(KEY_DELETE))
        {
            if (remove(ARQUIVO_SAVE) == 0)
            {
                strcpy(
                    mensagem,
                    "Save apagado!"
                );
            }
            else
            {
                strcpy(
                    mensagem,
                    "Nenhum save encontrado"
                );
            }

            tempoMensagem = 2.0f;
        }

        ordenarPorDistancia(jogador);

        if (tempoMensagem > 0)
            tempoMensagem -= GetFrameTime();

        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < totalEntidades; i++)
        {
            desenharEntidade(
                vetorEntidades[i]
            );
        }

        DrawText(
            TextFormat(
                "Jogador: %s",
                nomeJogador
            ),
            10,
            10,
            20,
            DARKGRAY
        );

        DrawText(
            TextFormat(
                "Vida: %d   Pontuacao: %d",
                jogador->vida,
                pontuacao
            ),
            10,
            35,
            20,
            DARKGRAY
        );

        DrawText(
            TextFormat(
                "Melhor pontuacao: %d",
                melhorPontuacao
            ),
            10,
            60,
            18,
            GRAY
        );

        DrawText(
            TextFormat(
                "Entidades ativas: %d",
                totalEntidades
            ),
            10,
            85,
            18,
            GRAY
        );

        DrawText(
            "F5: placar | F6: salvar | F9: carregar | DELETE: apagar save",
            10,
            ALTURA_JANELA - 45,
            16,
            GRAY
        );

        DrawText(
            "N: criar item | ESPACO: atirar | ESC: sair",
            10,
            ALTURA_JANELA - 25,
            16,
            GRAY
        );

        if (tempoMensagem > 0)
        {
            DrawText(
                mensagem,
                LARGURA_JANELA / 2 - 100,
                100,
                22,
                RED
            );
        }

        EndDrawing();
    }

    liberarTodasEntidades();

    CloseWindow();

    return 0;
}
