#ifndef FUNCOES_H
#define FUNCOES_H

#include <vector>
#include <cstddef>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

struct ponto {
    int y;
    int x;
};

struct HistogramaBGR {
    std::vector<std::vector<int>> F; // Frequencias [3][256] -> 0:B, 1:G, 2:R
    int max_global = 0;                     // Maior pico entre os 3 canais
};


// na ordem em que são executadas

int estimarCorFundo(const cv::Mat& imagem);

std::vector<std::vector<std::vector<int>>> CV2vectorvector(const cv::Mat& imagem);

std::vector<std::vector<int>> grayscale(const std::vector<std::vector<std::vector<int>>>& imagem);

void salvar_histograma(const HistogramaBGR& histograma, const std::string& caminho);

HistogramaBGR gerar_histograma(const std::vector<std::vector<std::vector<int>>>& imagem);


/* Suaviza a imagem. Métodos:
0 = Filtro Gaussiano
1 = Filtro Box Média
2 = Filtro por Mediana
tam_mascara = 3, 5, 7 ou 9
*/
std::vector<std::vector<int>> suavizar(const std::vector<std::vector<int>>& imagem, int metodo = 0, int tam_mascara = 7);
 
// Binariza com um limiar e margem de fundo parametrizável
std::vector<std::vector<int>> binarizar(const std::vector<std::vector<int>>& imagem_cinza, int fundoCinza, int margem_fundo = 45);

std::vector<std::vector<int>> erosao(const std::vector<std::vector<int>>& binaria, int tam_ee = 5);

std::vector<std::vector<int>> dilatacao(const std::vector<std::vector<int>>& binaria, int tam_ee = 5);

std::vector<std::vector<int>> subtrair(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B);

std::vector<std::vector<std::vector<int>>> adicionar_bordas(std::vector<std::vector<std::vector<int>>>& imagem, std::vector<std::vector<int>>& bordas);

std::vector<ponto> contornarBorda(const std::vector<std::vector<int>> &imagem, ponto ponto_inicial);
// auxiliares (nao sao chamadas no main)

int identificar_folhas(std::vector<std::vector<int>>& imagem_binarizada, int limite_max_folhas = -1);

std::vector<std::vector<int>> adicionar_padding(const std::vector<std::vector<int>>& imagem, int tamanho = 1, int valor = 0);
                                                
std::vector<std::vector<int>> remover_padding(const std::vector<std::vector<int>>& imagem, int tamanho);
#endif // FUNCOES_H