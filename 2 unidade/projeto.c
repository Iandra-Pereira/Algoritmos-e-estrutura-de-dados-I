#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    ESTADO_PARADO,
    ESTADO_EXECUTANDO,
    ESTADO_PAUSADO,
    ESTADO_ENCONTRADO,
    ESTADO_NAO_ENCONTRADO
} EstadoSimulacao;

typedef enum {
    TIPO_INTEIRO
} TipoDado;

typedef union {
    int v_int;
    float v_float;
} ValorElemento;

typedef struct {
    ValorElemento valor;
    TipoDado tipo;
    Color cor;
} Elemento;

typedef struct {
    int total_elementos;
    long comparacoes;
    long passo;
    int alvo;
    int low;
    int high;
    int pos;
} EstatisticasBusca;

// Carrega os dados de um arquivo de texto
int carregarDadosDeArquivo(const char *nomeArquivo, Elemento **array) {
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) return 0;

    int n;
    if (fscanf(file, "%d", &n) != 1) {
        fclose(file);
        return 0;
    }

    *array = (Elemento *)malloc(n * sizeof(Elemento));
    for (int i = 0; i < n; i++) {
        int val;
        fscanf(file, "%d", &val);
        (*array)[i].valor.v_int = val;
        (*array)[i].tipo = TIPO_INTEIRO;
        (*array)[i].cor = LIGHTGRAY;
    }

    fclose(file);
    return n;
}

// Reseta o estado da busca mantendo o array e atualizando o alvo
void reiniciarBusca(Elemento *arr, EstatisticasBusca *est, EstadoSimulacao *estado) {
    est->comparacoes = 0;
    est->passo = 0;
    est->low = 0;
    est->high = est->total_elementos - 1;
    est->pos = -1;
    *estado = ESTADO_PARADO;

    for (int k = 0; k < est->total_elementos; k++) {
        arr[k].cor = LIGHTGRAY;
    }
}

// Executa exatamente um passo da Busca por Interpolação
void passoBuscaInterpolacao(Elemento *arr, EstatisticasBusca *est, EstadoSimulacao *estado) {
    if (*estado == ESTADO_ENCONTRADO || *estado == ESTADO_NAO_ENCONTRADO) return;

    // Verificar se o valor está fora dos limites atuais do intervalo
    if (est->low > est->high || est->alvo < arr[est->low].valor.v_int || est->alvo > arr[est->high].valor.v_int) {
        *estado = ESTADO_NAO_ENCONTRADO;
        return;
    }

    est->comparacoes++;
    est->passo++;

    // Resetar visualização anterior
    for (int k = 0; k < est->total_elementos; k++) arr[k].cor = LIGHTGRAY;

    // Destacar o intervalo atual [low .. high]
    for (int k = est->low; k <= est->high; k++) arr[k].cor = GRAY;

    // Caso de intervalo com elementos de valores iguais
    if (arr[est->high].valor.v_int == arr[est->low].valor.v_int) {
        if (arr[est->low].valor.v_int == est->alvo) {
            arr[est->low].cor = GREEN;
            *estado = ESTADO_ENCONTRADO;
        } else {
            *estado = ESTADO_NAO_ENCONTRADO;
        }
        return;
    }

    // Fórmula da Busca por Interpolação
    est->pos = est->low + (int)(((double)(est->high - est->low) / 
               (arr[est->high].valor.v_int - arr[est->low].valor.v_int)) * 
               (est->alvo - arr[est->low].valor.v_int));

    // Validar limites da posição calculada
    if (est->pos < est->low || est->pos > est->high) {
        *estado = ESTADO_NAO_ENCONTRADO;
        return;
    }

    // Destacar visualmente os marcadores
    arr[est->low].cor = ORANGE;   // Limite inferior
    arr[est->high].cor = ORANGE;  // Limite superior
    arr[est->pos].cor = RED;       // Posição testada no passo atual

    // Testar se o elemento foi encontrado ou se deve ajustar o intervalo
    if (arr[est->pos].valor.v_int == est->alvo) {
        arr[est->pos].cor = GREEN;
        *estado = ESTADO_ENCONTRADO;
    } else if (arr[est->pos].valor.v_int < est->alvo) {
        est->low = est->pos + 1;
    } else {
        est->high = est->pos - 1;
    }
}

int main(void) {
    const int larguraTela = 1000;
    const int alturaTela = 600;
    InitWindow(larguraTela, alturaTela, "UFERSA - Busca por Interpolacao");

    Elemento *array = NULL;
    int n = carregarDadosDeArquivo("entrada.txt", &array);

    // Vetor de contingência ordenado se o arquivo não estiver presente
    if (n == 0) {
        n = 100; //AQUIIIIIIIIIIIIII
        array = (Elemento *)malloc(n * sizeof(Elemento));
        for (int k = 0; k < n; k++) {
            array[k].valor.v_int = (k + 1) * 10;
            array[k].tipo = TIPO_INTEIRO;
            array[k].cor = LIGHTGRAY;
        }
    }

    EstatisticasBusca est;
    est.total_elementos = n;
    est.alvo = array[n / 2].valor.v_int; // Alvo inicial: elemento central
    EstadoSimulacao estado = ESTADO_PARADO;

    reiniciarBusca(array, &est, &estado);
    SetTargetFPS(2); // Velocidade reduzida para acompanhar os passos na tela

    while (!WindowShouldClose()) {
        // --- SELEÇÃO DO ALVO PROCURADO ---
        if (estado == ESTADO_PARADO || estado == ESTADO_PAUSADO) {
            if (IsKeyPressed(KEY_UP)) {
                est.alvo += 5;
                reiniciarBusca(array, &est, &estado);
            }
            if (IsKeyPressed(KEY_DOWN)) {
                est.alvo -= 5;
                reiniciarBusca(array, &est, &estado);
            }
            // Seleção por clique do rato na barra desejada
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                float larguraBarra = (float)larguraTela / n;
                int idx = (int)(mousePos.x / larguraBarra);
                if (idx >= 0 && idx < n) {
                    est.alvo = array[idx].valor.v_int;
                    reiniciarBusca(array, &est, &estado);
                }
            }
        }

        // --- CONTROLES DA SIMULAÇÃO ---
        if (IsKeyPressed(KEY_SPACE)) {
            if (estado == ESTADO_EXECUTANDO) {
                estado = ESTADO_PAUSADO;
            } else if (estado == ESTADO_ENCONTRADO || estado == ESTADO_NAO_ENCONTRADO) {
                reiniciarBusca(array, &est, &estado);
                estado = ESTADO_EXECUTANDO;
            } else {
                estado = ESTADO_EXECUTANDO;
            }
        }

        if (IsKeyPressed(KEY_N)) { // Passo a passo
            if (estado == ESTADO_PARADO) estado = ESTADO_PAUSADO;
            passoBuscaInterpolacao(array, &est, &estado);
        }

        if (IsKeyPressed(KEY_R)) { // Reiniciar
            reiniciarBusca(array, &est, &estado);
        }

        // --- ATUALIZAÇÃO ---
        if (estado == ESTADO_EXECUTANDO) {
            passoBuscaInterpolacao(array, &est, &estado);
        }

        // --- DESENHO GRÁFICO ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        float larguraBarra = (float)larguraTela / n;
        int maxVal = array[n - 1].valor.v_int;

        for (int k = 0; k < n; k++) {
            float alturaBarra = ((float)array[k].valor.v_int / maxVal) * 320.0f;
            DrawRectangleV(
                (Vector2){ k * larguraBarra, alturaTela - alturaBarra - 30 },
                (Vector2){ larguraBarra - 2, alturaBarra },
                array[k].cor
            );
        }

        // PAINEL DE ESTATÍSTICAS
        DrawRectangle(10, 10, 420, 180, Fade(SKYBLUE, 0.7f));
        DrawRectangleLines(10, 10, 420, 180, BLUE);

        DrawText("Algoritmo: Busca por Interpolacao", 20, 20, 18, DARKBLUE);
        DrawText(TextFormat("Elementos (N): %d", est.total_elementos), 20, 45, 15, BLACK);
        DrawText(TextFormat("Alvo Procurado: %d (Seta Cima/Baixo ou Clique)", est.alvo), 20, 65, 15, DARKBLUE);
        DrawText(TextFormat("Intervalo [low, high]: [%d, %d]", est.low, est.high), 20, 85, 15, BLACK);
        DrawText(TextFormat("Posicao Testada (pos): %d", est.pos), 20, 105, 15, BLACK);
        DrawText(TextFormat("Comparacoes: %ld | Passo: %ld", est.comparacoes, est.passo), 20, 125, 15, BLACK);

        if (estado == ESTADO_ENCONTRADO) {
            DrawText("Status: ELEMENTO ENCONTRADO!", 20, 150, 16, GREEN);
        } else if (estado == ESTADO_NAO_ENCONTRADO) {
            DrawText("Status: ELEMENTO NAO ENCONTRADO!", 20, 150, 16, RED);
        } else if (estado == ESTADO_EXECUTANDO) {
            DrawText("Status: EM EXECUCAO...", 20, 150, 16, ORANGE);
        } else {
            DrawText("Status: PAUSADO / AGUARDANDO", 20, 150, 16, DARKGRAY);
        }

        DrawText("Controles: [ESPAÇO] Executar/Pausar | [N] Proximo Passo | [R] Reiniciar", 10, alturaTela - 25, 15, DARKGRAY);

        EndDrawing();
    }

    free(array);
    CloseWindow();
    return 0;
}
