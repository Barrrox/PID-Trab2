# Pipeline Macro do Projeto (PID 2026 - Morfologia Matemática)

Visão em alto nível do fluxo de processamento do projeto, destacando a etapa compartilhada de identificação e a bifurcação entre anotação e medição.

---

## 1. Diagrama Macro (Mermaid)

```mermaid
flowchart TD
    A["1. Selecionar e Abrir Imagem"] --> B["2. Binarizar Imagem\n(Folha = 1, Fundo = 0)"]
    B --> C["3. Aplicar Morfologia Matemática\n(Erosão e Subtração)"]
    C --> D["4. Identificar e Numerar Folhas\n(detectar_folhas - Função Compartilhada)"]
    
    D --> E{"Modo de Execução"}
    
    %% Ramo A: Anotação
    E -- "Modo Anotação" --> F["5A. Anotação do Gabarito\n- Destaca folha pelo ID\n- Usuário informa a espécie real"]
    F --> G[("tabela_especies.csv\n(imagem, folha_id ➔ espécie)")]
    
    %% Ramo B: Medição e Visualização
    E -- "Modo Medição / Interativo" --> H["5B. Medição e Visualização\n- Seguidor de fronteiras G&W\n- Contagem de pixels únicos\n- Borda em vermelho com <Enter>"]
    H --> I[("contagens.csv\n(imagem, folha_id ➔ pixels)")]
    
    %% Fase Final: Consolidação
    G --> J["6. Análise Estatística e Relatório\n- Cruzamento de dados folha a folha\n- Análise da capacidade de discriminação\n- Conclusões baseadas em dados"]
    I --> J
```

---

## 2. As Etapas em Alto Nível

1. **Selecionar e Abrir Imagem:** Carrega as fotos da pasta de entrada usando apenas as funções de leitura permitidas da OpenCV.
2. **Binarizar Imagem:** Converte a imagem colorida para binária, separando as folhas (1) do fundo (0).
3. **Aplicar Morfologia Matemática:** Executa operações manuais de limpeza (abertura e preenchimento de buracos) e extrai a fronteira morfológica ($A - (A \ominus B)$).
4. **Identificar e Numerar Folhas (`detectar_folhas`):** Varre a imagem e atribui identificadores determinísticos às folhas (Folha 1, Folha 2...). **Esta função é idêntica para os dois fluxos.**
5. **Bifurcação:**
   * **Fluxo 5A (Anotação do Gabarito):** Exibe a folha numerada para que o usuário informe a espécie real (com base na Figura 1 do enunciado), gerando a `tabela_especies.csv`.
   * **Fluxo 5B (Medição e Visualização):** Percorre a borda com o seguidor de fronteiras de Gonzalez & Woods, calcula o número de pixels únicos, exibe o contorno em vermelho folha a folha com `<Enter>` e salva as medições em `contagens.csv`.
6. **Análise Estatística e Relatório:** Cruza os pixels medidos com a espécie real de cada folha, analisa a separabilidade das classes e fundamenta a resposta do relatório.
