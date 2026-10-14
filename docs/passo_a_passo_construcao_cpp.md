# Passo a passo para construir o software em C++ (versão por pilhas)

Guia técnico para a construção do programa. O **único descritor** usado para distinguir as espécies é o **número total de pixels da fronteira de cada folha**. A análise é o **último passo** do trabalho.

Legenda: **[E]** exigido pelo enunciado, **[D]** decisão de implementação, **[S]** sugestão opcional.

---

## 1. Visão geral

### 1.1 Fluxo de execução do programa

1. Inicia o programa.
2. O programa busca na pasta `.../imagens/` os caminhos das imagens a carregar e os coloca em uma **pilha de imagens**.
3. Pega o topo da pilha e processa a imagem (fluxo 1.2).
4. Repete o passo 3 até o usuário **fechar o aplicativo manualmente**. O programa **não deve encerrar sozinho**: com a pilha vazia, ele avisa que terminou e continua aberto, aguardando.

### 1.2 Fluxo de processamento de uma imagem

1. Carrega a imagem.
2. Copia a imagem.
3. Realiza a operação de **erosão (leve)** na cópia.
4. Subtrai a imagem erodida da original, obtendo a **imagem de bordas**.
   - 4.1. Exibe a imagem resultante.
5. Faz uma **varredura** que identifica as folhas e cria uma **pilha de processamento** (pilha de folhas).
6. A cada **Enter** pressionado, alterna entre:
   - (a) exibir em vermelho o perímetro da folha que está no topo da pilha;
   - (b) descartar o topo e selecionar a próxima folha.

### 1.3 Fluxo completo (diagrama textual)

```
Inicia
 → lista arquivos de .../imagens/ → pilha de imagens
 → enquanto o usuário não fechar o app:
      se pilha de imagens não vazia:
          topo ← desempilha
          1. carrega (imread → estrutura própria → binária)
          2. copia a binária
          3. erosão leve na cópia
          4. bordas = original − erodida
          4.1 exibe bordas
          5. varredura: seguidor de fronteira em cada componente → pilha de folhas
             (registra as contagens da imagem)
          6. laço de Enter: [exibir vermelho do topo] ⇄ [descartar topo, selecionar próxima]
             até a pilha de folhas esvaziar
      senão:
          avisa "todas as imagens processadas; feche o aplicativo" e continua aguardando
 → (fora do programa) Análise final: tabela imagem × folha → estatística por espécie → relatório
```

### 1.4 Ajustes de interpretação que adotei (confira com o professor)

1. **O "carregar" inclui a binarização.** Erosão e subtração só fazem sentido como Morfologia Matemática **para imagens binárias** (o enunciado exige isso) se a imagem copiada, erodida e subtraída for a **binária** (folha = 1, fundo = 0). Se a imagem colorida fosse erodida e subtraída, o resultado seria um gradiente morfológico em níveis de cinza, e não a fronteira binária.
2. **A subtração é binária:** `bordas = 1` onde a original vale 1 e a erodida vale 0. É feita com laço seu, sem `cv::subtract`.
3. **A varredura do passo 5 aciona o seguidor de fronteiras.** Cada folha corresponde a um **componente conexo da imagem de bordas** (uma curva fechada). A varredura encontra o primeiro pixel de borda ainda não visitado e dispara o seguidor de Gonzalez & Woods, que percorre aquela fronteira. Assim a identificação da folha, o percurso e a contagem acontecem juntos.
4. **As contagens são registradas no passo 5**, antes dos Enters. Assim os resultados são salvos mesmo que o usuário feche o programa no meio da imagem.
5. **Leitura do passo 6:** o primeiro Enter exibe o perímetro vermelho do topo; o segundo descarta esse topo e seleciona a próxima folha (a imagem volta a ser exibida sem vermelho); o terceiro exibe a nova folha, e assim por diante.
6. Pontos a confirmar com o professor: se o seguidor deve percorrer a **máscara de bordas morfológica** (minha leitura) e se o pecíolo faz parte da folha.
7. **Anotação das espécies com pipeline compartilhado:** como espécies podem se misturar na mesma imagem, a espécie é anotada por `(imagem, folha_id)`. A anotação é um processo separado da medição, mas ambos usam a **mesma função** `detectar_folhas()`, para que os ids coincidam (detalhes na Etapa 6.3 e na Etapa 10.1).

---

## 2. Tecnologias e bibliotecas

### 2.1 Ferramentas

| Item | Escolha | Observação |
|---|---|---|
| Linguagem | C++17 | `std::filesystem`, `std::stack` |
| Compilador | g++ 9+, clang++ 10+ ou MSVC 2019+ | `-Wall -Wextra` |
| Build | CMake 3.16+ | Localiza a OpenCV em qualquer sistema |
| Depuração | gdb/lldb ou depurador do IDE | [S] AddressSanitizer (`-fsanitize=address,undefined`) |
| Versionamento | Git | [S] Um commit por etapa concluída |

### 2.2 OpenCV: só entrada, saída e janela [E]

M�dulos a linkar: `opencv_core`, `opencv_imgcodecs`, `opencv_highgui`.

| Função | Uso | Situação |
|---|---|---|
| `cv::imread` | Abrir a imagem | **Permitida** |
| `cv::imshow` | Exibir | **Permitida** |
| `cv::waitKey` | Ler Enter/Esc e manter a janela viva | **Permitida** |
| `cv::imwrite` | Salvar imagens de saída | **Permitida** |
| `cv::namedWindow` (flag `WINDOW_NORMAL`) | Janela redimensionável pelo usuário | **Permitida** (só janela) |
| `cv::getWindowProperty` (propriedade de visibilidade) | Detectar que o usuário fechou a janela | **Permitida** (só consulta o estado da janela) |
| `cv::destroyAllWindows` | Encerramento | **Permitida** |
| `cv::Mat` | Contêiner na leitura e na exibição | **Permitida** (copiar logo para estruturas próprias) |
| `cvtColor`, `threshold`, `adaptiveThreshold` | Conversão/segmentação | **Não utilizar** |
| `erode`, `dilate`, `morphologyEx`, `getStructuringElement` | Morfologia | **Não utilizar** |
| `subtract`, `absdiff`, `bitwise_*`, operadores `+` `-` entre matrizes | A subtração é o passo 4 do fluxo | **Não utilizar** (laço próprio) |
| `findContours`, `drawContours`, `connectedComponents*`, `floodFill` | Fronteira e rotulação | **Não utilizar** |
| `Canny`, `Sobel`, `Laplacian`, `GaussianBlur`, `medianBlur` | Bordas e filtros | **Não utilizar** |
| `resize` | Redimensionar | **Não utilizar** (use `WINDOW_NORMAL` ou redução por amostragem sua) |
| `line`, `circle`, `rectangle`, `putText` | Desenho/texto | **Não utilizar** (pinte pixel a pixel; escreva textos no terminal) |

Cuidados:

- `imread` devolve os canais na ordem **B, G, R**.
- Ao copiar um `cv::Mat` com atribuição, os dois compartilham os mesmos dados. Como você copiará para suas próprias estruturas (vetores), a cópia do passo 2 do fluxo será profunda automaticamente.
- Se o ambiente não tiver interface gráfica, `imshow` falha. Use `imwrite` para depurar.

### 2.3 Biblioteca padrão do C++ (todo o resto)

| Cabeçalho | Uso |
|---|---|
| `<stack>` | Pilha de imagens e pilha de folhas (ou `std::vector` com `push_back`/`back`/`pop_back`) |
| `<filesystem>` | Listar a pasta de imagens e criar pastas de saída |
| `<vector>`, `<utility>`, `<cstdint>` | Matrizes, pares de coordenadas, tipos de tamanho fixo |
| `<algorithm>` | Ordenar os caminhos antes de empilhar |
| `<queue>` | Fila usada no preenchimento de buracos (Etapa 4) e na consumação de componentes (Etapa 6) |
| `<fstream>`, `<sstream>` | Gravar e ler os CSV |
| `<cmath>`, `<numeric>` | Estatísticas da análise final |
| `<iostream>`, `<iomanip>` | Terminal: contadores e mensagens |

**Não utilizar:** Eigen, Boost.Geometry, scikit-image, PIL ou qualquer outra biblioteca de imagem para executar os algoritmos.

### 2.4 Instalação da OpenCV

- **Ubuntu/Debian/WSL:** `build-essential`, `cmake`, `pkg-config` e `libopencv-dev` pelo `apt`.
- **macOS:** `brew install opencv cmake`.
- **Windows:** MSYS2/MinGW (`mingw-w64-x86_64-opencv`) ou `vcpkg install opencv4` com o toolchain do vcpkg indicado ao CMake.

---

## 3. Caso de referência: imagem com várias folhas (exemplo com 8 folhas)

O projeto deve ser pensado para imagens com **várias folhas**, como a imagem de exemplo com 8 folhas sobre fundo branco (confira com o seu arquivo).

| Observação na imagem | Consequência |
|---|---|
| 8 folhas separadas, sem se tocar, aparentemente sem tocar a borda da imagem | A varredura deve encontrar **8 componentes de borda**. É o resultado esperado e o teste de referência |
| Espécies **misturadas** na mesma imagem | A espécie é atributo da **folha**, não da imagem. Por isso a análise usa uma tabela de espécies com a mesma forma da tabela de contagens |
| Fundo claro e quase uniforme, com leve variação de iluminação | Binarizar pela **distância de cor ao fundo** |
| Coleus com bordas verde-amareladas claras e manchas claras dentro da região escura | Bordas claras exigem a distância de cor. Manchas internas geram buracos e, depois, **fronteiras internas que apareceriam como "folhas falsas"**. Preencha os buracos |
| Pecíolos finos (Uva do mato e outras) | A abertura pode apagá-los. Defina uma regra única |
| Bordas serrilhadas (Pessegueiro, Coleus) | Muitos pixels de fronteira. Evite limpezas que alisem o serrilhado sem necessidade |
| Resolução alta (~2300 × 1600, ≈ 3,7 milhões de pixels) | O número de pixels de fronteira depende da resolução: use sempre a original |

---

## Etapa 0. Preparação do projeto

**Objetivo:** projeto que compila, linka a OpenCV e abre uma imagem.

**Estrutura [D]:**

```
projeto/
├── CMakeLists.txt
├── include/          (um .hpp por módulo)
├── src/              (um .cpp por módulo + main.cpp)
├── data/
│   └── imagens/      (as 36 imagens; a pasta que o programa varre)
├── results/
│   ├── contagens.csv              (gerado pelo programa)
│   ├── imagens_saida/             (imagens com o perímetro em vermelho)
│   ├── tabela_imagem_x_folha.csv  (gerado na Etapa 10)
│   └── tabela_especies.csv        (modelo gerado; preenchido à mão)
├── tests/
└── docs/
```

**O que o CMakeLists deve declarar** (para você escrever): versão mínima e nome do projeto; padrão C++17 obrigatório; `find_package` da OpenCV com os componentes `core`, `imgcodecs` e `highgui`; uma biblioteca estática com os módulos de `src/` (menos o `main`); um executável linkado à biblioteca e à OpenCV; `include/` nos caminhos de cabeçalho; flags de aviso.

**Critério de correção:** o CMake configura, o executável compila e uma janela mostra uma imagem de teste.

**Erros comuns:** OpenCV não encontrada (indique o caminho do pacote ao CMake), mistura de OpenCV 3 e 4, caminho relativo resolvido na pasta errada.

---

## Etapa 1. Módulos e estruturas de dados

**Módulos [D]:**

| Módulo | Responsabilidade |
|---|---|
| `GerenciadorImagens` | Varre a pasta, ordena e monta a pilha de imagens |
| `ImageIO` | `imread` → estrutura própria; salvar imagens com `imwrite` |
| `Binarizacao` | Distância de cor ao fundo, limiar e polaridade |
| `Morfologia` | Erosão, dilatação, abertura, fechamento, preenchimento de buracos |
| `ImagemBordas` | Cópia, erosão leve e subtração (passos 2 a 4) |
| `SeguidorFronteira` | Seguidor de Gonzalez & Woods |
| `VarreduraFolhas` | Varredura, disparo do seguidor, pilha de folhas. Expõe `detectar_folhas()`, compartilhada entre medição e anotação |
| `Visualizador` | Pinta em vermelho, converte para exibição, mostra janelas |
| `InterfaceUsuario` | Estado do Enter, mensagens, detecção de janela fechada |
| `Registro` | Grava `contagens.csv` |
| `Analise` | Etapa 10 (executável separado ou modo do programa) |

**Estruturas [D]:**

| Estrutura | Conteúdo | Observação |
|---|---|---|
| `ImagemRGB` | largura, altura, `std::vector<uint8_t>` de tamanho `L × A × 3` | Índice `(y·L + x)·3 + canal`. Use função de acesso `(x, y)` |
| `ImagemBinaria` | largura, altura, vetor de `uint8_t` com 0/1 | Usada para original binária, erodida e bordas |
| `ElementoEstruturante` | lista de pares `(dx, dy)` | Aceita formatos arbitrários |
| `Folha` | `id`, ponto inicial `b0`, lista **ordenada** do percurso, `pixels_unicos` | Resultado do seguidor |
| Pilha de imagens | `std::stack` de caminhos (`std::filesystem::path`) | LIFO: veja a ordem na Etapa 8 |
| Pilha de folhas | `std::stack<Folha>` | Mantenha também o total N em uma variável, pois `std::stack` não permite percorrer |
| `ResultadoImagem` | nome da imagem e vetor de contagens (uma por folha, na ordem de descoberta) | Gravado no passo 5 |

Índices lineares: com ~3,7 milhões de pixels e 3 canais, o índice passa de 11 milhões. Use `size_t` ou inteiro de 32 bits seguro.

---

## Etapa 2. Carregamento da imagem (passo 1 do fluxo, parte 1)

**Objetivo:** ler o arquivo e obter uma `ImagemRGB` própria.

```
abrir arquivo com OpenCV
se falhar: avisar no terminal e PULAR para a próxima imagem (sem fechar o programa)
criar ImagemRGB com as mesmas dimensões
para cada pixel (y, x):
    ler B, G, R do cv::Mat e gravar na ImagemRGB na ordem escolhida
```

**Cuidados:** conferir que a imagem tem 3 canais; documentar a ordem dos canais da sua estrutura (R, G, B é a mais intuitiva).

**Teste 1:** imprimir largura, altura e os valores do pixel (0,0) e do pixel central. Mostrar a cópia numa janela: se vier azulada, os canais estão trocados.

---

## Etapa 3. Binarização (passo 1 do fluxo, parte 2)

**Objetivo:** produzir `ImagemBinaria` com **folha = 1** e **fundo = 0**.

**Por que não usar só o cinza?** O Coleus tem bordas verde-amareladas claras, cujo cinza fica próximo ao do fundo branco. A **distância de cor ao fundo** as enxerga. A conversão para cinza fica opcional (por exemplo, para um histograma de inspeção).

**Estimativa da cor do fundo [D]:** mediana (ou média) **por canal** de uma faixa da borda da imagem. A mediana é mais robusta a sombras e a uma folha que encoste na borda. Se o fundo tiver gradiente, estime por região (por exemplo, quadrantes).

**Medida de distância [D]:**

| Medida | Descrição |
|---|---|
| Maior diferença absoluta entre canais | `max(|R−Rf|, |G−Gf|, |B−Bf|)` |
| Soma das diferenças absolutas | `|R−Rf| + |G−Gf| + |B−Bf|` |
| Euclidiana | Raiz da soma dos quadrados (compare o quadrado com o quadrado da tolerância para evitar a raiz) |

**Tolerância:** observe o histograma das distâncias. O fundo forma um pico perto de 0, e a folha fica bem acima. A tolerância deve ficar no vale entre os dois. Use **uma única regra** para as 36 imagens.

```
estimar cor de referência do fundo (mediana por canal numa faixa da borda)
para cada pixel:
    d = distância entre a cor do pixel e a de referência
    se d > tolerância: binária = 1 (folha) senão 0 (fundo)
```

**Cuidados:**

- **Polaridade:** o fundo deve ser a classe majoritária nas bordas. Se a folha ficar 0, todo o resto sai invertido.
- A limpeza de ruído e o preenchimento de buracos ocorrem na Etapa 4, depois que a morfologia existir.

**Teste 3:** salvar a binária (0 → preto, 1 → branco) das 36 imagens e conferir em miniaturas se todas ficaram razoáveis com a mesma regra. Na imagem de 8 folhas, conferir os dois Coleus.

---

## Etapa 4. Morfologia Matemática binária [E]

**Objetivo:** implementar erosão, dilatação, abertura, fechamento e preenchimento de buracos.

**Fundamentos:**

- Imagem binária A = conjunto de pixels com valor 1; elemento estruturante B = conjunto de deslocamentos.
- **Erosão** `A ⊖ B`: pixels `p` tais que B posicionado em `p` fica **inteiramente contido** em A.
- **Dilatação** `A ⊕ B`: pixels `p` tais que B posicionado em `p` **toca** A.
- **Abertura** = erosão e depois dilatação (remove ruído e pontas finas). **Fechamento** = dilatação e depois erosão (preenche buracos pequenos).

**Elementos estruturantes usuais:**

| Elemento | Offsets | Efeito na fronteira (Etapa 5) |
|---|---|---|
| Cruz 3×3 (4-vizinhos) | 5 | Fronteira mais fina, conexa por 8-vizinhança |
| Quadrado 3×3 (8-vizinhos) | 9 | Fronteira mais espessa, conexa por 4-vizinhança, mais pixels |

**Erosão (pseudocódigo):**

```
saída ← imagem zerada (separada da entrada)
para cada pixel p:
    contido ← verdadeiro
    para cada offset (dx, dy) do elemento:
        q ← p + (dx, dy)
        se q fora da imagem: tratar conforme a política de borda
        senão se entrada[q] == 0: contido ← falso; parar
    se contido: saída[p] ← 1
```

A dilatação é análoga, trocando "todos" por "algum".

**Política de borda [D]:** pixels fora da imagem contam como 0. Documente o efeito caso alguma folha toque a borda.

**Limpeza da binária (antes do passo 2 do fluxo) [D]:**

- **Ruído:** abertura com elemento pequeno remove pontos isolados.
- **Buracos dentro da folha** (manchas claras do Coleus, brilhos, furos de inseto): preencha os buracos, senão cada um gera uma fronteira interna que seria contada como folha. Método: percorra o **fundo** com uma fila (busca em largura, 4-vizinhança) partindo dos pixels de fundo na borda da imagem. Todo pixel de fundo **não alcançado** é buraco e passa a valer 1.
- **Pecíolos:** uma abertura 3×3 remove estruturas com menos de 3 pixels de largura. Defina se o pecíolo faz parte da folha e aplique a **mesma regra** em todas as imagens.
- **Serrilhado:** elementos grandes alisam os dentes e reduzem a contagem. Use elementos pequenos e registre o efeito.

**Testes 4 e 5 (à mão, em matrizes pequenas):**

1. Quadrado 5×5 de 1s erodido por 3×3 vira quadrado 3×3.
2. O mesmo quadrado dilatado por 3×3 vira 7×7.
3. Pixel isolado some na erosão e na abertura.
4. Buraco de 1 pixel é preenchido pelo fechamento e pelo preenchimento de buracos.
5. Idempotência: abertura aplicada duas vezes dá o mesmo resultado que uma vez.

Compare a saída do programa com o resultado calculado no papel.

---

## Etapa 5. Imagem de bordas (passos 2, 3, 4 e 4.1 do fluxo) [E]

**Objetivo:** obter a máscara de fronteira de **todas** as folhas por Morfologia Matemática: `bordas = A − (A ⊖ B)`.

```
cópia ← cópia profunda da binária                          (passo 2)
erodida ← erosão da cópia com elemento B pequeno, 1 vez     (passo 3, "erosão leve")
para cada pixel p:                                          (passo 4)
    bordas[p] ← 1 se binária[p] == 1 e erodida[p] == 0, senão 0
exibir bordas (0 → preto, 1 → branco)                       (passo 4.1)
```

**Decisões [D]:**

- **"Leve"** = elemento 3×3 e **uma única iteração**. Recomendo começar com a **cruz 3×3**, que gera fronteira de 1 pixel de espessura. Se usar o quadrado 3×3, a contagem final será maior. **Escolha um elemento e use-o em todas as imagens**; ele afeta diretamente os números da análise.
- Como a erodida está contida na original (o elemento contém a origem), a diferença nunca fica negativa.
- Folhas muito finas podem desaparecer na erosão; nesse caso todos os pixels delas viram borda.

**Teste 6:**

- Retângulo cheio 6×4: 16 pixels de borda, tanto com a cruz quanto com o quadrado.
- Quadrado 5×5: 16 pixels de borda.
- Pixel isolado: a erosão o apaga, e a borda é o próprio pixel (1 pixel).
- Forma em "L" ou em losango, para ver a diferença entre cruz e quadrado.
- Imagem real: a máscara deve ter 1 pixel de espessura e formar curvas fechadas ao redor de cada folha.

---

## Etapa 6. Varredura, seguidor de fronteira e pilha de folhas (passo 5 do fluxo) [E]

**Objetivo:** identificar cada folha, percorrer ordenadamente sua fronteira e contar seus pixels.

### 6.1 Três etapas que não se confundem

| Etapa | O que é | Onde |
|---|---|---|
| Máscara de fronteira | Conjunto de pixels de borda (sem ordem) | Etapa 5 |
| Percurso ordenado | Sequência de pixels visitados pelo seguidor | Esta etapa |
| Contagem | Pixels **únicos** do percurso | Esta etapa |

### 6.2 Seguidor de fronteiras (Gonzalez & Woods, p. 814–816)

Consulte o livro nas páginas indicadas e implemente conforme a descrição de lá. Resumo conceitual (vizinhança-8, estilo Moore):

1. O ponto inicial `b0` é o primeiro pixel de borda da varredura (linha de cima para baixo, esquerda para a direita). O vizinho a oeste de `b0` é fundo; chame-o `c0`.
2. Percorra os 8 vizinhos de `b0` em sentido fixo, começando em `c0`. O primeiro que for de borda é `b1`; o vizinho de fundo imediatamente anterior vira o novo `c`.
3. Repita a partir de cada novo `b`, sempre no mesmo sentido, começando no `c` atual.
4. **Parada:** quando o algoritmo retorna a `b0` e o próximo ponto encontrado é `b1`. Comparar só "voltou a b0" pode falhar quando o percurso passa duas vezes pelo mesmo pixel.

```
b ← b0; c ← c0; percurso ← [b0]
repetir:
    vizinhos ← os 8 vizinhos de b em ordem circular, começando em c
    para cada vizinho, na ordem:
        se vizinho é pixel de borda:
            novo_b ← vizinho; novo_c ← vizinho anterior na lista
            parar a busca
    se (novo_b == b0) e (próximo ponto seria b1): terminar
    senão: b ← novo_b; c ← novo_c; acrescentar b ao percurso
```

**Relação com a máscara:** a imagem de entrada do seguidor é a **imagem de bordas da Etapa 5**. Como ela já tem 1 pixel de espessura, o percurso acompanha a própria máscara. Em pontas finas e ramificações (ponta do Pessegueiro, pecíolo), o percurso passa pelo mesmo pixel mais de uma vez, e por isso a contagem usa pixels únicos.

**Casos especiais:**

- Borda de 1 pixel: sem vizinhos de borda, o algoritmo termina imediatamente.
- Borda de 2 pixels: teste a parada manualmente.
- **Proteção contra laço infinito [D]:** limite de passos (por exemplo, 4 × o tamanho do componente de borda). Se ultrapassar, aborte aquela folha com mensagem e continue.

### 6.3 Varredura que cria a pilha de folhas

```
visitado ← matriz zerada; descobertas ← lista vazia
para y de cima para baixo:
    para x da esquerda para a direita:
        se bordas[x,y] == 1 e visitado[x,y] == 0:
            percurso ← SEGUIDOR(bordas, ponto inicial (x,y))
            únicos ← 0
            para cada pixel do percurso:
                se visitado[pixel] == 0: visitado[pixel] ← 1; únicos++
            consumir o restante do componente (fila, 8-vizinhança), marcando visitado;
                contar os pixels do componente que o seguidor não visitou
            se tamanho do componente < L_min: descartar como ruído
            senão: guardar Folha{id = descobertas.tamanho()+1, b0, percurso, únicos}
registrar as contagens da imagem, na ordem de descoberta
empilhar as folhas de modo que o id 1 fique no TOPO (empilhar em ordem inversa à de descoberta)
```

**Decisões [D]:**

- **Ordem dos ids:** é a ordem da varredura (a folha cujo ponto mais alto vem primeiro). É determinística, e é ela que define as colunas da tabela final. Se preferir outra regra (por exemplo, da esquerda para a direita), aplique-a por ordenação depois da varredura, mas **nunca mude a regra depois de anotar as espécies**.
- **Função única compartilhada [D]:** encapsule o pipeline até aqui (binarização, limpeza, bordas, varredura, filtro `L_min`, ids) em uma só função, por exemplo `std::vector<Folha> detectar_folhas(const ImagemRGB&)`. Os modos de medição (interativo e lote) e de anotação das espécies (Etapa 10) **chamam a mesma função**, o que garante que o `folha_id` anotado é o mesmo medido.
- **Evitar duplicidade:** a matriz `visitado` impede que a mesma folha seja descoberta duas vezes. Ela também serve para contar os pixels únicos.
- **Ruído:** use `L_min`, um comprimento mínimo do componente de borda (em pixels), e **escreva no terminal os componentes descartados**. É um filtro sobre a fronteira, não uma medida de tamanho da folha.
- **Validação cruzada:** `únicos` do seguidor deve ser igual ao tamanho do componente de borda. Se diferir, emita um aviso (ramificações ou parada precoce). A medida pedida pelo enunciado é a de `únicos` do percurso.
- **Folhas que se tocam** formam um só componente. Trate como limitação documentada.
- **Folhas com buracos não preenchidos** geram componentes extras (folhas falsas). Se aparecerem, volte à Etapa 4.

### 6.4 As três medidas

| Grandeza | Como obter | Uso |
|---|---|---|
| **Pixels únicos da fronteira** | Pixels distintos do percurso | **A medida pedida e o descritor do trabalho** |
| Passos do seguidor | Tamanho do percurso (com repetições) | Diagnóstico |
| Comprimento geométrico | Soma de 1 ou √2 por passo | **Não substituir a medida pedida** |

**Teste 7 (identificar folhas):** imagens sintéticas com 0, 1, 2 e 3 blocos separados e um caso de dois blocos encostados pela diagonal. Em imagem real: a imagem de 8 folhas deve gerar **8 folhas**; qualquer diferença deve ser investigada antes de seguir.

**Teste 8 (percorrer):** quadrado 5×5 (anel de 16 pixels): o seguidor devolve os 16, em ordem circular, e para. Forma em "L" e linha fina de 1 pixel de espessura.

**Teste 9 (contar):** retângulo, círculo sintético e forma com duas pontas, com contagem feita à mão.

---

## Etapa 7. Interação por Enter (passo 6 do fluxo) [E]

**Objetivo:** mostrar uma folha por vez, com o contador.

**Estados [D]:**

| Estado | Ao pressionar Enter |
|---|---|
| `MOSTRAR` | Desenha em vermelho o perímetro da folha do **topo** sobre a imagem original colorida, exibe e escreve no terminal `Folha k de N: P pixels`. Passa para `AVANÇAR` |
| `AVANÇAR` | **Descarta o topo** e **seleciona a próxima folha** (novo topo). Exibe a imagem original sem vermelho e escreve `Próxima folha selecionada`. Passa para `MOSTRAR` |

Se, ao descartar, a pilha de folhas ficar vazia, a imagem terminou e o programa volta ao passo 3 do fluxo de execução (próxima imagem da pilha de imagens).

```
estado ← MOSTRAR; imagem_exibida ← bordas ainda na tela (passo 4.1)
enquanto pilha de folhas não vazia:
    esperar Enter (verificando também se a janela foi fechada)
    se estado == MOSTRAR:
        copiar a original colorida; pintar em vermelho (R=255, G=0, B=0) cada pixel do percurso do topo
        exibir; imprimir "Folha k de N: P pixels"; estado ← AVANÇAR
    senão:
        desempilhar o topo; exibir a original sem vermelho; estado ← MOSTRAR
```

**Cuidados:**

- Pinte pixel a pixel, com laço seu. Para enxergar melhor, [S] pinte uma espessura maior (3×3) **só na cópia exibida**, sem afetar a contagem.
- Imagens maiores que a tela: use `WINDOW_NORMAL` para o usuário redimensionar a janela.
- `waitKey` devolve 13 para Enter (e 10 em alguns sistemas): aceite ambos.
- [S] Salve com `imwrite` a imagem com o perímetro vermelho de cada folha (`<imagem>_folhaK.png`, com o mesmo `K` do `folha_id`). Ela serve para o relatório e para identificar a espécie de cada folha na Etapa 10 (modo de anotação, que reutiliza `detectar_folhas()`).
- [S] Imprima também as coordenadas de `b0`, para localizar a folha na imagem.

**Teste 10:** imagem com 3 folhas: 3 exibições em vermelho, 3 descartes, depois a próxima imagem. A numeração `k de N` deve bater com a imagem.

---

## Etapa 8. Laço do programa e pilha de imagens (fluxo de execução) [E]

**Objetivo:** montar a pilha de imagens e rodar o laço que só termina quando o usuário fechar o aplicativo.

```
caminhos ← arquivos de .../imagens/ com extensão de imagem (.jpg, .jpeg, .png, .bmp; sem diferenciar maiúsculas)
ordenar os caminhos em ordem DECRESCENTE e empilhar, de modo que o primeiro em ordem alfabética fique no topo
enquanto o usuário não fechou o app:
    se pilha de imagens não vazia:
        caminho ← desempilhar; processar_imagem(caminho)       (fluxo 1.2)
    senão:
        (uma vez) avisar "Todas as imagens foram processadas. Feche o aplicativo para sair."
        continuar aguardando, sem encerrar
```

**Como detectar que o usuário fechou o app [D]:**

- Use `waitKey` com **tempo curto** (por exemplo, 30 ms) dentro de um laço, em vez de `waitKey(0)`, e a cada volta consulte se a janela ainda está visível. Assim o fechamento pelo botão da janela é percebido.
- Aceite também Esc como saída explícita [S].
- Pilha vazia **não** é motivo para encerrar. Mantenha a janela aberta com a última imagem ou com uma mensagem.

**Cuidados:**

- Pasta inexistente ou vazia: avise no terminal e continue aguardando.
- Arquivo que não é imagem ou que falha ao abrir: avise e passe para o próximo.
- O usuário pode fechar a janela no meio de uma imagem; encerre com `destroyAllWindows` e garanta que `contagens.csv` já foi gravado (isso acontece no passo 5, por isso o registro lá).

**Teste 11:** pasta com 2 imagens (e depois com as 36): processa na ordem esperada, avisa o fim da pilha e continua aberto. Pasta vazia: mensagem e programa aberto.

---

## Etapa 9. Registro dos resultados

**Objetivo:** guardar as contagens de cada imagem para a análise final.

**Formato de `results/contagens.csv` (formato longo) [D]:** uma linha por folha, com `imagem,n_folhas,folha_id,pixels_fronteira`.

**Quando gravar:** logo após a varredura (passo 5), antes dos Enters.

**Cuidados:**

- Se uma imagem for processada mais de uma vez, **substitua** os registros anteriores dela (ou, na análise, mantenha o último), para não duplicar.
- Use o mesmo `folha_id` da ordem de descoberta da Etapa 6.
- Todas as imagens devem ser processadas com **os mesmos parâmetros** (tolerância, elemento estruturante, `L_min`, política de buracos e pecíolo). Se mudar algum, refaça tudo.
- [S] **Modo em lote sem janela** (por exemplo, uma opção na linha de comando), que executa os passos 1 a 5 e o registro para todas as imagens sem exibir nada e sem Enter. Reaproveita os mesmos módulos e gera as 36 linhas de contagens rapidamente. Ele não substitui o modo interativo exigido pelo enunciado.

---

## Etapa 10. Análise (último passo)

**Objetivo:** responder, com os dados reais, se o número de pixels da fronteira permite identificar a espécie. Esta etapa só começa depois que as 36 imagens foram processadas.

### 10.1 Criação da tabela imagem × folha

**Estrutura da Tabela 1 (contagens, gerada pelo programa):** cada **linha** é uma imagem e cada **coluna** é uma folha dessa imagem.

| imagem | n_folhas | Folha 1 | Folha 2 | Folha 3 | … | Folha F_max |
|---|---|---|---|---|---|---|
| (nome da imagem 1) | (n) | (pixels) | (pixels) | (pixels) | … | (vazio se não existir) |
| (nome da imagem 2) | (n) | (pixels) | (pixels) | (vazio) | … | (vazio) |
| … | | | | | | |
| (nome da imagem 36) | (n) | (pixels) | … | | | |

Os valores vêm exclusivamente de `contagens.csv`. Nenhum número deve ser digitado ou inventado.

**Algoritmo de geração (pseudocódigo):**

```
registros ← ler contagens.csv (imagem, folha_id, pixels_fronteira)
se uma mesma (imagem, folha_id) aparecer mais de uma vez: manter a última ocorrência
agrupar por imagem: cada imagem recebe um vetor indexado por folha_id
F_max ← maior n_folhas entre as imagens
imagens ← lista de imagens em ordem alfabética
escrever cabeçalho: imagem; n_folhas; Folha 1; …; Folha F_max
para cada imagem:
    linha ← nome; n_folhas
    para k de 1 a F_max:
        se k <= n_folhas da imagem: acrescentar a contagem da folha k
        senão: acrescentar célula vazia
    escrever linha
```

**Decisões [D]:**

- **Ordem das colunas** = ordem de descoberta da varredura (Etapa 6). Ela deve ser a mesma da execução interativa.
- **Células vazias:** imagens com menos folhas que `F_max` ficam com células em branco (não use 0, para não confundir com contagem real).
- **Separador do CSV:** `;` costuma abrir direto no Excel em configuração brasileira; `,` é o padrão internacional. Escolha um e mantenha.
- **Largura da tabela:** com uma imagem de 8 folhas, `F_max` será pelo menos 8.

**Validações:**

- Número de linhas = número de imagens processadas (36).
- Para cada linha, a quantidade de células preenchidas = `n_folhas`.
- Nenhuma contagem igual a 0 e nenhuma duplicada.
- Verificação visual: algumas linhas conferidas contra a execução interativa (os números impressos no terminal).

**Tabela 2 (espécies, preenchida à mão), com a mesma forma:** o programa gera um **modelo vazio** com as mesmas linhas e colunas da Tabela 1. Para cada célula, anote a espécie (use o vocabulário fixo: Folhado, Araçá, Quaresmeira, Pessegueiro, Coleus, Uva do mato). Para saber qual folha é qual, use as imagens com o perímetro vermelho salvas na Etapa 7 e a Figura 1 do enunciado. Cuidado: uma mesma imagem pode misturar espécies. Em caso de dúvida, confirme com o professor ou com a organização dos arquivos de treino.

**Importante:** como os `folha_id` dependem dos parâmetros e da ordem da varredura, **não altere** esses parâmetros depois de preencher a Tabela 2. Se alterar, refaça a anotação.

**Processo de anotação com pipeline compartilhado [D]:** a anotação das espécies (Tabela 2) é um processo separado da medição, mas ambos passam pela **mesma função** `detectar_folhas()` (binarização → limpeza → bordas → varredura → ids), definida na Etapa 6. Assim, a "Folha k" que você anota é, por construção, a "Folha k" medida. A bifurcação ocorre depois dela:

| Modo | O que faz com a lista de folhas |
|---|---|
| Medição (interativo/lote) | Percorre, conta, exibe e grava `contagens.csv` |
| Anotação [S] (`--anotar`) | Exibe cada folha com seu id (e `b0`), pede a espécie no terminal e grava a linha em `tabela_especies.csv` |

Chave da anotação: `(imagem, folha_id) → espécie`. Em memória: `std::map<std::pair<std::string,int>, std::string>`.

### 10.2 Estatística por espécie (descritor único)

1. Desdobrar as duas tabelas em uma lista de pares `(espécie, pixels_fronteira)`, percorrendo as células que existem nas duas.
2. Para cada espécie: quantidade de folhas, média, mediana, mínimo, máximo, amplitude e desvio padrão (escolha amostral ou populacional e documente).
3. Gráfico por espécie (pontos ou boxplot) para ver dispersão e sobreposição.
4. **Matriz de sobreposição:** para cada par de espécies, verificar se as faixas `[mínimo, máximo]` se sobrepõem.
5. Informar quantas folhas há por espécie: as classes podem ficar desbalanceadas.

### 10.3 Capacidade de identificar a espécie

O descritor é um **único número por folha**. Duas leituras complementares:

- **Leitura visual/estatística:** se as faixas de espécies diferentes ficam separadas, o descritor discrimina; se há muita sobreposição, não discrimina.
- **[S] Regra de decisão simples em uma dimensão**, apresentada como **análise experimental** (não faz parte da extração morfológica obrigatória): atribuir a folha à espécie de média mais próxima (ou por faixas). Avaliar com **leave-one-out** (retira uma folha, classifica com as demais) e apresentar taxa de acerto global, acerto por espécie e matriz de confusão 6×6. Compare com a taxa de "sempre responder a espécie mais frequente".

**Fatores que influenciam o número e devem ser discutidos:**

- Elemento estruturante e política de borda (Etapas 4 e 5).
- Resolução e escala das fotos (distância da câmera).
- Inclusão ou não do pecíolo.
- Serrilhado (Pessegueiro, Coleus), que tende a aumentar a contagem em relação a bordas lisas. É uma hipótese a verificar nos dados, não um resultado.
- Poucas amostras por espécie, o que aumenta a incerteza.

**Regra de ouro:** escreva a conclusão **depois** de ver os números e cite-os. A resposta à pergunta do enunciado ("é possível identificar a espécie usando apenas o número de pixels?") deve decorrer das tabelas e dos gráficos obtidos.

**Ferramentas:** estatísticas e geração das tabelas podem ser feitas em C++. Se usar planilha ou script externo para gráficos, declare no relatório que serviram apenas para visualizar a análise.

### 10.4 Relatório e entrega

1. Estrutura sugerida: Introdução, Objetivos, Fundamentação teórica, Morfologia Matemática, Metodologia, Desenvolvimento do algoritmo (fluxos 1.1 e 1.2), Algoritmo seguidor de fronteiras, Implementação em C++, Resultados (Tabela 1), Análise por espécie (Tabela 2 e estatísticas), Discussão, Conclusão, Declaração de uso de IA e Referências.
2. Figuras: imagem original, binária, imagem de bordas, perímetro em vermelho de algumas folhas, gráfico por espécie e matriz de confusão (se usada).
3. **Declaração de Uso de IA:** cite quais IAs foram usadas e para quê, descrevendo apenas o que realmente ocorreu.
4. Conferências finais: C++; OpenCV apenas na leitura, exibição e janela; fronteira obtida por erosão e subtração; seguidor de Gonzalez & Woods; pixels em vermelho; quantidade de folhas e contagem informadas; tabela imagem × folha; conclusão baseada em dados.
5. Enviar para `adair.santacatarina@unioeste.br` até **29/10/2026**. Preparar-se para a apresentação agendada: todos os integrantes devem saber explicar cada módulo e cada decisão.

---

## Apêndice A. Ordem de trabalho recomendada

1. Etapas 0 a 3: abrir a imagem e ver a binária das 36 imagens.
2. Etapa 4 em matrizes pequenas, **antes** de usar imagens reais.
3. Etapa 5 em formas sintéticas e depois numa imagem real.
4. Etapa 6: primeiro o seguidor em formas sintéticas, depois a varredura com várias folhas.
5. Etapas 7 e 8: interação e laço de imagens.
6. Etapa 9: registrar e conferir `contagens.csv`.
7. Processar todas as imagens e só então a Etapa 10.

## Apêndice B. Quadro de depuração

| Sintoma | Causa provável |
|---|---|
| Cores estranhas | Ordem B, G, R lida como R, G, B |
| Tudo branco ou tudo preto na binária | Tolerância fora da faixa ou polaridade invertida |
| Coleus incompleto na binária | Bordas claras confundidas com o fundo; usar distância de cor |
| Mais de 8 folhas na imagem de exemplo | Buracos internos (fronteiras internas) ou ruído sem `L_min` |
| Menos de 8 folhas | Folhas fundidas, folha perdida na binarização ou `L_min` alto demais |
| Bordas com 2 ou mais pixels de espessura | Elemento quadrado 3×3 em vez da cruz, ou erosão aplicada mais de uma vez |
| Bordas vazias ou muito esparsas | Subtração feita na ordem errada ou erosão sobre a imagem errada |
| Cópia "muda junto" com a original | Cópia rasa (compartilha dados) em vez de cópia profunda |
| Programa trava no seguidor | Condição de parada errada ou ponto inicial inválido |
| `únicos` diferente do tamanho do componente | Ramificações, parada precoce ou pixels desconexos na máscara |
| Perímetro vermelho deslocado | Mistura de coordenadas ou de ordem x/y |
| Enter não responde | Código 10 em vez de 13, ou janela sem foco |
| Programa fecha sozinho com a pilha vazia | Laço principal terminando quando a pilha de imagens esvazia |
| Janela fechada e programa continua rodando | Falta consultar a visibilidade da janela dentro do laço de espera |
| Imagens processadas fora da ordem esperada | Ordenação crescente empilhada sem inverter |
| Células "0" na tabela em vez de vazias | Preenchimento com zero nas folhas inexistentes |
| Espécie atribuída à folha errada | `folha_id` anotado numa execução e usado em outra com parâmetros diferentes |
| Taxa de acerto suspeitamente alta | Avaliação nos mesmos dados do classificador ou classes desbalanceadas |

## Apêndice C. Referências técnicas

- GONZALEZ, R. C.; WOODS, R. E. *Digital Image Processing*. 4. ed. New York: Pearson Education Limited, 2018, p. 814–816 (seguidor de fronteiras). Para morfologia binária, consultar o capítulo de Morfologia Matemática do mesmo livro.
- Documentação da OpenCV (módulos `imgcodecs` e `highgui`): https://docs.opencv.org
- Documentação do CMake: https://cmake.org/documentation
- Referência da biblioteca padrão C++: https://en.cppreference.com
