#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// parte do luis - arvore binaria

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

// Funcao para criar um no novo
No* criar_no(const char* nome, int tipo, float percentual_alvo, float valor_investido) {
    No* novo = (No*) malloc(sizeof(No));

    // copiar o nome
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

// Funcao para buscar um no pelo nome (recursivo)
No* buscar_no(No* raiz, const char* nome) {
    if(raiz == NULL) {
        return NULL;
    }

    // verificar se e o no atual
    if(strcmp(raiz->nome, nome) == 0) {
        return raiz;
    }

    // procurar na esquerda
    No* resultado = buscar_no(raiz->esquerda, nome);
    if(resultado != NULL) {
        return resultado;
    }

    // procurar na direita
    return buscar_no(raiz->direita, nome);
}

// Funcao para calcular o total de um no (recursivo)
float calcular_total_no(No* no) {
    if(no == NULL) {
        return 0.0;
    }

    // se nao tem filhos, retorna o valor investido
    if(no->esquerda == NULL && no->direita == NULL) {
        no->valor_total = no->valor_investido;
        return no->valor_total;
    }

    // se tem filhos, soma todos
    float soma = 0.0;
    soma = soma + calcular_total_no(no->esquerda);
    soma = soma + calcular_total_no(no->direita);

    // adiciona o valor proprio tambem
    soma = soma + no->valor_investido;
    no->valor_total = soma;

    return soma;
}

// Funcao para criar carteira por perfil
Arvore* criar_carteira_perfil(float valor_inicial, const char* perfil) {
    // alocar memoria para a arvore
    Arvore* carteira = (Arvore*) malloc(sizeof(Arvore));

    // criar no raiz
    carteira->raiz = criar_no("Carteira", RAIZ, 0.0, 0.0);

    // definir percentuais baseado no perfil
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

    // criar as categorias
    No* renda_fixa = criar_no("Renda Fixa", CATEGORIA, perc_rf, 0.0);
    No* acoes = criar_no("Acoes", CATEGORIA, perc_rv, 0.0);

    // adicionar categorias na raiz (RF esquerda, Acoes direita)
    carteira->raiz->esquerda = renda_fixa;
    carteira->raiz->direita = acoes;

    // calcular valores de cada categoria
    float valor_rf = valor_inicial * (perc_rf / 100.0);
    float valor_rv = valor_inicial * (perc_rv / 100.0);

    // criar ativos de renda fixa (dividir em 2)
    No* tesouro = criar_no("Tesouro Selic", ATIVO, 0.0, valor_rf / 2);
    No* cdb = criar_no("CDB XP", ATIVO, 0.0, valor_rf / 2);
    renda_fixa->esquerda = tesouro;
    renda_fixa->direita = cdb;

    // criar ativos de acoes (dividir em 2)
    No* petr4 = criar_no("PETR4", ATIVO, 0.0, valor_rv / 2);
    No* itub4 = criar_no("ITUB4", ATIVO, 0.0, valor_rv / 2);
    acoes->esquerda = petr4;
    acoes->direita = itub4;

    // calcular total
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

    // buscar o ativo
    No* ativo = buscar_no(arvore->raiz, nome);

    if(ativo == NULL) {
        printf("Ativo nao encontrado!\n");
        return;
    }

    if(ativo->tipo != ATIVO) {
        printf("Nao e um ativo!\n");
        return;
    }

    // simplificado: apenas zera o valor
    ativo->valor_investido = 0.0;

    // recalcular totais
    calcular_total_no(arvore->raiz);
    arvore->valor_total = arvore->raiz->valor_total;

    printf("Ativo removido com sucesso!\n");
}

// Funcao para atualizar percentuais e mostrar status
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

    // mostrar renda fixa (esquerda)
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

    // mostrar acoes (direita)
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

    // listar ativo da esquerda
    if(categoria->esquerda != NULL) {
        count++;
        printf("%d. %s - R$ %.2f\n", count, categoria->esquerda->nome, categoria->esquerda->valor_investido);
    }

    // listar ativo da direita
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