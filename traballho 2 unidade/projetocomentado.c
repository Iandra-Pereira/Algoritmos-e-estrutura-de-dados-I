#include "raylib.h" // Inclui a biblioteca Raylib para criação da janela e interface gráfica
#include <stdio.h>  // Inclui a biblioteca padrão de I/O para manipulacao de arquivos (fopen, fscanf)
#include <stdlib.h> // Inclui a biblioteca padrão para alocação dinâmica de memória (malloc, free)
#include <time.h>   // Inclui a biblioteca de tempo do C para manipulacao de cronômetros e medições

#define MAX_ELEMENTOS 10000 // Define a constante do tamanho máximo de elementos suportados no vetor

// =========================================================================================
// 1. ESTRUTURAS DE DADOS E ENUMERAÇÕES (Atendimento aos Requisitos Acadêmicos)
// =========================================================================================

// Enumeração para identificar o tipo de dado armazenado (Inteiro ou Ponto Flutuante)
typedef enum {
    TIPO_INT,   // Representa o tipo de dado inteiro (0)
    TIPO_FLOAT  // Representa o tipo de dado ponto flutuante (1)
} TipoDado;

// Enumeração para a Máquina de Estados Finitos que controla a simulação gráfica
typedef enum {
    AGUARDANDO,     // Estado inicial: aguardando o usuário iniciar a busca (0)
    EXECUTANDO,     // Estado ativo: busca rodando automaticamente a cada intervalo (1)
    PAUSADO,        // Estado de pausa: controle passo a passo habilitado (2)
    ENCONTRADO,     // Estado final de sucesso: elemento localizado no vetor (3)
    NAO_ENCONTRADO  // Estado final de falha: elemento provado ausente no vetor (4)
} EstadoBusca;

// União C para permitir armazenamento heterogêneo sem desperdício de memória
typedef union {
    int v_int;      // Memória compartilhada interpretada como número inteiro
    float v_float;  // Memória compartilhada interpretada como número de ponto flutuante
} ValorUnion;

// Estrutura do Elemento individual que compõe o array carregado do arquivo
typedef struct {
    ValorUnion valor; // Campo com a união contendo o valor numérico
    TipoDado tipo;    // Flag indicando o tipo armazenado na união
} Elemento;

// Estrutura principal que mantém todo o contexto e estatísticas da busca
typedef struct {
    Elemento *dados;      // Ponteiro para o array dinâmico na memória Heap
    int quantidade;       // Número total de elementos válidos carregados (N)
    int alvo;             // Chave numérica que o algoritmo está procurando
    int baixo;            // Ponteiro/Índice inferior do intervalo ativo (low)
    int alto;             // Ponteiro/Índice superior do intervalo ativo (high)
    int posicao;          // Índice estimado calculado pela fórmula de interpolação (pos)
    long comparacoes;     // Contador total de comparações lógicas efetuadas
    long sondagens;       // Contador de acessos/sondagens ao array
    int passo;            // Contador do número de iterações executadas
    EstadoBusca estado;   // Estado atual da Máquina de Estados Finitos
    double tempoExecucao; // Tempo de processamento do algoritmo medido em milissegundos
} Busca;

// =========================================================================================
// 2. MANIPULAÇÃO DE ARQUIVOS E VALIDAÇÃO DE DADOS
// Complexidade de Espaço Global dos Dados: O(N) na memória Heap.
// =========================================================================================

// Função que gera automaticamente os arquivos texto de teste caso não existam
// Complexidade de Tempo: O(N) para criar e escrever N elementos nos arquivos texto.
// Complexidade de Espaço: O(1) de memória secundária por arquivo gerado.
static void gerarArquivosDeTesteSeNaoExistirem(void) {
    FILE *fTest = fopen("dados_uniforme.txt", "r"); // Tenta abrir o arquivo para leitura
    if (fTest != NULL) { fclose(fTest); return; }   // Se o arquivo já existe, fecha e retorna

    // Cenário 1: Dados perfeitamente uniformes (Distribuição ideal com melhor caso O(log log N))
    FILE *f1 = fopen("dados_uniforme.txt", "w"); // Abre/Cria o arquivo uniforme para escrita
    for (int i = 1; i <= 500; i++) fprintf(f1, "%d ", i * 10); // Escreve múltiplos de 10
    fclose(f1); // Fecha o ponteiro do arquivo 1

    // Cenário 2: Dados desuniformes com crescimento não-linear (Testa pior alinhamento da reta)
    FILE *f2 = fopen("dados_nao_uniforme.txt", "w"); // Abre o arquivo desuniforme para escrita
    int val = 1; // Inicializa o valor base
    for (int i = 0; i < 500; i++) { // Laço para gerar 500 elementos com saltos variáveis
        fprintf(f2, "%d ", val); // Escreve o valor no arquivo
        val += (i % 5 == 0) ? (i * 8) : 2; // Aplica incremento não-linear
    }
    fclose(f2); // Fecha o ponteiro do arquivo 2

    // Cenário 3: Dados com elementos repetidos (Testa prevenção contra divisão por zero)
    FILE *f3 = fopen("dados_repetidos.txt", "w"); // Abre o arquivo de repetidos para escrita
    for (int i = 0; i < 500; i++) { // Laço de repetição
        fprintf(f3, "%d ", (i / 20) * 50); // Escreve blocos de 20 números idênticos
    }
    fclose(f3); // Fecha o ponteiro do arquivo 3

    // Cenário 4: Dados desordenados (Testa o mecanismo de rejeição e validação do programa)
    FILE *f4 = fopen("dados_desordenados.txt", "w"); // Abre o arquivo desordenado para escrita
    for (int i = 500; i >= 1; i--) fprintf(f4, "%d ", i * 10); // Escreve em ordem decrescente
    fclose(f4); // Fecha o ponteiro do arquivo 4
}

// Carrega os números do arquivo, aloca memória e valida a ordenação prévia
// Complexidade de Tempo: O(N) — Varredura linear para leitura e validação da ordenação.
// Complexidade de Espaço: O(N) — Alocação dinâmica de N elementos do tipo Elemento.
static int carregarDados(const char *nomeArquivo, Elemento **dados, int *quantidade, char *mensagemErro) {
    FILE *arquivo = fopen(nomeArquivo, "r"); // Tenta abrir o arquivo informado
    if (arquivo == NULL) { // Verifica se houve falha na abertura
        if (mensagemErro) sprintf(mensagemErro, "Erro ao abrir '%s'.", nomeArquivo); // Registra mensagem
        return 0; // Retorna código de erro
    }

    Elemento *vetor = (Elemento *)malloc(MAX_ELEMENTOS * sizeof(Elemento)); // Aloca memória na Heap
    if (vetor == NULL) { // Verifica se a alocação de memória falhou
        fclose(arquivo); // Fecha o arquivo aberto
        if (mensagemErro) sprintf(mensagemErro, "Memoria insuficiente."); // Registra mensagem
        return 0; // Retorna código de erro
    }

    int val, n = 0; // Variável auxiliar de valor e contador de elementos
    while (fscanf(arquivo, "%d", &val) == 1 && n < MAX_ELEMENTOS) { // Lê inteiros do arquivo
        vetor[n].valor.v_int = val; // Armazena o valor lido no membro inteiro da união
        vetor[n].tipo = TIPO_INT;    // Define o tipo armazenado como TIPO_INT
        n++; // Incrementa a contagem de elementos lidos
    }
    fclose(arquivo); // Fecha o arquivo após concluir a leitura

    if (n == 0) { // Verifica se o arquivo estava vazio
        free(vetor); // Libera o bloco de memória alocado
        if (mensagemErro) sprintf(mensagemErro, "Arquivo sem numeros inteiros validos."); // Registra erro
        return 0; // Retorna falha
    }

    // Validação estrita de ordenação (Verificação de Monotonicidade Crescentes)
    // O(N) de tempo de execução
    for (int i = 1; i < n; i++) { // Percorre a partir do segundo elemento
        if (vetor[i].valor.v_int < vetor[i - 1].valor.v_int) { // Compara com o elemento anterior
            free(vetor); // Desaloca memória se a sequência for desordenada
            if (mensagemErro) sprintf(mensagemErro, "ERRO: Dados desordenados! A busca requer ordem crescente."); // Erro
            return 0; // Cancela o carregamento
        }
    }

    if (*dados != NULL) free(*dados); // Se já existia um vetor antigo na memória, des-aloca
    *dados = vetor;       // Atualiza o ponteiro de dados do sistema para a nova memória
    *quantidade = n;      // Atualiza o número total de elementos válidos
    if (mensagemErro) mensagemErro[0] = '\0'; // Limpa mensagens de erro anteriores
    return 1; // Sucesso
}

// Reseta a estrutura de controle da busca para os valores iniciais
// Complexidade de Tempo: O(1)
// Complexidade de Espaço: O(1)
static void reiniciarBusca(Busca *b) {
    b->baixo = 0;                  // Define o limite inferior no primeiro índice (0)
    b->alto = b->quantidade - 1;   // Define o limite superior no último índice (N - 1)
    b->posicao = -1;               // Reseta o índice estimado para inválido (-1)
    b->comparacoes = 0;            // Zera o contador de comparações
    b->sondagens = 0;              // Zera o contador de sondagens
    b->passo = 0;                  // Zera o contador do passo
    b->estado = AGUARDANDO;        // Seta o estado da máquina de estados para AGUARDANDO
    b->tempoExecucao = 0.0;        // Zera a medição do tempo de processamento
}

// Ativa a execução automática da simulação
// Complexidade de Tempo: O(1)
// Complexidade de Espaço: O(1)
static void iniciarBusca(Busca *b) {
    if (b->estado == AGUARDANDO || b->estado == PAUSADO) { // Se estiver aguardando ou pausado
        b->estado = EXECUTANDO; // Altera o estado para execução automática
    }
}

// =========================================================================================
// 3. CORE DO ALGORITMO: BUSCA POR INTERPOLAÇÃO (Passo a Passo)
// Complexidade de Tempo por Passo: O(1)
// Complexidade de Tempo Total do Algoritmo:
//   - Melhor Caso / Caso Médio (Dados Uniformes): O(log log N)
//   - Pior Caso (Dados Não-Uniformes / Exponenciais): O(N)
// Complexidade de Espaço Auxiliar: O(1) — Apenas variáveis locais e ponteiros.
// =========================================================================================

static void proximoPasso(Busca *b) {
    if (b->estado == ENCONTRADO || b->estado == NAO_ENCONTRADO) return; // Retorna se já finalizou

    if (b->estado == AGUARDANDO) b->estado = PAUSADO; // Se manual, passa para pausado

    double tInicio = GetTime(); // Registra o timestamp inicial de alta precisão via Raylib

    // Condição de término 1: Intervalo esgotado (baixo ultrapassou alto)
    if (b->baixo > b->alto) {
        b->estado = NAO_ENCONTRADO; // Define falha na busca
        b->tempoExecucao = (GetTime() - tInicio) * 1000.0; // Converte segundos para ms
        return;
    }

    // Condição de término 2: Chave fora dos limites dos valores atualizados
    b->comparacoes++; // Incrementa comparação lógica
    if (b->alvo < b->dados[b->baixo].valor.v_int || b->alvo > b->dados[b->alto].valor.v_int) {
        b->estado = NAO_ENCONTRADO; // Alvo fora da faixa possível
        b->tempoExecucao = (GetTime() - tInicio) * 1000.0; // Registra tempo de execução em ms
        return;
    }

    int valorBaixo = b->dados[b->baixo].valor.v_int; // Extrai o valor inteiro no limite baixo
    int valorAlto = b->dados[b->alto].valor.v_int;   // Extrai o valor inteiro no limite alto

    // Prevenção contra Divisão por Zero em subarrays com elementos idênticos
    b->comparacoes++; // Incrementa comparação
    if (valorBaixo == valorAlto) {
        b->sondagens++; // Registra sondagem ao vetor
        b->passo++;     // Avança o passo
        b->posicao = b->baixo; // Define a posição testada
        b->estado = (valorBaixo == b->alvo) ? ENCONTRADO : NAO_ENCONTRADO; // Avalia busca
        b->tempoExecucao = (GetTime() - tInicio) * 1000.0; // Converte para milissegundos
        return;
    }

    // Aplicação da Fórmula da Interpolação Linear com casting long long (Prevenção de Overflow)
    long long numerador = ((long long)b->alvo - valorBaixo) * (b->alto - b->baixo); // Numerador
    long long denominador = (long long)valorAlto - valorBaixo; // Denominador
    int pos = b->baixo + (int)(numerador / denominador); // Cálculo da posição pos

    // Proteção de Limites (Clamp) para impedir acessos desalinhados de memória (Out of Bounds)
    if (pos < b->baixo) pos = b->baixo; // Força pos >= baixo
    if (pos > b->alto) pos = b->alto;   // Força pos <= alto

    b->posicao = pos; // Atualiza a posição calculada na estrutura
    b->sondagens++;   // Incrementa a contagem de sondagens
    b->passo++;       // Incrementa o passo iterativo
    int valorPos = b->dados[pos].valor.v_int; // Lê o valor armazenado na posição estimada

    // Avaliação 1: Sucesso se a estimativa acertar o elemento alvo
    b->comparacoes++; // Incrementa comparação
    if (valorPos == b->alvo) {
        b->estado = ENCONTRADO; // Marca estado como ENCONTRADO
        b->tempoExecucao = (GetTime() - tInicio) * 1000.0; // Armazena tempo do cálculo
        return;
    }

    // Avaliação 2: Ajuste dos limites da busca baseado na comparação
    b->comparacoes++; // Incrementa comparação
    if (valorPos < b->alvo) {
        b->baixo = pos + 1; // Desloca limite inferior para a direita da posição
    } else {
        b->alto = pos - 1;  // Desloca limite superior para a esquerda da posição
    }

    // Validação secundária do intervalo reajustado
    if (b->baixo > b->alto) {
        b->estado = NAO_ENCONTRADO; // Marca estado como NÃO ENCONTRADO se esgotado
    }

    b->tempoExecucao = (GetTime() - tInicio) * 1000.0; // Finaliza cálculo do tempo do passo
}

// Mapeia a constante do Enum para uma string para renderização na interface
// Complexidade de Tempo: O(1)
// Complexidade de Espaço: O(1)
static const char *nomeEstado(EstadoBusca estado) {
    switch (estado) {
        case AGUARDANDO: return "Aguardando Inicio"; // Texto explicativo para estado AGUARDANDO
        case EXECUTANDO: return "Em Execucao Automatica"; // Texto para estado EXECUTANDO
        case PAUSADO: return "Pausado (Passo a Passo)";   // Texto para estado PAUSADO
        case ENCONTRADO: return "Valor Encontrado!";      // Texto para estado ENCONTRADO
        case NAO_ENCONTRADO: return "Valor Nao Encontrado"; // Texto para estado NAO_ENCONTRADO
        default: return "Desconhecido"; // Default de segurança
    }
}

// =========================================================================================
// 4. FUNÇÃO PRINCIPAL E LOOP GRÁFICO (Raylib)
// Complexidade de Tempo da Renderização: O(V) por frame, onde V é o número de barras visíveis.
// Complexidade de Espaço do Loop Gráfico: O(1)
// =========================================================================================

int main(void) {
    gerarArquivosDeTesteSeNaoExistirem(); // Cria os arquivos texto de teste se ausentes

    Elemento *dados = NULL; // Ponteiro inicial para o vetor de dados
    int quantidade = 0;     // Variável contendo o número total de elementos
    char arquivoAtual[64] = "dados_uniforme.txt"; // String com o nome do arquivo ativo
    char mensagemErro[128] = ""; // Buffer para mensagens de erro no carregamento

    carregarDados(arquivoAtual, &dados, &quantidade, mensagemErro); // Tenta carregar dados iniciais

    Busca busca = {0}; // Inicializa a struct de busca com zeros
    busca.dados = dados; // Atribui o vetor de dados
    busca.quantidade = quantidade; // Atribui a quantidade de elementos
    busca.alvo = (dados != NULL && quantidade > 0) ? dados[0].valor.v_int : 0; // Alvo inicial
    int indiceAlvo = 0; // Índice de seleção do alvo pelas setas
    reiniciarBusca(&busca); // Inicializa o estado de busca

    const int largura = 1100, altura = 660; // Define as dimensões da janela em pixels
    InitWindow(largura, altura, "Busca por Interpolacao - AED I (UFERSA)"); // Inicializa janela
    SetTargetFPS(60); // Define a taxa de atualização para 60 Quadros Por Segundo (FPS)

    double ultimoAvanco = GetTime(); // Marca o tempo do último avanço automático de passo

    while (!WindowShouldClose()) { // Loop executado a cada frame enquanto a janela permanecer aberta
        
        // --- Processamento de Comandos de Teclado (Troca de Arquivos e Cenários) ---
        const char *novoArquivo = NULL; // Ponteiro para o nome do novo arquivo solicitado
        if (IsKeyPressed(KEY_ONE))   novoArquivo = "dados_uniforme.txt";     // Tecla 1: Uniforme
        if (IsKeyPressed(KEY_TWO))   novoArquivo = "dados_nao_uniforme.txt"; // Tecla 2: Não-uniforme
        if (IsKeyPressed(KEY_THREE)) novoArquivo = "dados_repetidos.txt";    // Tecla 3: Repetidos
        if (IsKeyPressed(KEY_FOUR))  novoArquivo = "dados_desordenados.txt"; // Tecla 4: Desordenado

        if (novoArquivo != NULL) { // Se o usuário pressionou uma das teclas de arquivo
            if (carregarDados(novoArquivo, &dados, &quantidade, mensagemErro)) { // Carrega novo arquivo
                snprintf(arquivoAtual, sizeof(arquivoAtual), "%s", novoArquivo); // Atualiza nome
                busca.dados = dados; // Atualiza ponteiro na struct de busca
                busca.quantidade = quantidade; // Atualiza a contagem N
                indiceAlvo = 0; // Reseta índice
                busca.alvo = dados[0].valor.v_int; // Define novo alvo inicial
                reiniciarBusca(&busca); // Reinicia os ponteiros de busca
            }
        }

        // --- Controles Interativos da Simulação ---
        if (dados != NULL && quantidade > 0) { // Se houver dados válidos carregados
            if (IsKeyPressed(KEY_UP)) { // Seta para cima: seleciona próximo elemento como alvo
                if (indiceAlvo < quantidade - 1) indiceAlvo++; // Incrementa índice
                busca.alvo = dados[indiceAlvo].valor.v_int; // Define valor como alvo
                reiniciarBusca(&busca); // Reinicia a simulação
            }
            if (IsKeyPressed(KEY_DOWN)) { // Seta para baixo: seleciona elemento anterior
                if (indiceAlvo > 0) indiceAlvo--; // Decrementa índice
                busca.alvo = dados[indiceAlvo].valor.v_int; // Define valor como alvo
                reiniciarBusca(&busca); // Reinicia a simulação
            }

            if (IsKeyPressed(KEY_S)) iniciarBusca(&busca); // Tecla S: Inicia simulação automática
            if (IsKeyPressed(KEY_P) && busca.estado == EXECUTANDO) busca.estado = PAUSADO; // Tecla P: Pausa
            if (IsKeyPressed(KEY_C) && busca.estado == PAUSADO) iniciarBusca(&busca);    // Tecla C: Continua
            
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_N)) { // ESPAÇO ou N: Avanço passo a passo
                if (busca.estado == EXECUTANDO) busca.estado = PAUSADO; // Troca para modo pausado
                if (busca.estado == ENCONTRADO || busca.estado == NAO_ENCONTRADO) reiniciarBusca(&busca); // Reseta se fim
                proximoPasso(&busca); // Executa um único passo
            }
            if (IsKeyPressed(KEY_R)) reiniciarBusca(&busca); // Tecla R: Reinicia a busca

            // Timer do avanço automático no modo EXECUTANDO (Avança a cada 0.8 segundos)
            if (busca.estado == EXECUTANDO && GetTime() - ultimoAvanco >= 0.8) {
                proximoPasso(&busca); // Executa passo
                ultimoAvanco = GetTime(); // Atualiza tempo do último avanço
            }
        }

        // --- Renderização Gráfica do Quadro (BeginDrawing/EndDrawing) ---
        BeginDrawing(); // Inicializa renderização do quadro
        ClearBackground((Color){246, 248, 251, 255}); // Limpa a tela com tom de azul/cinza suave

        DrawText("ALGORITMO: BUSCA POR INTERPOLACAO", 30, 15, 22, (Color){28, 47, 74, 255}); // Desenha título

        // Desenha a Barra Superior de Seleção de Cenários
        DrawRectangleRounded((Rectangle){28, 50, 1044, 32}, 0.2f, 8, (Color){200, 212, 225, 255}); // Retângulo
        DrawText("CENARIOS: [1] Uniforme | [2] Nao-Uniforme | [3] Repetidos | [4] Desordenado (Erro)", 38, 58, 14, (Color){28, 47, 74, 255}); // Texto do menu

        // Desenha a Barra de Comandos do Teclado
        DrawRectangleRounded((Rectangle){28, 88, 1044, 32}, 0.2f, 8, (Color){222, 230, 240, 255}); // Retângulo
        DrawText("CONTROLES: [S] Iniciar | [P] Pausar | [C] Continuar | [ESPAÇO] Passo a Passo | [R] Reiniciar | [SETAS] Alvo", 38, 96, 14, (Color){28, 47, 74, 255}); // Texto de controles

        // Desenha a Área Principal de Exibição das Barras/Vetor
        DrawRectangleRounded((Rectangle){28, 126, 1044, 180}, 0.08f, 8, (Color){232, 238, 245, 255}); // Contêiner

        if (mensagemErro[0] != '\0') { // Se existir mensagem de erro ativada (ex: arquivo desordenado)
            DrawText(mensagemErro, 48, 195, 18, MAROON); // Exibe mensagem em tom vermelho escuro
            DrawText("Pressione [1], [2] ou [3] para carregar um cenario com dados validos.", 48, 225, 15, DARKGRAY); // Instrução
        } else if (busca.quantidade > 0) { // Se houver dados válidos
            // Paginação Visual para cálculo da janela adaptativa de células
            int inicio = busca.baixo; // Janela inicia em baixo
            int fim = busca.alto;     // Janela termina em alto
            if (inicio < 0) inicio = 0; // Limita índice
            if (fim >= busca.quantidade) fim = busca.quantidade - 1; // Limita índice

            int visiveis = fim - inicio + 1; // Número de elementos na janela ativa
            if (visiveis > 22) { // Se a janela tiver mais de 22 elementos, ajusta visibilidade
                inicio = busca.posicao >= 0 ? busca.posicao - 11 : inicio; // Centraliza na posição
                if (inicio < busca.baixo) inicio = busca.baixo; // Garante limite
                fim = inicio + 21; // Define fim do corte
                if (fim > busca.alto) { // Se ultrapassar limite alto
                    fim = busca.alto; // Limita em alto
                    inicio = fim - 21; // Reajusta inicio
                    if (inicio < busca.baixo) inicio = busca.baixo; // Garante limite
                }
            }
            int vis = fim - inicio + 1; // Elementos visíveis a serem desenhados na tela
            int larguraCelula = 980 / (vis > 0 ? vis : 1); // Calcula a largura das retângulos/células

            Vector2 mousePos = GetMousePosition(); // Captura a posição X, Y do ponteiro do mouse

            // Desenho do Array e Interatividade via Mouse
            for (int i = inicio; i <= fim; i++) { // Percorre os elementos visíveis
                int x = 60 + (i - inicio) * larguraCelula; // Calcula coordenada X de cada barra
                Rectangle rectBarra = (Rectangle){ (float)x, 180.0f, (float)larguraCelula - 4, 45.0f }; // Define retângulo

                // Interatividade: Selecionar o Alvo ao clicar na célula do vetor com o mouse
                if (CheckCollisionPointRec(mousePos, rectBarra) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    busca.alvo = dados[i].valor.v_int; // Define novo alvo clicado
                    indiceAlvo = i; // Atualiza índice selecionado
                    reiniciarBusca(&busca); // Reinicia busca
                }

                // Aplicação de Cores por Estado do Elemento no Algoritmo
                Color cor = (Color){194, 207, 222, 255}; // Cor padrão: Cinza-azul
                if (i == busca.baixo || i == busca.alto) cor = (Color){105, 174, 220, 255}; // Azul: Ponteiros baixo/alto
                if (i == busca.posicao) cor = (Color){242, 170, 65, 255};                  // Laranja: Posição pos testada
                if (i == busca.posicao && busca.estado == ENCONTRADO) cor = (Color){74, 174, 112, 255};     // Verde: Sucesso
                if (i == busca.posicao && busca.estado == NAO_ENCONTRADO) cor = (Color){219, 92, 92, 255}; // Vermelho: Falha

                DrawRectangleRec(rectBarra, cor); // Renderiza o retângulo preenchido com a cor definida
                DrawRectangleLines((int)rectBarra.x, (int)rectBarra.y, (int)rectBarra.width, (int)rectBarra.height, (Color){100, 115, 130, 255}); // Borda
                DrawText(TextFormat("%d", dados[i].valor.v_int), x + 4, 193, 14, BLACK); // Valor do elemento
                DrawText(TextFormat("[%d]", i), x + 4, 230, 11, DARKGRAY); // Índice do elemento no vetor
            }
            DrawText("Legenda: Azul (Limites baixo/alto) | Laranja (Posicao testada pos) | Verde (Encontrado)", 48, 275, 13, DARKGRAY); // Legenda
        }

        // --- Renderização do Painel Inferior de Estatísticas de Execução Real ---
        DrawRectangleRounded((Rectangle){28, 315, 1044, 305}, 0.08f, 8, (Color){232, 238, 245, 255}); // Fundo do painel
        DrawText("Estatisticas da Execucao Real", 48, 330, 20, (Color){28, 47, 74, 255}); // Título das estatísticas

        if (mensagemErro[0] == '\0') { // Se dados válidos, exibe métricas em tempo real
            DrawText(TextFormat("Elementos (N): %d", busca.quantidade), 50, 370, 18, DARKGRAY); // Total N
            DrawText(TextFormat("Alvo Selecionado: %d", busca.alvo), 400, 370, 18, (Color){28, 47, 74, 255}); // Alvo
            DrawText(TextFormat("Comparacoes: %ld", busca.comparacoes), 50, 405, 18, DARKGRAY); // Contagem de comparações
            DrawText(TextFormat("Sondagens: %ld", busca.sondagens), 50, 440, 18, DARKGRAY);   // Contagem de sondagens
            DrawText(TextFormat("Passo Atual: %d", busca.passo), 400, 440, 18, DARKGRAY);       // Passo
            
            // Exibição do Tempo de Execução do algoritmo medido em milissegundos e microssegundos
            DrawText(TextFormat("Tempo de Execucao: %.6f ms (%.2f us)", busca.tempoExecucao, busca.tempoExecucao * 1000.0), 50, 480, 19, (Color){28, 47, 74, 255}); // Tempo
            DrawText(TextFormat("Estado da Aplicacao: %s", nomeEstado(busca.estado)), 50, 515, 18, DARKGRAY); // Estado FSM

            // Resultado Final do Processamento
            if (busca.estado == ENCONTRADO) {
                DrawText(TextFormat("RESULTADO: O valor %d foi ENCONTRADO no indice %d do vetor!", busca.alvo, busca.posicao), 50, 555, 18, (Color){32, 125, 70, 255}); // Sucesso
            } else if (busca.estado == NAO_ENCONTRADO) {
                DrawText(TextFormat("RESULTADO: O valor %d NAO existe no conjunto de dados.", busca.alvo), 50, 555, 18, MAROON); // Falha
            } else {
                DrawText("Aperte [ESPAÇO] para avançar passo a passo.", 50, 555, 16, (Color){28, 47, 74, 255}); // Instrução
            }
        }

        EndDrawing(); // Finaliza renderização do quadro
    }

    // Desalocação e encerramento ao fechar o aplicativo
    CloseWindow(); // Fecha a janela gráfica e desinicializa o contexto da Raylib
    free(dados);   // Libera a memória Heap alocada dinamicamente para o array de dados
    return 0;      // Retorna 0 (execução concluída com sucesso)
}
