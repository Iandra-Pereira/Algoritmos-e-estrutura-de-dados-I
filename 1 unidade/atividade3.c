#define LARGURA_JANELA 800
#define ALTURA_JANELA 600

typedef struct {
    float dano;
    int alcance;
} DadosArma;

typedef struct {
    int cura;
} DadosPocao;s

typedef struct {
    int absorcao;
} DadosEscudo;

typedef union {
    DadosArma arma;
    DadosPocao pocao;
    DadosEscudo escudo;
} DadosItem;

typedef enum {
    ITEM_ARMA,
    ITEM_POCAO,
    ITEM_ESCUDO
} TipoItem;

typedef struct {
    Vector2 pos;
    float raio;
    TipoItem tipo;
    DadosItem dados;
    bool coletado;
} Item;

typedef struct {
    Vector2 pos;
    float raio;
    int vida;
    float dano;
    int armadura;
} Jogador;

Item *criarItens(int quantidade)
{
    Item *itens = (Item *)malloc(
        quantidade * sizeof(Item)
    );

    if (itens == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < quantidade; i++)
    {
        Item *it = itens + i;

        it->pos = (Vector2){
            GetRandomValue(30, LARGURA_JANELA - 30),
            GetRandomValue(30, ALTURA_JANELA - 30)
        };

        it->raio = 12.0f;
        it->coletado = false;

        it->tipo = (TipoItem)GetRandomValue(
            ITEM_ARMA,
            ITEM_ESCUDO
        );

        if (it->tipo == ITEM_ARMA)
        {
            it->dados.arma.dano =
                (float)GetRandomValue(2, 8);

            it->dados.arma.alcance =
                GetRandomValue(1, 3);
        }

        else if (it->tipo == ITEM_POCAO)
        {
            it->dados.pocao.cura =
                GetRandomValue(10, 30);

            int sorteioVeneno =
                GetRandomValue(0, 9);

            if (sorteioVeneno < 3)
            {
                it->dados.pocao.cura =
                    -it->dados.pocao.cura;
            }
        }

        else if (it->tipo == ITEM_ESCUDO)
        {
            it->dados.escudo.absorcao =
                GetRandomValue(5, 15);
        }
    }

    return itens;
}

void aplicarItem(Jogador *j, Item *item)
{
    switch (item->tipo)
    {
        case ITEM_ARMA:
            j->dano += item->dados.arma.dano;
            break;

        case ITEM_POCAO:
            j->vida += item->dados.pocao.cura;

            if (j->vida < 0)
            {
                j->vida = 0;
            }

            break;

        case ITEM_ESCUDO:
            j->armadura +=
                item->dados.escudo.absorcao;
            break;
    }

    item->coletado = true;
}

void desenharItem(Item *item)
{
    if (item->coletado)
    {
        return;
    }

    Color cor;

    switch (item->tipo)
    {
        case ITEM_ARMA:
            cor = RED;
            break;

        case ITEM_POCAO:

            if (item->dados.pocao.cura < 0)
            {
                cor = PURPLE;
            }
            else
            {
                cor = GREEN;
            }

            break;

        case ITEM_ESCUDO:
            cor = BLUE;
            break;

        default:
            cor = WHITE;
            break;
    }

    DrawCircleV(
        item->pos,
        item->raio,
        cor
    );

    DrawCircleLines(
        (int)item->pos.x,
        (int)item->pos.y,
        item->raio,
        BLACK
    );
}

int main(void)
{
    InitWindow(
        LARGURA_JANELA,
        ALTURA_JANELA,
        "Atividade 3 - Union e Enum"
    );

    SetTargetFPS(60);

    Jogador jogador;

    jogador.pos = (Vector2){
        LARGURA_JANELA / 2,
        ALTURA_JANELA / 2
    };

    jogador.raio = 15;
    jogador.vida = 100;
    jogador.dano = 10;
    jogador.armadura = 0;

    int quantidadeItens = 12;

    Item *itens =
        criarItens(quantidadeItens);

    if (itens == NULL)
    {
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_RIGHT))
        {
            jogador.pos.x += 4;
        }

        if (IsKeyDown(KEY_LEFT))
        {
            jogador.pos.x -= 4;
        }

        if (IsKeyDown(KEY_UP))
        {
            jogador.pos.y -= 4;
        }

        if (IsKeyDown(KEY_DOWN))
        {
            jogador.pos.y += 4;
        }

        if (jogador.pos.x < jogador.raio)
        {
            jogador.pos.x = jogador.raio;
        }

        if (jogador.pos.x >
            LARGURA_JANELA - jogador.raio)
        {
            jogador.pos.x =
                LARGURA_JANELA - jogador.raio;
        }

        if (jogador.pos.y < jogador.raio)
        {
            jogador.pos.y = jogador.raio;
        }

        if (jogador.pos.y >
            ALTURA_JANELA - jogador.raio)
        {
            jogador.pos.y =
                ALTURA_JANELA - jogador.raio;
        }

        for (int i = 0; i < quantidadeItens; i++)
        {
            Item *it = itens + i;

            if (!it->coletado)
            {
                float dx =
                    it->pos.x - jogador.pos.x;

                float dy =
                    it->pos.y - jogador.pos.y;

                float distancia =
                    dx * dx + dy * dy;

                float somaRaios =
                    it->raio + jogador.raio;

                if (distancia <=
                    somaRaios * somaRaios)
                {
                    aplicarItem(
                        &jogador,
                        it
                    );
                }
            }
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < quantidadeItens; i++)
        {
            desenharItem(&itens[i]);
        }

        DrawCircleV(
            jogador.pos,
            jogador.raio,
            DARKGRAY
        );

        DrawText(
            TextFormat(
                "Vida: %d",
                jogador.vida
            ),
            10,
            10,
            20,
            BLACK
        );

        DrawText(
            TextFormat(
                "Dano: %.1f",
                jogador.dano
            ),
            10,
            40,
            20,
            BLACK
        );

        DrawText(
            TextFormat(
                "Armadura: %d",
                jogador.armadura
            ),
            10,
            70,
            20,
            BLACK
        );

        DrawText(
            "Setas: movimentar",
            10,
            ALTURA_JANELA - 30,
            18,
            DARKGRAY
        );

        EndDrawing();
    }

    free(itens);

    CloseWindow();

    return 0;
}
}
