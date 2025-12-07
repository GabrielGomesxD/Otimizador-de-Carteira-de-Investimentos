#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>

// ========================================
// DEFINICOES
// ========================================

#define ATIVO 2
#define CATEGORIA 1
#define RAIZ 0

// ========================================
// STRUCTS - ARVORE BINARIA
// ========================================

typedef struct No {
    char nome[64];
    int tipo;
    float percentual_alvo;
    float valor_investido;
    float valor_total;
    struct No* esquerda;  // filho esquerdo
    struct No* direita;   // filho direito
} No;

typedef struct Arvore {
    No* raiz;
    float valor_total;
} Arvore;

// ========================================
// FUNCOES DO LUIS
// ========================================

// Funcao para criar um no novo
No* criar_no(const char* nome, int tipo, float percentual_alvo, float valor_investido) {
    No* novo = (No*) malloc(sizeof(No));

    strncpy(novo->nome, nome, 63);
    novo->nome[63] = '\0';

    novo->tipo = tipo;
    novo->percentual_alvo = percentual_alvo;
    novo->valor_investido = valor_investido;
    novo->valor_total = 0.0;
    novo->esquerda = NULL;
    novo->direita = NULL;

    return novo;
}

// Funcao para buscar um no pelo nome
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

// Funcao para calcular o total de um no
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

// Funcao para criar carteira por perfil
Arvore* criar_carteira_perfil(float valor_inicial, const char* perfil) {
    Arvore* carteira = (Arvore*) malloc(sizeof(Arvore));

    carteira->raiz = criar_no("Carteira", RAIZ, 0.0, 0.0);

    float perc_rf, perc_rv;

    if(strcmp(perfil, "CONSERVADOR") == 0) {
        perc_rf = 70.0;
        perc_rv = 30.0;
        printf("\nCriando carteira CONSERVADORA...\n");
    }
    else if(strcmp(perfil, "MODERADO") == 0) {
        perc_rf = 50.0;
        perc_rv = 50.0;
        printf("\nCriando carteira MODERADA...\n");
    }
    else if(strcmp(perfil, "ARROJADO") == 0) {
        perc_rf = 30.0;
        perc_rv = 70.0;
        printf("\nCriando carteira ARROJADA...\n");
    }
    else {
        perc_rf = 50.0;
        perc_rv = 50.0;
        printf("\nPerfil desconhecido. Usando MODERADO...\n");
    }

    No* renda_fixa = criar_no("Renda Fixa", CATEGORIA, perc_rf, 0.0);
    No* acoes = criar_no("Acoes", CATEGORIA, perc_rv, 0.0);

    carteira->raiz->esquerda = renda_fixa;
    carteira->raiz->direita = acoes;

    float valor_rf = valor_inicial * (perc_rf / 100.0);
    float valor_rv = valor_inicial * (perc_rv / 100.0);

    No* tesouro = criar_no("Tesouro Selic", ATIVO, 0.0, valor_rf / 2);
    No* cdb = criar_no("CDB XP", ATIVO, 0.0, valor_rf / 2);
    renda_fixa->esquerda = tesouro;
    renda_fixa->direita = cdb;

    No* petr4 = criar_no("PETR4", ATIVO, 0.0, valor_rv / 2);
    No* itub4 = criar_no("ITUB4", ATIVO, 0.0, valor_rv / 2);
    acoes->esquerda = petr4;
    acoes->direita = itub4;

    calcular_total_no(carteira->raiz);
    carteira->valor_total = carteira->raiz->valor_total;

    printf("Carteira criada! Valor total: R$ %.2f\n", carteira->valor_total);

    return carteira;
}

// Funcao para remover um ativo
void remover_ativo(Arvore* arvore, const char* nome) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("Carteira vazia!\n");
        return;
    }

    No* ativo = buscar_no(arvore->raiz, nome);

    if(ativo == NULL) {
        printf("Ativo nao encontrado!\n");
        return;
    }

    if(ativo->tipo != ATIVO) {
        printf("Nao e um ativo!\n");
        return;
    }

    ativo->valor_investido = 0.0;

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("Ativo removido com sucesso!\n");
}

// Funcao para atualizar percentuais
void atualizar_percentuais(Arvore* arvore) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("Carteira vazia!\n");
        return;
    }

    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("\n=== PERCENTUAIS DA CARTEIRA ===\n");
    printf("Valor total: R$ %.2f\n\n", arvore->valor_total);

    if(arvore->raiz->esquerda != NULL) {
        No* rf = arvore->raiz->esquerda;
        float percentual_atual = (rf->valor_total / arvore->valor_total) * 100.0;

        printf("%s:\n", rf->nome);
        printf("  Meta: %.1f%%\n", rf->percentual_alvo);
        printf("  Atual: %.1f%%\n", percentual_atual);
        printf("  Valor: R$ %.2f\n", rf->valor_total);

        float diferenca = percentual_atual - rf->percentual_alvo;
        if(diferenca > 1.0) {
            printf("  Status: Acima do alvo (+%.1f%%)\n", diferenca);
        } else if(diferenca < -1.0) {
            printf("  Status: Abaixo do alvo (%.1f%%)\n", diferenca);
        } else {
            printf("  Status: OK\n");
        }
        printf("\n");
    }

    if(arvore->raiz->direita != NULL) {
        No* acoes = arvore->raiz->direita;
        float percentual_atual = (acoes->valor_total / arvore->valor_total) * 100.0;

        printf("%s:\n", acoes->nome);
        printf("  Meta: %.1f%%\n", acoes->percentual_alvo);
        printf("  Atual: %.1f%%\n", percentual_atual);
        printf("  Valor: R$ %.2f\n", acoes->valor_total);

        float diferenca = percentual_atual - acoes->percentual_alvo;
        if(diferenca > 1.0) {
            printf("  Status: Acima do alvo (+%.1f%%)\n", diferenca);
        } else if(diferenca < -1.0) {
            printf("  Status: Abaixo do alvo (%.1f%%)\n", diferenca);
        } else {
            printf("  Status: OK\n");
        }
        printf("\n");
    }
}

// Funcao para listar ativos de uma categoria
void listar_ativos(Arvore* arvore, const char* nome_categoria) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("Carteira vazia!\n");
        return;
    }

    No* categoria = buscar_no(arvore->raiz, nome_categoria);

    if(categoria == NULL) {
        printf("Categoria nao encontrada!\n");
        return;
    }

    if(categoria->tipo != CATEGORIA) {
        printf("Nao e uma categoria!\n");
        return;
    }

    printf("\n=== ATIVOS EM %s ===\n", categoria->nome);

    int count = 0;

    if(categoria->esquerda != NULL) {
        count++;
        printf("%d. %s - R$ %.2f\n", count, categoria->esquerda->nome, categoria->esquerda->valor_investido);
    }

    if(categoria->direita != NULL) {
        count++;
        printf("%d. %s - R$ %.2f\n", count, categoria->direita->nome, categoria->direita->valor_investido);
    }

    if(count == 0) {
        printf("Nenhum ativo nesta categoria.\n");
    }

    printf("Total: R$ %.2f\n", categoria->valor_total);
}

// Funcao para liberar memoria
void liberar_no(No* no) {
    if(no == NULL) return;

    liberar_no(no->esquerda);
    liberar_no(no->direita);

    free(no);
}

void liberar_arvore(Arvore* arvore) {
    if(arvore == NULL) return;

    liberar_no(arvore->raiz);
    free(arvore);
}

// ========================================
// FUNCOES DO GABRIEL
// ========================================

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

    if(arvore->raiz->esquerda != NULL) {
        No* renda_fixa = arvore->raiz->esquerda;
        float valor_rf = valor_aporte * (renda_fixa->percentual_alvo / 100.0);

        printf("* %s (%.0f%%): + R$ %.2f\n",
               renda_fixa->nome, renda_fixa->percentual_alvo, valor_rf);
        printf("  Novo total: R$ %.2f -> R$ %.2f\n\n",
               renda_fixa->valor_total, renda_fixa->valor_total + valor_rf);

        if(renda_fixa->esquerda != NULL && renda_fixa->esquerda->tipo == ATIVO) {
            renda_fixa->esquerda->valor_investido += valor_rf / 2;
        }
        if(renda_fixa->direita != NULL && renda_fixa->direita->tipo == ATIVO) {
            renda_fixa->direita->valor_investido += valor_rf / 2;
        }
    }

    if(arvore->raiz->direita != NULL) {
        No* acoes = arvore->raiz->direita;
        float valor_rv = valor_aporte * (acoes->percentual_alvo / 100.0);

        printf("* %s (%.0f%%): + R$ %.2f\n",
               acoes->nome, acoes->percentual_alvo, valor_rv);
        printf("  Novo total: R$ %.2f -> R$ %.2f\n\n",
               acoes->valor_total, acoes->valor_total + valor_rv);

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

// ========================================
// FUNCOES DO MARCELLO - MENU
// ========================================

void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar() {
    printf("\nPressione ENTER para continuar...");
    while(getchar() != '\n');
    getchar();
}

void menu_principal(Arvore** carteira) {
    int opcao;
    float valor;
    char nome[64];
    int escolha;

    while(1) {
        limpar_tela();

        printf("\n========================================\n");
        printf("   OTIMIZADOR DE CARTEIRA\n");
        printf("========================================\n");
        printf("1. Criar nova carteira\n");
        printf("2. Ver percentuais da carteira\n");
        printf("3. Listar ativos de uma categoria\n");
        printf("4. Atualizar valor de ativo\n");
        printf("5. Remover ativo\n");
        printf("6. Detectar desbalanceamento\n");
        printf("7. Sugerir rebalanceamento\n");
        printf("8. Simular aporte\n");
        printf("0. Sair\n");
        printf("========================================\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        if(opcao == 1) {
            if(*carteira != NULL) {
                printf("\nJa existe uma carteira! Removendo anterior...\n");
                liberar_arvore(*carteira);
                *carteira = NULL;
            }

            printf("\nQual o valor inicial da carteira? R$ ");
            scanf("%f", &valor);

            printf("\nEscolha o perfil da carteira:\n");
            printf("1. CONSERVADOR (70%% Renda Fixa, 30%% Acoes)\n");
            printf("2. MODERADO (50%% Renda Fixa, 50%% Acoes)\n");
            printf("3. ARROJADO (30%% Renda Fixa, 70%% Acoes)\n");
            printf("Opcao: ");
            scanf("%d", &escolha);

            if(escolha == 1) {
                *carteira = criar_carteira_perfil(valor, "CONSERVADOR");
            } else if(escolha == 2) {
                *carteira = criar_carteira_perfil(valor, "MODERADO");
            } else if(escolha == 3) {
                *carteira = criar_carteira_perfil(valor, "ARROJADO");
            } else {
                printf("\nOpcao invalida! Usando MODERADO...\n");
                *carteira = criar_carteira_perfil(valor, "MODERADO");
            }

            pausar();
        }
        else if(opcao == 2) {
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                atualizar_percentuais(*carteira);
            }
            pausar();
        }
        else if(opcao == 3) {
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                printf("\nQual categoria deseja listar?\n");
                printf("1. Renda Fixa\n");
                printf("2. Acoes\n");
                printf("Opcao: ");
                scanf("%d", &escolha);

                if(escolha == 1) {
                    listar_ativos(*carteira, "Renda Fixa");
                } else if(escolha == 2) {
                    listar_ativos(*carteira, "Acoes");
                } else {
                    printf("\nOpcao invalida!\n");
                }
            }
            pausar();
        }
        else if(opcao == 4) {
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                printf("\nNome do ativo (ex: PETR4, ITUB4, Tesouro Selic, CDB XP): ");
                scanf("%s", nome);
                printf("Novo valor do ativo: R$ ");
                scanf("%f", &valor);
                atualizar_valores_mercado(*carteira, nome, valor);
            }
            pausar();
        }
        else if(opcao == 5) {
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                printf("\nNome do ativo a remover: ");
                scanf("%s", nome);
                remover_ativo(*carteira, nome);
            }
            pausar();
        }
        else if(opcao == 6) {
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                detectar_desbalanceamento(*carteira);
            }
            pausar();
        }
        else if(opcao == 7) {
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                sugerir_rebalanceamento(*carteira);
            }
            pausar();
        }
        else if(opcao == 8) {
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                printf("\nQual o valor do aporte? R$ ");
                scanf("%f", &valor);
                simular_aporte(*carteira, valor);
            }
            pausar();
        }
        else if(opcao == 0) {
            printf("\nEncerrando o programa...\n");
            if(*carteira != NULL) {
                liberar_arvore(*carteira);
                *carteira = NULL;
            }
            printf("Programa finalizado!\n");
            return;
        }
        else {
            printf("\nOpcao invalida! Tente novamente.\n");
            pausar();
        }
    }
}

// ========================================
// MAIN
// ========================================

int main() {
    setlocale(LC_ALL, "Portuguese");

    Arvore* carteira = NULL;

    menu_principal(&carteira);

    return 0;
}