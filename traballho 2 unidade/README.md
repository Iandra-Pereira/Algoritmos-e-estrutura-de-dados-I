# Algoritmo de Busca por Interpolação (AED I - UFERSA)

## Funcionalidades
- **Visualização Gráfica Interativa:** Acompanhe o comportamento dos ponteiros (`baixo`, `alto`) e a posição calculada pela fórmula de interpolação (`pos`).
- **Múltiplos Cenários de Teste:** Geração automática e suporte a diferentes distribuições de dados:
  1. `dados_uniforme.txt` (Distribuição uniforme)
  2. `dados_nao_uniforme.txt` (Distribuição não-uniforme / espaçamento variável)
  3. `dados_repetidos.txt` (Elementos repetidos)
  4. `dados_desordenados.txt` (Cenário de erro para dados não ordenados)
- **Métrica de Desempenho Isolada:** Medição precisa do tempo real de processamento do algoritmo (em milissegundos e microssegundos), sem interferência do tempo de espera visual da interface.
- **Interatividade Total:** Seleção de alvos via teclado (setas) ou clique direto com o mouse nas barras do vetor.


## ⌨️ Comandos e Atalhos (Teclado e Mouse)

| Tecla / Ação | Função |
| :--- | :--- |
| **[1]** | Carregar cenário Uniforme |
| **[2]** | Carregar cenário Não-Uniforme |
| **[3]** | Carregar cenário com Elementos Repetidos |
| **[4]** | Carregar cenário Desordenado (Simula erro de ordenação) |
| **[S]** | Iniciar execução automática |
| **[P]** | Pausar execução |
| **[C]** | Continuar execução automática |
| **[ESPAÇO] ou [N]** | Avançar passo a passo |
| **[R]** | Reiniciar busca para o estado inicial |
| **[SETAS CIMA / BAIXO]** | Alterar o valor do alvo selecionado |
| **Clique com Mouse** | Selecionar diretamente o elemento clicado como alvo |

---

## 📊 Arquivos do Projeto
- `main.c`: Código-fonte completo com a lógica do algoritmo, geradores automáticos de arquivos, interface gráfica e cronômetro de precisão.
- `dados_*.txt`: Arquivos de entrada gerados automaticamente na primeira execução para preenchimento da tabela de experimentos do relatório técnico.
