# Orientações: Trabalho PID 2026, Parte 01 (Morfologia Matemática)

**Disciplina:** Processamento de Imagens Digitais, UNIOESTE, Campus Cascavel
**Professor:** Adair Santa Catarina
**Entrega:** 29/10/2026, por e-mail (adair.santacatarina@unioeste.br)
**Grupo:** até 3 integrantes. Vale 50% da nota do Trabalho 2.

Legenda usada neste documento:

- **[E]** exigido pelo enunciado
- **[D]** decisão de implementação
- **[S]** sugestão opcional

Este documento é a visão geral do trabalho. Os detalhes técnicos de cada etapa estão em `passo_a_passo_construcao_cpp.md`.

---

## 1. Objetivo

Desenvolver, em C++, um software que analisa imagens de folhas de 6 espécies (Folhado, Araçá, Quaresmeira, Pessegueiro, Coleus e Uva do mato) fotografadas sobre fundo "constante". O programa deve:

1. Permitir selecionar e abrir uma imagem.
2. Identificar cada folha presente.
3. Obter a fronteira de cada folha por Morfologia Matemática para imagens binárias.
4. Percorrer essa fronteira com o algoritmo seguidor de fronteiras de Gonzalez & Woods (4ª ed., p. 814–816).
5. Marcar os pixels da fronteira em vermelho.
6. Informar quantas folhas existem e quantos pixels tem a fronteira de cada uma.
7. Exibir uma folha por vez (`<Enter>` para avançar, ou método mais claro).

Depois, com as 36 imagens de treino, investigar se o **número de pixels da fronteira**, usado como **único descritor**, permite identificar a espécie, com conclusões baseadas nos dados.

---

## 2. Pontos centrais do enunciado

- A fronteira deve ser obtida por Morfologia Matemática binária. Outras formas de extração de borda não são consideradas. **[E]**
- O seguidor percorre a fronteira identificada por operações morfológicas. Minha leitura é que ele deve operar sobre a **máscara de fronteira morfológica**. **[E]** (confirmar com o professor)
- A biblioteca auxiliar (OpenCV) só pode ser usada para abrir imagens. Todos os algoritmos de processamento são implementados pelo grupo. **[E]**
- A medida pedida é o **número de pixels** da fronteira, e não o perímetro geométrico. **[E]**
- O descritor da análise final é **somente** o número de pixels da fronteira de cada folha. **[E]**
- A conclusão do relatório precisa estar embasada em dados e na análise deles. **[E]**
- O relatório deve conter a Declaração de Uso de IA. **[E]**
- Linguagem permitida: C, C++ ou Rust. Neste projeto: C++. **[E]**

---

## 3. Requisitos

### Funcionais [E]

| ID | Requisito |
|---|---|
| RF1 | Selecionar e abrir imagens |
| RF2 | Binarizar a imagem |
| RF3 | Obter a fronteira por Morfologia Matemática |
| RF4 | Identificar cada folha individualmente |
| RF5 | Percorrer a fronteira com o seguidor de Gonzalez & Woods |
| RF6 | Marcar a fronteira em vermelho |
| RF7 | Informar a quantidade de folhas |
| RF8 | Informar o número de pixels da fronteira de cada folha |
| RF9 | Exibir uma folha por vez com `<Enter>` ou método mais claro |
| RF10 | Processar as 36 imagens e analisar se o número de pixels da fronteira discrimina a espécie |
| RF11 | Relatório com conclusões baseadas em dados e Declaração de Uso de IA |

### Funcionais de projeto [D]

| ID | Requisito |
|---|---|
| RD1 | As imagens a processar vêm de uma **pilha de imagens** montada ao iniciar, a partir da pasta `.../imagens/` |
| RD2 | O programa processa o topo da pilha, repete, e **só termina quando o usuário fecha o aplicativo** |
| RD3 | As folhas de cada imagem ficam numa **pilha de folhas**, e o `<Enter>` alterna entre exibir o perímetro vermelho do topo e descartá-lo para selecionar a próxima |
| RD4 | As contagens de cada imagem são gravadas em arquivo logo após a varredura |
| RD5 | A análise final gera uma tabela em que cada linha é uma imagem e cada coluna é uma folha dela |

### Técnicos [E]

| ID | Requisito |
|---|---|
| RT1 | Linguagem C++ |
| RT2 | OpenCV apenas para abrir, exibir e gerenciar a janela |
| RT3 | Todos os algoritmos de processamento implementados pelo grupo |
| RT4 | Entrega por e-mail até 29/10/2026 |

---

## 4. Fluxos

### 4.1 Fluxo de execução do programa [D]

1. Inicia o programa.
2. Busca em `.../imagens/` os caminhos das imagens e os coloca em uma pilha.
3. Pega o topo da pilha e processa a imagem.
4. Repete o passo 3 até o usuário fechar o aplicativo manualmente. O programa não fecha sozinho: com a pilha vazia, avisa que terminou e continua aberto.

### 4.2 Fluxo de processamento de uma imagem [D]

1. Carrega a imagem (leitura e binarização, com folha = 1 e fundo = 0).
2. Copia a imagem binária.
3. Realiza a erosão leve na cópia.
4. Subtrai a erodida da original: **imagem de bordas** (`A − (A ⊖ B)`).
   - 4.1. Exibe a imagem resultante.
5. Faz uma varredura que identifica as folhas (seguidor de fronteira em cada componente de borda) e cria a pilha de folhas.
6. A cada `<Enter>`, alterna entre exibir em vermelho o perímetro da folha do topo e descartar o topo, selecionando a próxima.

### 4.3 Fluxo geral do trabalho

```
Pilha de imagens → (para cada imagem) carrega → binariza → copia → erosão leve
 → subtração (bordas) → exibe bordas → varredura + seguidor + contagem → pilha de folhas
 → Enter: vermelho do topo ⇄ descarta e seleciona próxima → registro das contagens
 → [após as 36 imagens] ANÁLISE: tabela imagem × folha → estatística por espécie → relatório
```

### 4.4 Distinções importantes

- **Máscara de fronteira**, **percurso ordenado** e **contagem** são três etapas diferentes e não devem ser confundidas.
- Três medidas distintas:
  1. número de pixels únicos da fronteira (**a medida pedida**);
  2. número de passos do seguidor;
  3. comprimento geométrico aproximado.
- A morfologia aparece em dois papéis: limpeza da binária (abertura, fechamento, preenchimento de buracos) e extração da fronteira (erosão e subtração).

---

## 5. Uso da OpenCV

Na dúvida, a função é classificada como "não utilizar".

| Função / recurso | Classificação | Justificativa |
|---|---|---|
| `cv::imread` | Permitida | Abrir a imagem é explicitamente permitido |
| `cv::imshow` | Permitida | Exibição, sem processamento |
| `cv::waitKey` | Permitida | Captura de tecla (`<Enter>`, Esc) e manutenção da janela |
| `cv::imwrite` | Permitida | Apenas salvar resultados |
| `cv::namedWindow` (`WINDOW_NORMAL`), `destroyAllWindows` | Permitidas | Só gerenciam a janela |
| `cv::getWindowProperty` | Permitida | Só consulta se a janela foi fechada |
| `cv::Mat` como contêiner | Permitida, com cuidado | [S] Copiar para estruturas próprias logo após o `imread` |
| Operações matriciais (`+`, `-`, `subtract`, `absdiff`, `bitwise_*`) | Não utilizar | A subtração do fluxo é parte do algoritmo; use laço próprio |
| `cv::cvtColor`, `threshold`, `adaptiveThreshold` | Não utilizar | Conversão e binarização são processamento |
| `erode`, `dilate`, `morphologyEx`, `getStructuringElement` | Não utilizar | Fazem a morfologia exigida |
| `findContours`, `drawContours` | Não utilizar | Resolvem a fronteira e o percurso |
| `connectedComponents*`, `floodFill` | Não utilizar | Resolvem a rotulação |
| `Canny`, `Sobel`, `Laplacian` | Não utilizar | Detecção de bordas proibida |
| `GaussianBlur`, `medianBlur`, `resize` | Não utilizar | Processamento não autorizado |
| `line`, `circle`, `rectangle`, `putText` | Não utilizar | Pinte pixel a pixel; escreva textos no terminal |
| Seleção de arquivo por GUI | Dúvida | Substituída pela varredura da pasta de imagens |

---

## 6. Arquitetura [D]

M�dulos independentes da interface.

| Módulo | Responsabilidade |
|---|---|
| `GerenciadorImagens` | Varre a pasta, ordena e monta a pilha de imagens |
| `ImageIO` | Abrir imagem (OpenCV), converter para estrutura própria e salvar imagens |
| `Binarizacao` | Distância de cor ao fundo, limiar e polaridade |
| `Morfologia` | Erosão, dilatação, abertura, fechamento e preenchimento de buracos |
| `ImagemBordas` | Cópia, erosão leve e subtração |
| `SeguidorFronteira` | Seguidor de Gonzalez & Woods |
| `VarreduraFolhas` | Varredura, disparo do seguidor, contagem e pilha de folhas |
| `Visualizador` | Pinta em vermelho e exibe |
| `InterfaceUsuario` | Estado do `<Enter>`, mensagens e detecção de janela fechada |
| `Registro` | Grava as contagens em `contagens.csv` |
| `Analise` | Tabela imagem × folha e estatísticas (último passo) |

### Diretórios sugeridos

```
projeto/
├── include/              # cabeçalhos dos módulos
├── src/                  # implementações + main
├── data/
│   └── imagens/          # as 36 imagens (pasta varrida pelo programa)
├── results/
│   ├── contagens.csv              # gerado pelo programa
│   ├── imagens_saida/             # perímetros em vermelho
│   ├── tabela_imagem_x_folha.csv  # gerado na análise
│   └── tabela_especies.csv        # modelo gerado; preenchido à mão
├── tests/                # imagens sintéticas e testes por etapa
├── docs/                 # relatório, notas, registro de uso de IA
└── CMakeLists.txt
```

- **[S]** Modo em lote sem janela para gerar as contagens das 36 imagens rapidamente. Ele não substitui o modo interativo exigido pelo enunciado.
- **[S]** Manter em `docs/` um registro de cada uso de IA desde o início, para facilitar a declaração final.

---

## 7. Riscos técnicos a verificar nas imagens

- **Fundo:** pode não ser idêntico entre as fotos. Estime a cor do fundo em cada imagem.
- **Coleus:** bordas verde-amareladas claras (próximas do fundo) e manchas claras internas. As manchas geram **buracos**, e cada buraco gera uma fronteira interna que seria contada como folha. Preencha os buracos antes da erosão.
- **Uva do mato:** pecíolo saliente e furos de inseto. Decida e documente se o pecíolo entra.
- **Pessegueiro e Quaresmeira:** folhas alongadas, com pontas finas que a erosão ou a abertura podem apagar.
- **Serrilhado** (Pessegueiro, Coleus): aumenta a contagem de pixels de fronteira.
- **Sombras e reflexos:** podem ser segmentados como parte da folha.
- **Folhas encostadas:** formam um único componente de borda.
- **Espécies misturadas na mesma imagem:** a espécie é atributo da folha, não da imagem.

---

## 8. Etapas de implementação

| Etapa | Conteúdo | Teste associado |
|---|---|---|
| 0 | Preparação: ambiente, CMake, OpenCV, organização, registro de uso de IA | compila e abre uma janela |
| 1 | Módulos e estruturas de dados | estruturas compilam e são testadas isoladamente |
| 2 | Carregamento e representação de pixels | Teste 1 |
| 3 | Binarização (distância de cor ao fundo, polaridade) | Teste 2 |
| 4 | Morfologia: erosão, dilatação, abertura, fechamento, preenchimento de buracos (matrizes pequenas primeiro) | Testes 3 e 4 |
| 5 | Imagem de bordas: cópia, erosão leve e subtração | Teste 5 |
| 6 | Varredura, seguidor de fronteira, contagem e pilha de folhas | Testes 6, 7 e 8 |
| 7 | Interação por `<Enter>` (alternância vermelho ⇄ descartar) | Teste 9 |
| 8 | Laço do programa: pilha de imagens, sem fechar sozinho | Teste 10 |
| 9 | Registro das contagens e (opcional) modo em lote | Teste 11 |
| 10 | **Análise (último passo):** tabela imagem × folha, estatística por espécie, relatório e entrega | revisão de conformidade |

A Etapa 4 pode ser testada em matrizes pequenas, sem depender das imagens reais. Vale começar por ela se a binarização der trabalho.

Para cada etapa, o roteiro é: objetivo, conceito, entrada, saída, estruturas de dados, pseudocódigo abstrato, cuidados, erros, testes e critérios de correção.

---

## 9. Plano de testes

| Teste | O que verificar |
|---|---|
| 1. Carregar imagem | Dimensões e canais corretos; ordem dos canais (BGR vs RGB) |
| 2. Binarização | Folha = 1, fundo = 0; mesma regra para as 36 imagens; Coleus inteiro |
| 3. Erosão e dilatação manuais | Resultado igual ao calculado à mão em matrizes pequenas |
| 4. Abertura, fechamento e buracos | Pixel isolado some; buraco é preenchido; idempotência |
| 5. Imagem de bordas | Retângulo 6×4 e quadrado 5×5 com 16 pixels de borda; máscara de 1 pixel de espessura |
| 6. Identificar folhas | 0, 1, 2 e 3 blocos sintéticos; imagem de 8 folhas gera 8 folhas |
| 7. Percorrer a fronteira | O seguidor volta ao ponto inicial e termina |
| 8. Contar pixels | Pixels únicos do percurso = tamanho do componente de borda |
| 9. Interação | Em imagem com 3 folhas: 3 exibições em vermelho, 3 descartes, depois a próxima imagem |
| 10. Laço de imagens | Ordem esperada, aviso de pilha vazia, programa continua aberto, pasta vazia tratada |
| 11. As 36 imagens | Nenhuma falha ou laço infinito; `contagens.csv` completo |

---

## 10. Experimentos com as 36 imagens

Registrar, a partir da execução real do programa (nunca inventar dados), no arquivo `contagens.csv`:

| imagem | n_folhas | folha_id | pixels_fronteira |
|---|---|---|---|
| (preencher) | (preencher) | (preencher) | (preencher) |

A tabela imagem × folha (linhas = imagens, colunas = folhas) é gerada **na análise**, a partir desse arquivo.

---

## 11. Análise (último passo)

Descritor único: **número de pixels da fronteira**.

1. **Tabela imagem × folha:** cada linha é uma imagem e cada coluna é uma folha dela (Folha 1 … Folha F_max, na ordem de descoberta da varredura). Células vazias onde a imagem tem menos folhas.
2. **Tabela de espécies:** modelo vazio com a mesma forma, preenchido à mão (a espécie é da folha, não da imagem).
3. **Estatísticas por espécie:** quantidade, média, mediana, mínimo, máximo, amplitude e desvio padrão.
4. **Distribuição e sobreposição:** gráfico por espécie e matriz de sobreposição das faixas `[mínimo, máximo]`.
5. **[S]** Regra de decisão simples em uma dimensão (média mais próxima ou faixas), avaliada com leave-one-out, matriz de confusão 6×6 e comparação com "sempre a espécie mais frequente". É análise experimental, e não parte da extração morfológica.

A conclusão deve ser escrita **depois** dos números e citá-los. Fatores a discutir: elemento estruturante, resolução e escala das fotos, pecíolo, serrilhado e número de amostras por espécie.

---

## 12. Estrutura do relatório

1. Introdução
2. Objetivos
3. Fundamentação teórica
4. Morfologia Matemática
5. Metodologia
6. Desenvolvimento do algoritmo (fluxos 4.1 e 4.2)
7. Algoritmo seguidor de fronteiras
8. Implementação em C++
9. Resultados (tabela imagem × folha)
10. Análise por espécie
11. Análise da capacidade de identificação das espécies
12. Discussão
13. Conclusão
14. Declaração de uso de IA
15. Referências

A Declaração de Uso de IA deve citar quais IAs foram utilizadas e com qual finalidade, descrevendo apenas o que realmente ocorreu (por exemplo: orientação, esclarecimento conceitual, planejamento, revisão ou documentação).

Referência principal: GONZALEZ, R. C.; WOODS, R. E. *Digital Image Processing*. 4. ed. New York: Pearson Education Limited, 2018, p. 814–816.

---

## 13. Dúvidas a esclarecer com o professor

1. O seguidor de fronteiras deve operar sobre a máscara de fronteira morfológica? (Leitura atual: sim.)
2. A binarização pode fazer parte do carregamento da imagem, antes da erosão e da subtração? (Leitura atual: sim, pois a morfologia exigida é binária.)
3. O pecíolo conta como parte da folha?
4. O fundo é de fato idêntico entre as imagens?
5. Como identificar com segurança a espécie de cada folha nas imagens com várias folhas?

---

## 14. Checklist de conformidade

Marcar cada item conforme o andamento: **A** = atendido, **N** = não atendido, **V** = precisa ser verificado experimentalmente.

| Requisito | Status | Quando verificar |
|---|:--:|---|
| C++ | [ ] A  [ ] N  [ ] V | Etapa 0 |
| OpenCV só nas funções permitidas | [ ] A  [ ] N  [ ] V | Etapas 2, 7 e 8 |
| Processamento desenvolvido pelos autores | [ ] A  [ ] N  [ ] V | Etapas 3–6 |
| Imagem binária | [ ] A  [ ] N  [ ] V | Etapa 3 |
| Morfologia Matemática | [ ] A  [ ] N  [ ] V | Etapa 4 |
| Fronteira por Morfologia Matemática (erosão e subtração) | [ ] A  [ ] N  [ ] V | Etapa 5 |
| Identificação individual das folhas | [ ] A  [ ] N  [ ] V | Etapa 6 |
| Seguidor de fronteiras (G&W, p. 814–816) | [ ] A  [ ] N  [ ] V | Etapa 6 |
| Contagem dos pixels únicos | [ ] A  [ ] N  [ ] V | Etapa 6 |
| Fronteiras em vermelho | [ ] A  [ ] N  [ ] V | Etapa 7 |
| Quantidade de folhas informada | [ ] A  [ ] N  [ ] V | Etapas 6 e 7 |
| `<Enter>` alternando exibição e descarte | [ ] A  [ ] N  [ ] V | Etapa 7 |
| Pilha de imagens e programa que não fecha sozinho | [ ] A  [ ] N  [ ] V | Etapa 8 |
| Contagens registradas por folha | [ ] A  [ ] N  [ ] V | Etapa 9 |
| Tabela imagem × folha | [ ] A  [ ] N  [ ] V | Etapa 10 |
| Análise das 36 imagens | [ ] A  [ ] N  [ ] V | Etapa 10 |
| Investigação da identificação da espécie (descritor único) | [ ] A  [ ] N  [ ] V | Etapa 10 |
| Conclusões baseadas em dados | [ ] A  [ ] N  [ ] V | Etapa 10 |
| Declaração de uso de IA | [ ] A  [ ] N  [ ] V | Etapa 10 |
| Entrega até 29/10/2026 por e-mail | [ ] A  [ ] N  [ ] V | Etapa 10 |
