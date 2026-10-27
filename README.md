# PID 2026 – Parte 01: Analisador de Fronteira de Folhas

**Autores:** André Franco e Matheus Barros  
**Disciplina:** Processamento de Imagens Digitais (UNIOESTE Cascavel)  
**Professor:** Adair Santa Catarina

---

## Organização dos Arquivos

- `src/funcoes.h` / `src/funcoes.cpp`: Implementações manuais de PID (conversão para cinza, suavização Gaussiana/Média/Mediana, binarização, erosão, dilatação, extração de bordas, seguidor de fronteiras e identificação de folhas).
- `src/main.cpp`: Modo interativo (processa uma imagem escolhida pelo usuário, salva as etapas em `output/` e exibe as janelas).
- `src/main2.cpp`: Processamento em lote testando os 3 métodos de suavização sobre o banco de imagens (`output2/`).
- `src/main3.cpp`: Processamento em lote salvando as etapas `5.1 - Dilatação (_D)` e `5.2 - Fechamento (_F)` (`output3/`).
- `src/gridsearch.cpp`: Busca em grade (Grid Search) de hiperparâmetros com poda automática (`[6, 10]` folhas por imagem), gerando `output_gridsearch/resultados_validos.csv`.
- `imagens/`: Banco de imagens `.bmp` de entrada.

---

## Escolhendo qual código compilar

No arquivo `CMakeLists.txt`, ajuste a linha `add_executable` para o arquivo principal desejado:

```cmake
# Modo interativo padrão:
add_executable(ContadorFolhas src/main.cpp src/funcoes.cpp)

# Ou para rodar o Grid Search:
# add_executable(ContadorFolhas src/gridsearch.cpp src/funcoes.cpp)

# Ou para rodar main2 / main3:
# add_executable(ContadorFolhas src/main2.cpp src/funcoes.cpp)
# add_executable(ContadorFolhas src/main3.cpp src/funcoes.cpp)
```

---

## Compilar e Executar (Windows - PowerShell / CMD)

### 1. Configurar o CMake (apenas na 1ª vez ou ao criar a pasta `build`)
```powershell
cmake -B build
```

### 2. Compilar o projeto
```powershell
cmake --build ./build
```
*(Para compilar com otimização máxima no Grid Search, use `cmake --build ./build --config Release`)*

### 3. Executar (a partir da raiz do projeto)
```powershell
.\build\Debug\ContadorFolhas.exe
```
*(Ou `.\build\Release\ContadorFolhas.exe` caso tenha compilado em modo `Release`)*

---

## Compilar e Executar (Linux)

### 1. Configurar e compilar
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build
```

### 2. Executar (a partir da raiz do projeto)
```bash
./build/ContadorFolhas
```

---

## Notas do Grid Search
- `num_imagens` = 34 / 35
- `margens` = 11 (`[40, 50]`)
- `num de filtros` = 3 (`Gauss`, `Media`, `Mediana`)
- `numero de tamanhos de filtros` = 4 (`3x3`, `5x5`, `7x7`, `9x9`)
- `numero de tamanhos de EE` = 4 (`3x3`, `5x5`, `7x7`, `9x9`)
