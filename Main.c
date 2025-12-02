#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>

#define MAX_FILHOS 20
#define ATIVO 2
#define CATEGORIA 1
#define RAIZ 0

// ========================================
// STRUCTS (COMPARTILHADAS)
// ========================================

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

// ========================================
// DECLARAÇÕES DE FUNÇÕES
// ========================================

// Funções do Luis
No* criar_no(const char* nome, int tipo, float percentual_alvo, float valor_investido);
void adicionar_filho(No* pai, No* filho);
No* buscar_no(No* raiz, const char* nome);
float calcular_total_no(No* no);
Arvore* criar_carteira_perfil(float valor_inicial, const char* perfil);
void remover_ativo(Arvore* arvore, const char* nome);
void atualizar_percentuais(Arvore* arvore);
void listar_ativos(Arvore* arvore, const char* nome_categoria);
void liberar_no(No* no);
void liberar_arvore(Arvore* arvore);

// Funções do Gabriel
void detectar_desbalanceamento(Arvore* arvore);
void sugerir_rebalanceamento(Arvore* arvore);
void simular_aporte(Arvore* arvore, float valor_aporte);
void atualizar_valores_mercado(Arvore* arvore, char* nome_ativo, float novo_valor);

// ========================================
// IMPLEMENTAÇÕES - LUIS
// ========================================

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
    No* acoes = criar_no("Acoes", CATEGORIA, perc_rv, 0.0);
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
        printf("Ativo nao encontrado!\n");
        return;
    }

    if(ativo->tipo != ATIVO) {
        printf("Nao e um ativo!\n");
        return;
    }

    // procurar o pai desse ativo
    No* pai = NULL;
    int indice = -1;

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
        printf("Erro: pai nao encontrado!\n");
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

    printf("Ativo removido com sucesso!\n");
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
        printf("Categoria nao encontrada!\n");
        return;
    }

    if(categoria->tipo != CATEGORIA) {
        printf("Nao e uma categoria!\n");
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

// ========================================
// IMPLEMENTAÇÕES - GABRIEL
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

    int i;
    for(i = 0; i < arvore->raiz->num_filhos; i++) {
        No* categoria = arvore->raiz->filhos[i];

        float percentual_atual = (categoria->valor_total / arvore->valor_total) * 100.0;
        float diferenca = percentual_atual - categoria->percentual_alvo;

        printf("\n%s:\n", categoria->nome);
        printf("  Meta:  %.1f%%\n", categoria->percentual_alvo);
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
        printf("Sugestao: Use a opcao 8 para ver como rebalancear\n");
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

    typedef struct {
        char nome[50];
        float valor_atual;
        float valor_alvo;
        float diferenca_reais;
    } Info;

    Info categorias[20];
    int num_categorias = arvore->raiz->num_filhos;

    int i;
    for(i = 0; i < num_categorias; i++) {
        No* categoria = arvore->raiz->filhos[i];

        float percentual_atual = (categoria->valor_total / arvore->valor_total) * 100.0;
        float valor_alvo = arvore->valor_total * (categoria->percentual_alvo / 100.0);
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
        printf("\nCarteira ja esta balanceada!\n");
        printf("========================================\n");
        return;
    }

    printf("\nAcoes necessarias:\n\n");

    for(i = 0; i < num_categorias; i++) {
        if(categorias[i].diferenca_reais > (arvore->valor_total * tolerancia / 100.0)) {
            printf("* VENDER R$ %.2f de %s\n",
                   categorias[i].diferenca_reais, categorias[i].nome);
        } else if(categorias[i].diferenca_reais < -(arvore->valor_total * tolerancia / 100.0)) {
            printf("* COMPRAR R$ %.2f em %s\n",
                   -categorias[i].diferenca_reais, categorias[i].nome);
        }
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

    int i, j;
    for(i = 0; i < arvore->raiz->num_filhos; i++) {
        No* categoria = arvore->raiz->filhos[i];

        float valor_categoria = valor_aporte * (categoria->percentual_alvo / 100.0);

        printf("* %s (%.0f%%): + R$ %.2f\n",
               categoria->nome, categoria->percentual_alvo, valor_categoria);
        printf("  Novo total: R$ %.2f -> R$ %.2f\n\n",
               categoria->valor_total, categoria->valor_total + valor_categoria);

        if(categoria->num_filhos > 0) {
            int count_ativos = 0;
            for(j=0; j<categoria->num_filhos; j++)
                if(categoria->filhos[j]->tipo == ATIVO) count_ativos++;

            float valor_por_ativo = (count_ativos > 0) ? valor_categoria / count_ativos : 0.0;

            for(j = 0; j < categoria->num_filhos; j++) {
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
    if(valor_anterior != 0.0)
        variacao = ((novo_valor - valor_anterior) / valor_anterior) * 100.0;

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
// MAIN - TESTES
// ========================================

int main() {
    setlocale(LC_ALL, "Portuguese");

    printf("========================================\n");
    printf("OTIMIZADOR DE CARTEIRA DE INVESTIMENTOS\n");
    printf("========================================\n");

    // Criar carteira conservadora com R$ 10.000
    Arvore* carteira = criar_carteira_perfil(10000.0, "CONSERVADOR");

    // TESTE 1: Estado inicial
    printf("\n=== TESTE 1: Estado inicial ===\n");
    atualizar_percentuais(carteira);
    detectar_desbalanceamento(carteira);

    // TESTE 2: Valorizacao PETR4
    printf("\n=== TESTE 2: Valorizacao PETR4 (+100%%) ===\n");
    atualizar_valores_mercado(carteira, "PETR4", 2000.0);
    detectar_desbalanceamento(carteira);
    sugerir_rebalanceamento(carteira);

    // TESTE 3: Aporte R$ 1000
    printf("\n=== TESTE 3: Aporte R$ 1000 ===\n");
    simular_aporte(carteira, 1000.0);
    detectar_desbalanceamento(carteira);

    // TESTE 4: Listar ativos de Acoes
    printf("\n=== TESTE 4: Listar ativos de Acoes ===\n");
    listar_ativos(carteira, "Acoes");

    // TESTE 5: Remover ITUB4
    printf("\n=== TESTE 5: Remover ITUB4 ===\n");
    remover_ativo(carteira, "ITUB4");
    atualizar_percentuais(carteira);

    // Liberar memória
    liberar_arvore(carteira);

    printf("\n========================================\n");
    printf("Programa finalizado com sucesso!\n");
    printf("========================================\n");

    return 0;
}