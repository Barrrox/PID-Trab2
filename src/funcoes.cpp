#include "funcoes.h"
#include <cmath>
#include <cstdlib>

// Abrir imagem


// Binarizar imagem

BGR estimarCorFundo(const ImagemBGR& imagem) {
    if (imagem.largura <= 0 || imagem.altura <= 0 || imagem.dados.empty()) {
        return { 255, 255, 255 };
    }

    long long somaB = 0;
    long long somaG = 0;
    long long somaR = 0;
    int totalAmostras = 0;

    // Amostra a borda superior (y = 0) e borda inferior (y = altura - 1)
    for (int x = 0; x < imagem.largura; x++) {
        BGR pTop = imagem.obterPixel(x, 0);
        somaB += pTop.b;
        somaG += pTop.g;
        somaR += pTop.r;

        BGR pBottom = imagem.obterPixel(x, imagem.altura - 1);
        somaB += pBottom.b;
        somaG += pBottom.g;
        somaR += pBottom.r;
        totalAmostras += 2;
    }

    // Amostra as bordas laterais esquerda (x = 0) e direita (x = largura - 1),
    // excluindo os cantos que já foram contabilizados (y de 1 a altura - 2)
    for (int y = 1; y < imagem.altura - 1; y++) {
        BGR pLeft = imagem.obterPixel(0, y);
        somaB += pLeft.b;
        somaG += pLeft.g;
        somaR += pLeft.r;

        BGR pRight = imagem.obterPixel(imagem.largura - 1, y);
        somaB += pRight.b;
        somaG += pRight.g;
        somaR += pRight.r;
        totalAmostras += 2;
    }

    BGR fundo;
    if (totalAmostras > 0) {
        fundo.b = static_cast<int>(somaB / totalAmostras);
        fundo.g = static_cast<int>(somaG / totalAmostras);
        fundo.r = static_cast<int>(somaR / totalAmostras);
    }
    return fundo;
}


ImagemBinaria binarizar(const ImagemBGR& imagem, int tolerancia) {
    ImagemBinaria binaria;
    binaria.largura = imagem.largura;
    binaria.altura = imagem.altura;
    binaria.dados.resize(static_cast<size_t>(imagem.largura) * imagem.altura, 0);

    if (imagem.largura <= 0 || imagem.altura <= 0 || imagem.dados.empty()) {
        return binaria;
    }

    // Cor média de referência das bordas
    BGR fundo = estimarCorFundo(imagem);

    for (int y = 0; y < imagem.altura; y++) {
        for (int x = 0; x < imagem.largura; x++) {
            BGR p = imagem.obterPixel(x, y);

            // Distância de cor Manhattan entre o pixel e o fundo no espaço BGR
            int dist = std::abs(p.b - fundo.b) +
                       std::abs(p.g - fundo.g) +
                       std::abs(p.r - fundo.r);

            // Se a diferença de cor for maior que a tolerância, classifica como folha (1), senão fundo (0)
            if (dist > tolerancia) {
                binaria.definirPixel(x, y, 1);
            } else {
                binaria.definirPixel(x, y, 0);
            }
        }
    }

    return binaria;
}

// Aplicar morfologia matemática