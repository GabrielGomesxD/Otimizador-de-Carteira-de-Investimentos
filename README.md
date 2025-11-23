📊 Otimizador de Carteira 

Este projeto implementa um sistema completo de análise, detecção de desbalanceamento, rebalanceamento, simulação de aportes e variação de mercado de uma carteira de investimentos.
O objetivo é demonstrar, de forma clara e modular, como executar operações comuns de alocação de ativos.

O código foi estruturado seguindo boas práticas, com funções separadas e documentação detalhada para facilitar a compreensão e manutenção.

🚀 Funcionalidades Implementadas

✔ Detecção de desbalanceamento
✔ Sugestão automática de rebalanceamento
✔ Simulação de aporte proporcional
✔ Atualização de valores após variação de mercado
✔ Cálculos percentuais com precisão e locale brasileiro
✔ Casos de teste automatizados
✔ Código modular e documentado

🧠 Lógica Geral do Sistema

📌 1. Detecção de Desbalanceamento

Compara a porcentagem atual de cada ativo com o percentual ideal desejado.

Se a diferença for maior que um limiar (ex.: 3%), considera o ativo desbalanceado.

📌 2. Sugestão de Rebalanceamento

Calcula quanto deve ser vendido ou comprado para cada ativo retornar ao percentual ideal.

A fórmula usada:

valor_total = soma de todos os valores da carteira
valor_ideal = percentual_desejado * valor_total
diferença = valor_atual - valor_ideal

📌 3. Atualização de Mercado

Aplica variações positivas ou negativas em cada ativo, simulando cenários reais de oscilação.

📌 4. Simulação de Aporte

Distribui o valor informado de acordo com o percentual ideal de cada ativo.

🖥️ Exemplo do Código (main.py)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h> 

#define MAX_FILHOS 20
#define ATIVO 2
#define CATEGORIA 1
#define RAIZ 0

typedef struct No {
    char nome[64];
    int tipo; // RAIZ, CATEGORIA ou ATIVO
    float percentual_alvo; // usado para categorias (em %)
    float valor_investido; // usado para ativos
    float valor_total; // soma recursiva (categoria ou raiz)

    struct No* filhos[MAX_FILHOS];
    int num_filhos;
} No;

typedef struct Arvore {
    No* raiz;
    float valor_total;
} Arvore;

// ----- helpers de criação e montagem -----
No* criar_no(const char* nome, int tipo, float percentual_alvo, float valor_investido) {
    No* n = (No*) malloc(sizeof(No));
    if(!n) {
        perror("malloc");
        exit(1);
    }
    strncpy(n->nome, nome, sizeof(n->nome)-1);
    n->nome[sizeof(n->nome)-1] = '\0';
    n->tipo = tipo;
    n->percentual_alvo = percentual_alvo;
    n->valor_investido = valor_investido;
    n->valor_total = 0.0f;
    n->num_filhos = 0;
    for(int i=0;i<MAX_FILHOS;i++) n->filhos[i] = NULL;
    return n;
}

void adicionar_filho(No* pai, No* filho) {
    if(pai->num_filhos >= MAX_FILHOS) {
        printf("\nErro: max filhos atingido para %s\n", pai->nome);
        return;
    }
    pai->filhos[pai->num_filhos++] = filho;
}

// ----- operações na árvore -----

// Calcula recursivamente o valor_total de um nó e retorna-o
float calcular_total_no(No* no) {
    if(no == NULL) return 0.0f;
    if(no->num_filhos == 0) {
        no->valor_total = no->valor_investido;
        return no->valor_total;
    }
    float soma = 0.0f;
    for(int i=0;i<no->num_filhos;i++) {
        soma += calcular_total_no(no->filhos[i]);
    }
    soma += no->valor_investido;
    no->valor_total = soma;
    return soma;
}

// Busca nó pelo nome (pré-ordem)
No* buscar_no(No* raiz, const char* nome) {
    if(raiz == NULL) return NULL;
    if(strcmp(raiz->nome, nome) == 0) return raiz;
    for(int i=0;i<raiz->num_filhos;i++) {
        No* r = buscar_no(raiz->filhos[i], nome);
        if(r != NULL) return r;
    }
    return NULL;
}

// ----- Funções do sistema de carteira -----

void detectar_desbalanceamento(Arvore* arvore) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("\n Carteira vazia!\n");
        return;
    }

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("\n========================================\n");
    printf("ANÁLISE DE BALANCEAMENTO\n");
    printf("========================================\n");

    int desbalanceado = 0;
    float tolerancia = 2.0;

    for(int i = 0; i < arvore->raiz->num_filhos; i++) {
        No* categoria = arvore->raiz->filhos[i];

        float percentual_atual = (categoria->valor_total / arvore->valor_total) * 100.0f;
        float diferenca = percentual_atual - categoria->percentual_alvo;

        printf("\n%s:\n", categoria->nome);
        printf("  Meta:  %.1f%%\n", categoria->percentual_alvo);
        printf("  Atual: %.1f%%\n", percentual_atual);
        printf("  Diferença: %+.1f%%\n", diferenca);

        if(fabs(diferenca) > tolerancia) {
            if(diferenca > 0) {
                printf("    ACIMA do alvo (sobra %.1f%%)\n", diferenca);
            } else {
                printf("    ABAIXO do alvo (falta %.1f%%)\n", -diferenca);
            }
            desbalanceado = 1;
        } else {
            printf("   Dentro do alvo\n");
        }
    }

    printf("\n========================================\n");
    if(desbalanceado) {
        printf(" CARTEIRA DESBALANCEADA\n");
        printf("Sugestão: Use a opção 8 para ver como rebalancear\n");
    } else {
        printf(" CARTEIRA BALANCEADA\n");
    }
    printf("========================================\n");
}

void sugerir_rebalanceamento(Arvore* arvore) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("\n Carteira vazia!\n");
        return;
    }

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("\n========================================\n");
    printf("SUGESTÕES DE REBALANCEAMENTO\n");
    printf("========================================\n");

    float tolerancia = 2.0;
    int precisa_rebalancear = 0;

    typedef struct {
        char nome[50];
        float valor_atual;
        float valor_alvo;
        float diferenca_reais;
    } Info;

    Info categorias[20];
    int num_categorias = arvore->raiz->num_filhos;

    for(int i = 0; i < num_categorias; i++) {
        No* categoria = arvore->raiz->filhos[i];

        float percentual_atual = (categoria->valor_total / arvore->valor_total) * 100.0f;
        float valor_alvo = arvore->valor_total * (categoria->percentual_alvo / 100.0f);
        float diferenca = categoria->valor_total - valor_alvo;

        strncpy(categorias[i].nome, categoria->nome, sizeof(categorias[i].nome)-1);
        categorias[i].nome[sizeof(categorias[i].nome)-1] = '\0';
        categorias[i].valor_atual = categoria->valor_total;
        categorias[i].valor_alvo = valor_alvo;
        categorias[i].diferenca_reais = diferenca;

        if(fabs((percentual_atual - categoria->percentual_alvo)) > tolerancia) {
            precisa_rebalancear = 1;
        }
    }

    if(!precisa_rebalancear) {
        printf("\n Carteira já está balanceada!\n");
        printf("========================================\n");
        return;
    }

    printf("\nAções necessárias:\n\n");

    for(int i = 0; i < num_categorias; i++) {
        if(categorias[i].diferenca_reais > (arvore->valor_total * tolerancia / 100.0f)) {
            printf("• VENDER R$ %.2f de %s\n", 
                   categorias[i].diferenca_reais, categorias[i].nome);
        } else if(categorias[i].diferenca_reais < -(arvore->valor_total * tolerancia / 100.0f)) {
            printf("• COMPRAR R$ %.2f em %s\n", 
                   -categorias[i].diferenca_reais, categorias[i].nome);
        }
    }

    printf("\n========================================\n");
}

void simular_aporte(Arvore* arvore, float valor_aporte) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("\n Carteira vazia!\n");
        return;
    }

    if(valor_aporte <= 0) {
        printf("\n Valor de aporte inválido!\n");
        return;
    }

    printf("\n========================================\n");
    printf("SIMULAÇÃO DE APORTE\n");
    printf("========================================\n");
    printf("Valor do aporte: R$ %.2f\n\n", valor_aporte);

    printf("Distribuição proporcional:\n\n");

    for(int i = 0; i < arvore->raiz->num_filhos; i++) {
        No* categoria = arvore->raiz->filhos[i];

        float valor_categoria = valor_aporte * (categoria->percentual_alvo / 100.0f);

        printf("• %s (%.0f%%): + R$ %.2f\n", 
               categoria->nome, categoria->percentual_alvo, valor_categoria);
        printf("  Novo total: R$ %.2f  R$ %.2f\n\n",
               categoria->valor_total, categoria->valor_total + valor_categoria);

        if(categoria->num_filhos > 0) {
            int count_ativos = 0;
            for(int j=0;j<categoria->num_filhos;j++) 
                if(categoria->filhos[j]->tipo == ATIVO) count_ativos++;

            float valor_por_ativo = (count_ativos > 0) ? valor_categoria / count_ativos : 0.0f;

            for(int j = 0; j < categoria->num_filhos; j++) {
                No* ativo = categoria->filhos[j];
                if(ativo->tipo == ATIVO) {
                    ativo->valor_investido += valor_por_ativo;
                }
            }
        }
    }

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("========================================\n");
    printf("Novo valor total da carteira: R$ %.2f\n", arvore->valor_total);
    printf(" Carteira atualizada com aporte!\n");
    printf("========================================\n");
}

void atualizar_valores_mercado(Arvore* arvore, char* nome_ativo, float novo_valor) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("\n Carteira vazia!\n");
        return;
    }

    No* ativo = buscar_no(arvore->raiz, nome_ativo);

    if(ativo == NULL) {
        printf("\n Ativo '%s' não encontrado!\n", nome_ativo);
        return;
    }

    if(ativo->tipo != ATIVO) {
        printf("\n '%s' não é um ativo!\n", nome_ativo);
        return;
    }

    float valor_anterior = ativo->valor_investido;
    float variacao = 0.0f;
    if(valor_anterior != 0.0f)
        variacao = ((novo_valor - valor_anterior) / valor_anterior) * 100.0f;

    ativo->valor_investido = novo_valor;

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("\n========================================\n");
    printf("ATUALIZAÇÃO DE MERCADO\n");
    printf("========================================\n");
    printf("Ativo: %s\n", nome_ativo);
    printf("Valor anterior: R$ %.2f\n", valor_anterior);
    printf("Novo valor: R$ %.2f\n", novo_valor);
    printf("Variação: %+.2f%%\n", variacao);
    printf("========================================\n");
    printf("\n Dica: Use detectar_desbalanceamento() para verificar se a carteira desbalanceou.\n");
}

// ----- liberar memória -----

void liberar_no(No* no) {
    if(no == NULL) return;
    for(int i=0;i<no->num_filhos;i++) liberar_no(no->filhos[i]);
    free(no);
}

// ----- exemplo da carteira -----

Arvore* criar_exemplo() {
    Arvore* a = (Arvore*) malloc(sizeof(Arvore));
    a->raiz = criar_no("Carteira", RAIZ, 0.0f, 0.0f);

    No* renda_fixa = criar_no("Renda Fixa", CATEGORIA, 50.0f, 0.0f);
    No* acoes = criar_no("Ações", CATEGORIA, 40.0f, 0.0f);
    No* caixa = criar_no("Caixa", CATEGORIA, 10.0f, 0.0f);

    No* tesouro = criar_no("Tesouro Selic", ATIVO, 0.0f, 3000.0f);
    No* cdb = criar_no("CDB XP", ATIVO, 0.0f, 2000.0f);

    No* petro = criar_no("PETR4", ATIVO, 0.0f, 1500.0f);
    No* itau = criar_no("ITUB4", ATIVO, 0.0f, 1500.0f);

    No* conta = criar_no("Conta Corrente", ATIVO, 0.0f, 500.0f);

    adicionar_filho(a->raiz, renda_fixa);
    adicionar_filho(a->raiz, acoes);
    adicionar_filho(a->raiz, caixa);

    adicionar_filho(renda_fixa, tesouro);
    adicionar_filho(renda_fixa, cdb);

    adicionar_filho(acoes, petro);
    adicionar_filho(acoes, itau);

    adicionar_filho(caixa, conta);

    calcular_total_no(a->raiz);
    a->valor_total = a->raiz->valor_total;

    return a;
}

int main() {
    setlocale(LC_ALL, "Portuguese"); // habilita acentuação

    Arvore* carteira = criar_exemplo();

    printf("\n=== Estado inicial da carteira ===\n");
    printf("Valor total: R$ %.2f\n", carteira->valor_total);
    detectar_desbalanceamento(carteira);

    printf("\n=== Teste Caso 2: valorizar PETR4 ===\n");
    atualizar_valores_mercado(carteira, "PETR4", 3000.0f);
    detectar_desbalanceamento(carteira);
    sugerir_rebalanceamento(carteira);

    printf("\n=== Teste Caso 3: aporte de R$ 1000 ===\n");
    simular_aporte(carteira, 1000.0f);
    detectar_desbalanceamento(carteira);
    sugerir_rebalanceamento(carteira);

    liberar_no(carteira->raiz);
    free(carteira);
    return 0;
}

🧪 Casos de Teste

O script já executa automaticamente:

✔ Caso 1 — Carteira inicial

Mostra percentuais, detecta desbalanceamento e sugere correções.

✔ Caso 2 — Após variação de mercado

Simula valorização/desvalorização e recalcula toda a carteira.

✔ Caso 3 — Simulação de aporte

Distribui aporte conforme a alocação desejada e exibe nova composição.

📘 Documentação das Funções
detectar_desbalanceamento(carteira, alocacao_ideal, tolerancia)

Retorna ativos que estão fora da tolerância permitida.

sugerir_rebalanceamento(carteira, alocacao_ideal)

Gera instruções de compra/venda para corrigir a carteira.

atualizar_valores_mercado(carteira, variacoes)

Aplica percentuais de valorização/desvalorização.

simular_aporte(carteira, alocacao_ideal, valor)

Distribui um aporte conforme a alocação ideal.

👨‍💻 Autores

Gabriel, Luis, Marcello
Estudantes de Engenharia da Computação – UNISC
2025
