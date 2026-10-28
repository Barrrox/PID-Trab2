# TODO — Entrega do Trabalho 2 (Parte 01) de PID

**Data limite:** 29/10/2026 (`adair.santacatarina@unioeste.br`)  
**Autores:** André Franco e Matheus Barros

---

## 1. Pipeline Principal e Interação (`src/main.cpp`)
- [ ] **Definir e fixar o pipeline morfológico final** em `src/main.cpp` (suavização, binarização BGR, fechamento/dilatação/erosão e subtração para extração da borda $A - (A \ominus B)$).
- [ ] **Integrar `identificar_folhas()` e `contornarBorda()`** no `src/main.cpp` (atualmente implementadas em `src/funcoes.cpp`, mas não chamadas no `main.cpp`).
- [ ] **Exibir o total de folhas detectadas** na imagem aberta no terminal.
- [ ] **Implementar exibição interativa folha a folha com `<Enter>`**:
  - Destacar em vermelho (`R=255, G=0, B=0`) os pixels da fronteira de uma folha por vez sobre a imagem original.
  - Exibir no terminal o contador de pixels da fronteira da folha atual (`Folha k de N: X pixels`).
  - Aguardar tecla `<Enter>` (código 13 ou 10 no `cv::waitKey`) para avançar para a próxima folha.

## 2. Revisão dos Algoritmos (`src/funcoes.cpp` e `src/funcoes.h`)
- [ ] **Verificar espessura da borda e contagem única:** garantir que a contagem de pixels por folha reflita exatamente os pixels únicos da fronteira percorrida pelo seguidor de Gonzalez & Woods.
- [ ] **Revisar critério de parada em `contornarBorda()`:** validar se o retorno a $b_0$ em pontas de 1 pixel precisa checar o próximo vizinho $b_1$ (Gonzalez & Woods, p. 814–816) ou deduplicar pontos repetidos.
- [ ] **Filtragem de ruído/buracos:** garantir que manchas internas (ex.: *Coleus*) ou pequenos artefatos não gerem falsas folhas na varredura de `identificar_folhas()`.
- [ ] **Corrigir retorno vazio em `binarizarBGR()`:** trocar `return {{NULL}};` (linha 582 de `src/funcoes.cpp`) por `return {};`.

## 3. Limpeza de Código e Build (`CMakeLists.txt`, `src/`, `README.md`)
- [ ] **Limpar `src/main.cpp`:** remover blocos de código comentados de testes antigos e janelas de depuração desnecessárias para a apresentação/entrega.
- [ ] **Limpar `src/funcoes.cpp`:** remover variáveis globais não utilizadas (`elemento_estruturante` nas linhas 16–50) e blocos de código morto/comentado.
- [ ] **Tornar `CMakeLists.txt` portável:** ajustar `set(OpenCV_DIR C:/Users/mathe/opencv/build)` para não quebrar a compilação na máquina do professor (usar verificação condicional `if(EXISTS ...)`).
- [ ] **Atualizar `README.md`:** deixar instruções de compilação e execução focadas na versão final de entrega (`ContadorFolhas`).
- [ ] **Remover arquivos temporários de teste** antes de gerar o pacote final de entrega.

## 4. Extração de Dados e Análise Estatística (36 Imagens)
- [ ] **Gerar tabela de contagens das 36 imagens:** registrar o número de folhas e a quantidade de pixels de fronteira de cada folha para todas as 36 imagens de treino.
- [ ] **Rotular a espécie de cada folha:** associar cada folha individual (das 36 imagens) à sua espécie (*Folhado*, *Araçá*, *Quaresmeira*, *Pessegueiro*, *Coleus*, *Uva do mato*).
- [ ] **Calcular estatísticas por espécie:** número de folhas ($N$), média, mediana, desvio padrão, mínimo, máximo e amplitude de pixels de fronteira.
- [ ] **Gerar gráficos/tabelas comparativas:** boxplot ou gráfico de dispersão/faixas `[mín, máx]` mostrando a sobreposição entre as 6 espécies.

## 5. Relatório Técnico e Entrega
- [ ] **Fundamentação e Metodologia:** descrever estimativa de fundo, binarização BGR, operações de Morfologia Matemática binária, extração de fronteira e seguidor de fronteiras de Gonzalez & Woods.
- [ ] **Resultados e Discussão:** apresentar tabelas, imagens de exemplo com bordas em vermelho e análise estatística respondendo à pergunta do enunciado (*"é possível identificar a espécie da planta usando apenas este descritor de fronteiras?"*).
- [ ] **Incluir Declaração de Uso de IA (Obrigatório):** explicitar quais ferramentas de IA foram utilizadas e com qual finalidade (ex.: esclarecimento conceitual, revisão, documentação), conforme exigido no enunciado.
- [ ] **Empacotar e enviar por e-mail** para `adair.santacatarina@unioeste.br` até **29/10/2026**.
