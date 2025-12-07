#ifndef OTIMIZADOR_H
#define OTIMIZADOR_H

// Definicoes
#define ATIVO 2
#define CATEGORIA 1
#define RAIZ 0

// Structs
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

#endif