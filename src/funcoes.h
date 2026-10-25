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


// na ordem em que são executadas

int estimarCorFundo(const cv::Mat& imagem);

std::vector<std::vector<std::vector<int>>> CV2vectorvector(const cv::Mat& imagem);

std::vector<std::vector<int>> grayscale(const std::vector<std::vector<std::vector<int>>>& imagem);

std::vector<std::vector<int>> suavizar(const std::vector<std::vector<int>>& imagem);

// Binariza com um limiar
std::vector<std::vector<int>> binarizar(const std::vector<std::vector<int>>& imagem_cinza, int fundoCinza);

std::vector<std::vector<int>> erosao(const std::vector<std::vector<int>>& binaria);

std::vector<std::vector<int>> subtrair(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B);

std::vector<std::vector<std::vector<int>>> adicionar_bordas(std::vector<std::vector<std::vector<int>>>& imagem, std::vector<std::vector<int>>& bordas);

int contadorBorda(const std::vector<std::vector<int>>& imagem);
// auxiliares (nao sao chamadas no main)

std::vector<std::vector<int>> adicionar_padding(const std::vector<std::vector<int>>& imagem, int tamanho = 1, int valor = 0);
                                                
std::vector<std::vector<int>> remover_padding(const std::vector<std::vector<int>>& imagem, int tamanho);
#endif // FUNCOES_H