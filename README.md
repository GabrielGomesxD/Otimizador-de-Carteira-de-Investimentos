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
