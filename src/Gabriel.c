#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// parte do gabriel - arvore binaria

#define ATIVO 2
#define CATEGORIA 1
#define RAIZ 0

typedef struct No {
    char nome[64];
    int tipo;
    float percentual_alvo;
    float valor_investido;
    float valor_total;
    struct No* esquerda;
    struct No* direita;
} No;

typedef struct Arvore {
    No* raiz;
    float valor_total;
} Arvore;

// funcoes auxiliares (declaracoes)
float calcular_total_no(No* no);
No* buscar_no(No* raiz, const char* nome);

// Funcao para calcular total (recursiva)
float calcular_total_no(No* no) {
    if(no == NULL) {
        return 0.0;
    }

    if(no->esquerda == NULL && no->direita == NULL) {
        no->valor_total = no->valor_investido;
        return no->valor_total;
    }

    float soma = 0.0;
    soma = soma + calcular_total_no(no->esquerda);
    soma = soma + calcular_total_no(no->direita);
    soma = soma + no->valor_investido;

    no->valor_total = soma;
    return soma;
}

// Funcao para buscar no (recursiva)
No* buscar_no(No* raiz, const char* nome) {
    if(raiz == NULL) {
        return NULL;
    }

    if(strcmp(raiz->nome, nome) == 0) {
        return raiz;
    }

    No* resultado = buscar_no(raiz->esquerda, nome);
    if(resultado != NULL) {
        return resultado;
    }

    return buscar_no(raiz->direita, nome);
}

// Funcao para detectar desbalanceamento
void detectar_desbalanceamento(Arvore* arvore) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("\nCarteira vazia!\n");
        return;
    }

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("\n========================================\n");
    printf("ANALISE DE BALANCEAMENTO\n");
    printf("========================================\n");

    int desbalanceado = 0;
    float tolerancia = 2.0;

    // verificar renda fixa (esquerda)
    if(arvore->raiz->esquerda != NULL) {
        No* renda_fixa = arvore->raiz->esquerda;
        float percentual_atual = (renda_fixa->valor_total / arvore->valor_total) * 100.0;
        float diferenca = percentual_atual - renda_fixa->percentual_alvo;

        printf("\n%s:\n", renda_fixa->nome);
        printf("  Meta:  %.1f%%\n", renda_fixa->percentual_alvo);
        printf("  Atual: %.1f%%\n", percentual_atual);
        printf("  Diferenca: %+.1f%%\n", diferenca);

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

    // verificar acoes (direita)
    if(arvore->raiz->direita != NULL) {
        No* acoes = arvore->raiz->direita;
        float percentual_atual = (acoes->valor_total / arvore->valor_total) * 100.0;
        float diferenca = percentual_atual - acoes->percentual_alvo;

        printf("\n%s:\n", acoes->nome);
        printf("  Meta:  %.1f%%\n", acoes->percentual_alvo);
        printf("  Atual: %.1f%%\n", percentual_atual);
        printf("  Diferenca: %+.1f%%\n", diferenca);

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
        printf("CARTEIRA DESBALANCEADA\n");
        printf("Sugestao: Use a opcao para ver como rebalancear\n");
    } else {
        printf("CARTEIRA BALANCEADA\n");
    }
    printf("========================================\n");
}

// Funcao para sugerir rebalanceamento
void sugerir_rebalanceamento(Arvore* arvore) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("\nCarteira vazia!\n");
        return;
    }

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("\n========================================\n");
    printf("SUGESTOES DE REBALANCEAMENTO\n");
    printf("========================================\n");

    float tolerancia = 2.0;
    int precisa_rebalancear = 0;

    printf("\nAcoes necessarias:\n\n");

    // verificar renda fixa (esquerda)
    if(arvore->raiz->esquerda != NULL) {
        No* renda_fixa = arvore->raiz->esquerda;
        float valor_alvo = arvore->valor_total * (renda_fixa->percentual_alvo / 100.0);
        float diferenca = renda_fixa->valor_total - valor_alvo;

        if(diferenca > (arvore->valor_total * tolerancia / 100.0)) {
            printf("* VENDER R$ %.2f de %s\n", diferenca, renda_fixa->nome);
            precisa_rebalancear = 1;
        } else if(diferenca < -(arvore->valor_total * tolerancia / 100.0)) {
            printf("* COMPRAR R$ %.2f em %s\n", -diferenca, renda_fixa->nome);
            precisa_rebalancear = 1;
        }
    }

    // verificar acoes (direita)
    if(arvore->raiz->direita != NULL) {
        No* acoes = arvore->raiz->direita;
        float valor_alvo = arvore->valor_total * (acoes->percentual_alvo / 100.0);
        float diferenca = acoes->valor_total - valor_alvo;

        if(diferenca > (arvore->valor_total * tolerancia / 100.0)) {
            printf("* VENDER R$ %.2f de %s\n", diferenca, acoes->nome);
            precisa_rebalancear = 1;
        } else if(diferenca < -(arvore->valor_total * tolerancia / 100.0)) {
            printf("* COMPRAR R$ %.2f em %s\n", -diferenca, acoes->nome);
            precisa_rebalancear = 1;
        }
    }

    if(!precisa_rebalancear) {
        printf("Carteira ja esta balanceada!\n");
    }

    printf("\n========================================\n");
}

// Funcao para simular aporte
void simular_aporte(Arvore* arvore, float valor_aporte) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("\nCarteira vazia!\n");
        return;
    }

    if(valor_aporte <= 0) {
        printf("\nValor de aporte invalido!\n");
        return;
    }

    printf("\n========================================\n");
    printf("SIMULACAO DE APORTE\n");
    printf("========================================\n");
    printf("Valor do aporte: R$ %.2f\n\n", valor_aporte);

    printf("Distribuicao proporcional:\n\n");

    // distribuir para renda fixa (esquerda)
    if(arvore->raiz->esquerda != NULL) {
        No* renda_fixa = arvore->raiz->esquerda;
        float valor_rf = valor_aporte * (renda_fixa->percentual_alvo / 100.0);

        printf("* %s (%.0f%%): + R$ %.2f\n",
               renda_fixa->nome, renda_fixa->percentual_alvo, valor_rf);
        printf("  Novo total: R$ %.2f -> R$ %.2f\n\n",
               renda_fixa->valor_total, renda_fixa->valor_total + valor_rf);

        // dividir entre os ativos da categoria
        if(renda_fixa->esquerda != NULL && renda_fixa->esquerda->tipo == ATIVO) {
            renda_fixa->esquerda->valor_investido += valor_rf / 2;
        }
        if(renda_fixa->direita != NULL && renda_fixa->direita->tipo == ATIVO) {
            renda_fixa->direita->valor_investido += valor_rf / 2;
        }
    }

    // distribuir para acoes (direita)
    if(arvore->raiz->direita != NULL) {
        No* acoes = arvore->raiz->direita;
        float valor_rv = valor_aporte * (acoes->percentual_alvo / 100.0);

        printf("* %s (%.0f%%): + R$ %.2f\n",
               acoes->nome, acoes->percentual_alvo, valor_rv);
        printf("  Novo total: R$ %.2f -> R$ %.2f\n\n",
               acoes->valor_total, acoes->valor_total + valor_rv);

        // dividir entre os ativos da categoria
        if(acoes->esquerda != NULL && acoes->esquerda->tipo == ATIVO) {
            acoes->esquerda->valor_investido += valor_rv / 2;
        }
        if(acoes->direita != NULL && acoes->direita->tipo == ATIVO) {
            acoes->direita->valor_investido += valor_rv / 2;
        }
    }

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("========================================\n");
    printf("Novo valor total da carteira: R$ %.2f\n", arvore->valor_total);
    printf("Carteira atualizada com aporte!\n");
    printf("========================================\n");
}

// Funcao para atualizar valores de mercado
void atualizar_valores_mercado(Arvore* arvore, char* nome_ativo, float novo_valor) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("\nCarteira vazia!\n");
        return;
    }

    No* ativo = buscar_no(arvore->raiz, nome_ativo);

    if(ativo == NULL) {
        printf("\nAtivo nao encontrado!\n");
        return;
    }

    if(ativo->tipo != ATIVO) {
        printf("\nNao e um ativo!\n");
        return;
    }

    float valor_anterior = ativo->valor_investido;
    float variacao = 0.0;
    if(valor_anterior != 0.0) {
        variacao = ((novo_valor - valor_anterior) / valor_anterior) * 100.0;
    }

    ativo->valor_investido = novo_valor;

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("\n========================================\n");
    printf("ATUALIZACAO DE MERCADO\n");
    printf("========================================\n");
    printf("Ativo: %s\n", nome_ativo);
    printf("Valor anterior: R$ %.2f\n", valor_anterior);
    printf("Novo valor: R$ %.2f\n", novo_valor);
    printf("Variacao: %+.2f%%\n", variacao);
    printf("========================================\n");
    printf("\nDica: Use detectar_desbalanceamento() para verificar.\n");
}