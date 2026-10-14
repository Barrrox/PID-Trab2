#ifndef FUNCOES_H
#define FUNCOES_H

#include <vector>
#include <cstddef>

// Representa um pixel no formato BGR
struct BGR {
    int b = 0;
    int g = 0;
    int r = 0;
};

// Imagem colorida no formato BGR
struct ImagemBGR {
    int largura = 0;
    int altura = 0;
    // Vetor linear com dados no formato BGR: (y * largura + x) * 3 + canal
    // canal 0: B, canal 1: G, canal 2: R
    std::vector<unsigned char> dados;

    BGR obterPixel(int x, int y) const {
        size_t idx = (static_cast<size_t>(y) * largura + x) * 3;
        return { dados[idx], dados[idx + 1], dados[idx + 2] };
    }

    void definirPixel(int x, int y, const BGR& cor) {
        size_t idx = (static_cast<size_t>(y) * largura + x) * 3;
        dados[idx] = static_cast<unsigned char>(cor.b);
        dados[idx + 1] = static_cast<unsigned char>(cor.g);
        dados[idx + 2] = static_cast<unsigned char>(cor.r);
    }
};

// Imagem binária: folha = 1, fundo = 0
struct ImagemBinaria {
    int largura = 0;
    int altura = 0;
    // Vetor linear de 1 canal: (y * largura + x)
    // 0 = fundo, 1 = folha (ou 255 para exibição)
    std::vector<unsigned char> dados;

    unsigned char obterPixel(int x, int y) const {
        return dados[static_cast<size_t>(y) * largura + x];
    }

    void definirPixel(int x, int y, unsigned char valor) {
        dados[static_cast<size_t>(y) * largura + x] = valor;
    }
};

// Compatibilidade com possíveis códigos que ainda usem o nome RGB
using RGB = BGR;
using ImagemRGB = ImagemBGR;

// Estima a cor média do fundo (BGR) analisando as bordas da imagem
BGR estimarCorFundo(const ImagemBGR& imagem);

// Binariza a imagem calculando a distância de cor BGR em relação ao fundo
// Retorna ImagemBinaria onde 1 = folha e 0 = fundo
ImagemBinaria binarizar(const ImagemBGR& imagem, int tolerancia);

#endif // FUNCOES_H
