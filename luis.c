#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_FILHOS 20
#define ATIVO 2
#define CATEGORIA 1
#define RAIZ 0

typedef struct No {
    char nome[64];
    int tipo;
    float percentual_alvo;
    float valor_investido;
    float valor_total;
    struct No* filhos[MAX_FILHOS];
    int num_filhos;
} No;

typedef struct Arvore {
    No* raiz;
    float valor_total;
} Arvore;

// ========== FUNÇÕES DO LUIS ==========

// Função para criar um nó novo
No* criar_no(const char* nome, int tipo, float percentual_alvo, float valor_investido) {
    No* novo = (No*) malloc(sizeof(No));

    // copiar o nome
    strncpy(novo->nome, nome, 63);
    novo->nome[63] = '\0';

    novo->tipo = tipo;
    novo->percentual_alvo = percentual_alvo;
    novo->valor_investido = valor_investido;
    novo->valor_total = 0.0;
    novo->num_filhos = 0;

    // inicializar os filhos como NULL
    int i;
    for(i = 0; i < MAX_FILHOS; i++) {
        novo->filhos[i] = NULL;
    }

    return novo;
}

// Função para adicionar um filho no nó pai
void adicionar_filho(No* pai, No* filho) {
    if(pai->num_filhos >= MAX_FILHOS) {
        printf("Erro: limite de filhos atingido!\n");
        return;
    }

    pai->filhos[pai->num_filhos] = filho;
    pai->num_filhos++;
}

// Função para buscar um nó pelo nome (recursivo)
No* buscar_no(No* raiz, const char* nome) {
    if(raiz == NULL) {
        return NULL;
    }

    // verificar se é o nó atual
    if(strcmp(raiz->nome, nome) == 0) {
        return raiz;
    }

    // procurar nos filhos
    int i;
    for(i = 0; i < raiz->num_filhos; i++) {
        No* resultado = buscar_no(raiz->filhos[i], nome);
        if(resultado != NULL) {
            return resultado;
        }
    }

    return NULL;
}

// Função para calcular o total de um nó (recursivo)
float calcular_total_no(No* no) {
    if(no == NULL) {
        return 0.0;
    }

    // se não tem filhos, retorna o valor investido
    if(no->num_filhos == 0) {
        no->valor_total = no->valor_investido;
        return no->valor_total;
    }

    // se tem filhos, soma todos
    float soma = 0.0;
    int i;
    for(i = 0; i < no->num_filhos; i++) {
        soma = soma + calcular_total_no(no->filhos[i]);
    }

    // adiciona o valor próprio também
    soma = soma + no->valor_investido;
    no->valor_total = soma;

    return soma;
}

// Função para criar carteira por perfil
Arvore* criar_carteira_perfil(float valor_inicial, const char* perfil) {
    // alocar memória para a árvore
    Arvore* carteira = (Arvore*) malloc(sizeof(Arvore));

    // criar nó raiz
    carteira->raiz = criar_no("Carteira", RAIZ, 0.0, 0.0);

    // definir percentuais baseado no perfil
    float perc_rf, perc_rv, perc_caixa;

    if(strcmp(perfil, "CONSERVADOR") == 0) {
        perc_rf = 70.0;
        perc_rv = 20.0;
        perc_caixa = 10.0;
        printf("\nCriando carteira CONSERVADORA...\n");
    }
    else if(strcmp(perfil, "MODERADO") == 0) {
        perc_rf = 50.0;
        perc_rv = 40.0;
        perc_caixa = 10.0;
        printf("\nCriando carteira MODERADA...\n");
    }
    else if(strcmp(perfil, "ARROJADO") == 0) {
        perc_rf = 20.0;
        perc_rv = 70.0;
        perc_caixa = 10.0;
        printf("\nCriando carteira ARROJADA...\n");
    }
    else {
        // padrão: moderado
        perc_rf = 50.0;
        perc_rv = 40.0;
        perc_caixa = 10.0;
        printf("\nPerfil desconhecido. Usando MODERADO...\n");
    }

    // criar as categorias
    No* renda_fixa = criar_no("Renda Fixa", CATEGORIA, perc_rf, 0.0);
    No* acoes = criar_no("Ações", CATEGORIA, perc_rv, 0.0);
    No* caixa = criar_no("Caixa", CATEGORIA, perc_caixa, 0.0);

    // adicionar categorias na raiz
    adicionar_filho(carteira->raiz, renda_fixa);
    adicionar_filho(carteira->raiz, acoes);
    adicionar_filho(carteira->raiz, caixa);

    // calcular valores de cada categoria
    float valor_rf = valor_inicial * (perc_rf / 100.0);
    float valor_rv = valor_inicial * (perc_rv / 100.0);
    float valor_caixa = valor_inicial * (perc_caixa / 100.0);

    // criar ativos de renda fixa (dividir em 2)
    No* tesouro = criar_no("Tesouro Selic", ATIVO, 0.0, valor_rf / 2);
    No* cdb = criar_no("CDB XP", ATIVO, 0.0, valor_rf / 2);
    adicionar_filho(renda_fixa, tesouro);
    adicionar_filho(renda_fixa, cdb);

    // criar ativos de ações (dividir em 2)
    No* petr4 = criar_no("PETR4", ATIVO, 0.0, valor_rv / 2);
    No* itub4 = criar_no("ITUB4", ATIVO, 0.0, valor_rv / 2);
    adicionar_filho(acoes, petr4);
    adicionar_filho(acoes, itub4);

    // criar ativo de caixa
    No* conta = criar_no("Conta Corrente", ATIVO, 0.0, valor_caixa);
    adicionar_filho(caixa, conta);

    // calcular total
    calcular_total_no(carteira->raiz);
    carteira->valor_total = carteira->raiz->valor_total;

    printf("Carteira criada! Valor total: R$ %.2f\n", carteira->valor_total);

    return carteira;
}

// Função para remover um ativo
void remover_ativo(Arvore* arvore, const char* nome) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("Carteira vazia!\n");
        return;
    }

    // buscar o ativo
    No* ativo = buscar_no(arvore->raiz, nome);

    if(ativo == NULL) {
        printf("Ativo '%s' não encontrado!\n", nome);
        return;
    }

    if(ativo->tipo != ATIVO) {
        printf("'%s' não é um ativo!\n", nome);
        return;
    }

    // procurar o pai desse ativo
    // (vamos ter que procurar de novo na árvore toda)
    No* pai = NULL;
    int indice = -1;

    // função auxiliar para achar o pai
    // (não vou criar função separada, vou fazer inline)
    int i, j;
    for(i = 0; i < arvore->raiz->num_filhos; i++) {
        No* categoria = arvore->raiz->filhos[i];
        for(j = 0; j < categoria->num_filhos; j++) {
            if(categoria->filhos[j] == ativo) {
                pai = categoria;
                indice = j;
                break;
            }
        }
        if(pai != NULL) break;
    }

    if(pai == NULL) {
        printf("Erro: pai não encontrado!\n");
        return;
    }

    // remover o filho do pai (reorganizar array)
    for(i = indice; i < pai->num_filhos - 1; i++) {
        pai->filhos[i] = pai->filhos[i + 1];
    }
    pai->filhos[pai->num_filhos - 1] = NULL;
    pai->num_filhos--;

    // liberar memória do ativo
    free(ativo);

    // recalcular totais
    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("Ativo '%s' removido com sucesso!\n", nome);
}

// Função para atualizar percentuais e mostrar status
void atualizar_percentuais(Arvore* arvore) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("Carteira vazia!\n");
        return;
    }

    // recalcular totais
    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("\n=== PERCENTUAIS DA CARTEIRA ===\n");
    printf("Valor total: R$ %.2f\n\n", arvore->valor_total);

    // mostrar cada categoria
    int i;
    for(i = 0; i < arvore->raiz->num_filhos; i++) {
        No* categoria = arvore->raiz->filhos[i];

        float percentual_atual = (categoria->valor_total / arvore->valor_total) * 100.0;

        printf("%s:\n", categoria->nome);
        printf("  Meta: %.1f%%\n", categoria->percentual_alvo);
        printf("  Atual: %.1f%%\n", percentual_atual);
        printf("  Valor: R$ %.2f\n", categoria->valor_total);

        float diferenca = percentual_atual - categoria->percentual_alvo;
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

// Função para listar ativos de uma categoria
void listar_ativos(Arvore* arvore, const char* nome_categoria) {
    if(arvore == NULL || arvore->raiz == NULL) {
        printf("Carteira vazia!\n");
        return;
    }

    No* categoria = buscar_no(arvore->raiz, nome_categoria);

    if(categoria == NULL) {
        printf("Categoria '%s' não encontrada!\n", nome_categoria);
        return;
    }

    if(categoria->tipo != CATEGORIA) {
        printf("'%s' não é uma categoria!\n", nome_categoria);
        return;
    }

    printf("\n=== ATIVOS EM %s ===\n", categoria->nome);

    if(categoria->num_filhos == 0) {
        printf("Nenhum ativo nesta categoria.\n");
    } else {
        int i;
        for(i = 0; i < categoria->num_filhos; i++) {
            No* ativo = categoria->filhos[i];
            printf("%d. %s - R$ %.2f\n", i+1, ativo->nome, ativo->valor_investido);
        }
    }

    printf("Total: R$ %.2f\n", categoria->valor_total);
}

// Função para liberar memória
void liberar_no(No* no) {
    if(no == NULL) return;

    int i;
    for(i = 0; i < no->num_filhos; i++) {
        liberar_no(no->filhos[i]);
    }

    free(no);
}

void liberar_arvore(Arvore* arvore) {
    if(arvore == NULL) return;

    liberar_no(arvore->raiz);
    free(arvore);
}