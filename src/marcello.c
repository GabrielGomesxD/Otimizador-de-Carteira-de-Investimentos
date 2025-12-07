#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// parte do marcello - menu interativo

// declaracoes das funcoes do Luis
typedef struct No No;
typedef struct Arvore Arvore;

Arvore* criar_carteira_perfil(float valor_inicial, const char* perfil);
void atualizar_percentuais(Arvore* arvore);
void listar_ativos(Arvore* arvore, const char* nome_categoria);
void remover_ativo(Arvore* arvore, const char* nome);
void liberar_arvore(Arvore* arvore);

// declaracoes das funcoes do Gabriel
void detectar_desbalanceamento(Arvore* arvore);
void sugerir_rebalanceamento(Arvore* arvore);
void simular_aporte(Arvore* arvore, float valor_aporte);
void atualizar_valores_mercado(Arvore* arvore, char* nome_ativo, float novo_valor);

// funcoes auxiliares do menu
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

// funcao principal do menu
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
            // criar nova carteira
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
            // ver percentuais
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                atualizar_percentuais(*carteira);
            }
            pausar();
        }
        else if(opcao == 3) {
            // listar ativos de categoria
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
            // atualizar valor de ativo
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
            // remover ativo
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
            // detectar desbalanceamento
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                detectar_desbalanceamento(*carteira);
            }
            pausar();
        }
        else if(opcao == 7) {
            // sugerir rebalanceamento
            if(*carteira == NULL) {
                printf("\nCrie uma carteira primeiro! (opcao 1)\n");
            } else {
                sugerir_rebalanceamento(*carteira);
            }
            pausar();
        }
        else if(opcao == 8) {
            // simular aporte
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
            // sair
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