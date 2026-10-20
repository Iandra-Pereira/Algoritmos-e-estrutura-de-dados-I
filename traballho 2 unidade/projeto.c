#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ELEMENTOS 10000

// 1. ENUM: Estado da Busca e Tipo de Dado 
typedef enum {
    TIPO_INT,
    TIPO_FLOAT
} TipoDado;

typedef enum {
    AGUARDANDO,
    EXECUTANDO,
    PAUSADO,
    ENCONTRADO,
    NAO_ENCONTRADO
} EstadoBusca;

// 2. UNION: Suporte a tipos heterogêneos na mesma struct 
typedef union {
    int v_int;
    float v_float;
} ValorUnion;

// 3. STRUCT: Representação do Elemento no array
typedef struct {
    ValorUnion valor;
    TipoDado tipo;
} Elemento;

// STRUCT: Controle e Estatísticas da Busca
typedef struct {
    Elemento *dados;
    int quantidade;
    int alvo;
    int baixo;
    int alto;
    int posicao;
    long comparacoes;
    long sondagens;
    int passo;
    EstadoBusca estado;
    double tempoExecucao; 
} Busca;

// Gerador automático dos 4 arquivos de teste exigidos pelo trabalho (Teclas 1, 2, 3, 4)
static void gerarArquivosDeTesteSeNaoExistirem(void) {
    FILE *fTest = fopen("dados_uniforme.txt", "r");
    if (fTest != NULL) { fclose(fTest); return; }

    // 1. Uniforme
    FILE *f1 = fopen("dados_uniforme.txt", "w");
    for (int i = 1; i <= 500; i++) fprintf(f1, "%d ", i * 10);
    fclose(f1);

    // 2. Não-Uniforme (crescimento irregular)
    FILE *f2 = fopen("dados_nao_uniforme.txt", "w");
    int val = 1;
    for (int i = 0; i < 500; i++) {
        fprintf(f2, "%d ", val);
        val += (i % 5 == 0) ? (i * 8) : 2;
    }
    fclose(f2);

    // 3. Com Repetições
    FILE *f3 = fopen("dados_repetidos.txt", "w");
    for (int i = 0; i < 500; i++) {
        fprintf(f3, "%d ", (i / 20) * 50);
    }
    fclose(f3);

    // 4. Desordenado / Inverso (Cenário de Falha)
    FILE *f4 = fopen("dados_desordenados.txt", "w");
    for (int i = 500; i >= 1; i--) fprintf(f4, "%d ", i * 10);
    fclose(f4);
}

// Carregar dados a partir do arquivo escolhido com validação de ordenação
static int carregarDados(const char *nomeArquivo, Elemento **dados, int *quantidade, char *mensagemErro) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        if (mensagemErro) sprintf(mensagemErro, "Erro ao abrir '%s'.", nomeArquivo);
        return 0;
    }

    Elemento *vetor = (Elemento *)malloc(MAX_ELEMENTOS * sizeof(Elemento));
    if (vetor == NULL) {
        fclose(arquivo);
        if (mensagemErro) sprintf(mensagemErro, "Memoria insuficiente.");
        return 0;
    }

    int val, n = 0;
    while (fscanf(arquivo, "%d", &val) == 1 && n < MAX_ELEMENTOS) {
        vetor[n].valor.v_int = val;
        vetor[n].tipo = TIPO_INT;
        n++;
    }
    fclose(arquivo);

    if (n == 0) {
        free(vetor);
        if (mensagemErro) sprintf(mensagemErro, "Arquivo sem numeros inteiros validos.");
        return 0;
    }

    // Validação de ordenação em ordem crescente
    for (int i = 1; i < n; i++) {
        if (vetor[i].valor.v_int < vetor[i - 1].valor.v_int) {
            free(vetor);
            if (mensagemErro) sprintf(mensagemErro, "ERRO: Dados desordenados! A busca requer ordem crescente.");
            return 0;
        }
    }

    if (*dados != NULL) free(*dados);
    *dados = vetor;
    *quantidade = n;
    if (mensagemErro) mensagemErro[0] = '\0';
    return 1;
}

static void reiniciarBusca(Busca *b) {
    b->baixo = 0;
    b->alto = b->quantidade - 1;
    b->posicao = -1;
    b->comparacoes = 0;
    b->sondagens = 0;
    b->passo = 0;
    b->estado = AGUARDANDO;
    b->tempoExecucao = 0.0;
}

static void iniciarBusca(Busca *b) {
    if (b->estado == AGUARDANDO || b->estado == PAUSADO) {
        b->estado = EXECUTANDO;
    }
}

// Executa um passo do algoritmo contabilizando o tempo exato de processamento
static void proximoPasso(Busca *b) {
    if (b->estado == ENCONTRADO || b->estado == NAO_ENCONTRADO) return;

    if (b->estado == AGUARDANDO) b->estado = PAUSADO;

    double tInicio = GetTime();

    if (b->baixo > b->alto) {
        b->estado = NAO_ENCONTRADO;
        b->tempoExecucao = (GetTime() - tInicio) * 1000.0;
        return;
    }

    b->comparacoes++;
    if (b->alvo < b->dados[b->baixo].valor.v_int || b->alvo > b->dados[b->alto].valor.v_int) {
        b->estado = NAO_ENCONTRADO;
        b->tempoExecucao = (GetTime() - tInicio) * 1000.0;
        return;
    }

    int valorBaixo = b->dados[b->baixo].valor.v_int;
    int valorAlto = b->dados[b->alto].valor.v_int;

    b->comparacoes++;
    if (valorBaixo == valorAlto) {
        b->sondagens++;
        b->passo++;
        b->posicao = b->baixo;
        b->estado = (valorBaixo == b->alvo) ? ENCONTRADO : NAO_ENCONTRADO;
        b->tempoExecucao = (GetTime() - tInicio) * 1000.0;
        return;
    }

    // Cálculo da Interpolação Linear
    long long numerador = ((long long)b->alvo - valorBaixo) * (b->alto - b->baixo);
    long long denominador = (long long)valorAlto - valorBaixo;
    int pos = b->baixo + (int)(numerador / denominador);

    if (pos < b->baixo) pos = b->baixo;
    if (pos > b->alto) pos = b->alto;

    b->posicao = pos;
    b->sondagens++;
    b->passo++;
    int valorPos = b->dados[pos].valor.v_int;

    b->comparacoes++;
    if (valorPos == b->alvo) {
        b->estado = ENCONTRADO;
        b->tempoExecucao = (GetTime() - tInicio) * 1000.0;
        return;
    }

    b->comparacoes++;
    if (valorPos < b->alvo) {
        b->baixo = pos + 1;
    } else {
        b->alto = pos - 1;
    }

    if (b->baixo > b->alto) {
        b->estado = NAO_ENCONTRADO;
    }

    b->tempoExecucao = (GetTime() - tInicio) * 1000.0;
}

static const char *nomeEstado(EstadoBusca estado) {
    switch (estado) {
        case AGUARDANDO: return "Aguardando Inicio";
        case EXECUTANDO: return "Em Execucao Automatica";
        case PAUSADO: return "Pausado (Passo a Passo)";
        case ENCONTRADO: return "Valor Encontrado!";
        case NAO_ENCONTRADO: return "Valor Nao Encontrado";
        default: return "Desconhecido";
    }
}

int main(void) {
    gerarArquivosDeTesteSeNaoExistirem();

    Elemento *dados = NULL;
    int quantidade = 0;
    char arquivoAtual[64] = "dados_uniforme.txt";
    char mensagemErro[128] = "";

    carregarDados(arquivoAtual, &dados, &quantidade, mensagemErro);

    Busca busca = {0};
    busca.dados = dados;
    busca.quantidade = quantidade;
    busca.alvo = (dados != NULL && quantidade > 0) ? dados[0].valor.v_int : 0;
    int indiceAlvo = 0;
    reiniciarBusca(&busca);

    const int largura = 1100, altura = 660;
    InitWindow(largura, altura, "Busca por Interpolacao - AED I (UFERSA)");
    SetTargetFPS(60);

    double ultimoAvanco = GetTime();

    while (!WindowShouldClose()) {
        // --- TROCA DE CENÁRIOS VIA TECLAS 1, 2, 3, 4 ---
        const char *novoArquivo = NULL;
        if (IsKeyPressed(KEY_ONE))   novoArquivo = "dados_uniforme.txt";
        if (IsKeyPressed(KEY_TWO))   novoArquivo = "dados_nao_uniforme.txt";
        if (IsKeyPressed(KEY_THREE)) novoArquivo = "dados_repetidos.txt";
        if (IsKeyPressed(KEY_FOUR))  novoArquivo = "dados_desordenados.txt";

        if (novoArquivo != NULL) {
            if (carregarDados(novoArquivo, &dados, &quantidade, mensagemErro)) {
                snprintf(arquivoAtual, sizeof(arquivoAtual), "%s", novoArquivo);
                busca.dados = dados;
                busca.quantidade = quantidade;
                indiceAlvo = 0;
                busca.alvo = dados[0].valor.v_int;
                reiniciarBusca(&busca);
            }
        }

        if (dados != NULL && quantidade > 0) {
            if (IsKeyPressed(KEY_UP)) {
                if (indiceAlvo < quantidade - 1) indiceAlvo++;
                busca.alvo = dados[indiceAlvo].valor.v_int;
                reiniciarBusca(&busca);
            }
            if (IsKeyPressed(KEY_DOWN)) {
                if (indiceAlvo > 0) indiceAlvo--;
                busca.alvo = dados[indiceAlvo].valor.v_int;
                reiniciarBusca(&busca);
            }

            if (IsKeyPressed(KEY_S)) iniciarBusca(&busca);
            if (IsKeyPressed(KEY_P) && busca.estado == EXECUTANDO) busca.estado = PAUSADO;
            if (IsKeyPressed(KEY_C) && busca.estado == PAUSADO) iniciarBusca(&busca);
            
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_N)) {
                if (busca.estado == EXECUTANDO) busca.estado = PAUSADO;
                if (busca.estado == ENCONTRADO || busca.estado == NAO_ENCONTRADO) reiniciarBusca(&busca);
                proximoPasso(&busca);
            }
            if (IsKeyPressed(KEY_R)) reiniciarBusca(&busca);

            if (busca.estado == EXECUTANDO && GetTime() - ultimoAvanco >= 0.8) {
                proximoPasso(&busca);
                ultimoAvanco = GetTime();
            }
        }

        // --- INTERFACE GRÁFICA ---
        BeginDrawing();
        ClearBackground((Color){246, 248, 251, 255});

        DrawText("ALGORITMO: BUSCA POR INTERPOLACAO", 30, 15, 22, (Color){28, 47, 74, 255});

        // Barra de Seleção de Cenários (Teclas 1, 2, 3, 4)
        DrawRectangleRounded((Rectangle){28, 50, 1044, 32}, 0.2f, 8, (Color){200, 212, 225, 255});
        DrawText("CENARIOS: [1] Uniforme | [2] Nao-Uniforme | [3] Repetidos | [4] Desordenado (Erro)", 38, 58, 14, (Color){28, 47, 74, 255});

        // Barra de Controles
        DrawRectangleRounded((Rectangle){28, 88, 1044, 32}, 0.2f, 8, (Color){222, 230, 240, 255});
        DrawText("CONTROLES: [S] Iniciar | [P] Pausar | [C] Continuar | [ESPAÇO] Passo a Passo | [R] Reiniciar | [SETAS] Alvo", 38, 96, 14, (Color){28, 47, 74, 255});

        // Desenho dos Elementos
        DrawRectangleRounded((Rectangle){28, 126, 1044, 180}, 0.08f, 8, (Color){232, 238, 245, 255});

        if (mensagemErro[0] != '\0') {
            DrawText(mensagemErro, 48, 195, 18, MAROON);
            DrawText("Pressione [1], [2] ou [3] para carregar um cenario com dados validos.", 48, 225, 15, DARKGRAY);
        } else if (busca.quantidade > 0) {
            int inicio = busca.baixo;
            int fim = busca.alto;
            if (inicio < 0) inicio = 0;
            if (fim >= busca.quantidade) fim = busca.quantidade - 1;

            int visiveis = fim - inicio + 1;
            if (visiveis > 22) {
                inicio = busca.posicao >= 0 ? busca.posicao - 11 : inicio;
                if (inicio < busca.baixo) inicio = busca.baixo;
                fim = inicio + 21;
                if (fim > busca.alto) {
                    fim = busca.alto;
                    inicio = fim - 21;
                    if (inicio < busca.baixo) inicio = busca.baixo;
                }
            }
            int vis = fim - inicio + 1;
            int larguraCelula = 980 / (vis > 0 ? vis : 1);

            Vector2 mousePos = GetMousePosition();

            for (int i = inicio; i <= fim; i++) {
                int x = 60 + (i - inicio) * larguraCelula;
                Rectangle rectBarra = (Rectangle){ (float)x, 180.0f, (float)larguraCelula - 4, 45.0f };

                if (CheckCollisionPointRec(mousePos, rectBarra) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    busca.alvo = dados[i].valor.v_int;
                    indiceAlvo = i;
                    reiniciarBusca(&busca);
                }

                Color cor = (Color){194, 207, 222, 255};
                if (i == busca.baixo || i == busca.alto) cor = (Color){105, 174, 220, 255};
                if (i == busca.posicao) cor = (Color){242, 170, 65, 255};
                if (i == busca.posicao && busca.estado == ENCONTRADO) cor = (Color){74, 174, 112, 255};
                if (i == busca.posicao && busca.estado == NAO_ENCONTRADO) cor = (Color){219, 92, 92, 255};

                DrawRectangleRec(rectBarra, cor);
                DrawRectangleLines((int)rectBarra.x, (int)rectBarra.y, (int)rectBarra.width, (int)rectBarra.height, (Color){100, 115, 130, 255});
                DrawText(TextFormat("%d", dados[i].valor.v_int), x + 4, 193, 14, BLACK);
                DrawText(TextFormat("[%d]", i), x + 4, 230, 11, DARKGRAY);
            }
            DrawText("Legenda: Azul (Limites baixo/alto) | Laranja (Posicao testada pos) | Verde (Encontrado)", 48, 275, 13, DARKGRAY);
        }

        // Estatísticas
        DrawRectangleRounded((Rectangle){28, 315, 1044, 305}, 0.08f, 8, (Color){232, 238, 245, 255});
        DrawText("Estatisticas da Execucao Real", 48, 330, 20, (Color){28, 47, 74, 255});

        if (mensagemErro[0] == '\0') {
            DrawText(TextFormat("Elementos (N): %d", busca.quantidade), 50, 370, 18, DARKGRAY);
            DrawText(TextFormat("Alvo Selecionado: %d", busca.alvo), 400, 370, 18, (Color){28, 47, 74, 255});
            DrawText(TextFormat("Comparacoes: %ld", busca.comparacoes), 50, 405, 18, DARKGRAY);
            DrawText(TextFormat("Sondagens: %ld", busca.sondagens), 50, 440, 18, DARKGRAY);
            DrawText(TextFormat("Passo Atual: %d", busca.passo), 400, 440, 18, DARKGRAY);
            
            // Exibição do Tempo de Execução Real
            DrawText(TextFormat("Tempo de Execucao: %.6f ms (%.2f us)", busca.tempoExecucao, busca.tempoExecucao * 1000.0), 50, 480, 19, (Color){28, 47, 74, 255});
            DrawText(TextFormat("Estado da Aplicacao: %s", nomeEstado(busca.estado)), 50, 515, 18, DARKGRAY);

            if (busca.estado == ENCONTRADO) {
                DrawText(TextFormat("RESULTADO: O valor %d foi ENCONTRADO no indice %d do vetor!", busca.alvo, busca.posicao), 50, 555, 18, (Color){32, 125, 70, 255});
            } else if (busca.estado == NAO_ENCONTRADO) {
                DrawText(TextFormat("RESULTADO: O valor %d NAO existe no conjunto de dados.", busca.alvo), 50, 555, 18, MAROON);
            } else {
                DrawText("Aperte [ESPAÇO] para avançar passo a passo.", 50, 555, 16, (Color){28, 47, 74, 255});
            }
        }

        EndDrawing();
    }

    CloseWindow();
    free(dados);
    return 0;
}
