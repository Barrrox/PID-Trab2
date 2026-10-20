#include "funcoes.h"
#include <cmath>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;
#include <opencv2/core/utils/logger.hpp> // Para silenciar INFOs do openCV

// Abrir imagem


// Binarizar imagem

int estimarCorFundo(const Mat& imagem) {

    long long somaB = 0;
    long long somaG = 0;
    long long somaR = 0;
    

    int y_max = imagem.rows - 1;
    int x_max = imagem.cols - 1;

    int totalAmostras = (y_max * 2 + x_max * 2) - 4; // Total de pixels nas borda // 4 é das bordas sobrepostas nos cantos
    
    // Amostra a borda superior (y = 0) e borda inferior (y = altura - 1)
    for (int x = 0; x < imagem.cols; x++) {

        // Borda inferior
        cv::Vec3b pixel1 = imagem.at<cv::Vec3b>(y_max, x);

        somaB += pixel1[0];
        somaG += pixel1[1];
        somaR += pixel1[2];

        // Borda superior
        cv::Vec3b pixel2 = imagem.at<cv::Vec3b>(0, x);

        somaB += pixel2[0];
        somaG += pixel2[1];
        somaR += pixel2[2];

    }

    
    // 1 2 3 4
    // 1 2 3 4
    // 1 2 3 4
    // 1 2 3 4
    // Amostra as bordas laterais esquerda (x = 0) e direita (x = largura - 1),
  
    for (int y = 1; y < imagem.rows - 1; y++) {

        // Borda direita
        cv::Vec3b pixel1 = imagem.at<cv::Vec3b>(y, x_max);

        somaB += pixel1[0];
        somaG += pixel1[1];
        somaR += pixel1[2];

        // Borda esquerda
        cv::Vec3b pixel2 = imagem.at<cv::Vec3b>(y, 0);

        somaB += pixel2[0];
        somaG += pixel2[1];
        somaR += pixel2[2];

    }

    cv::Vec3b fundo;
    if (totalAmostras > 0) {
        fundo[0] = static_cast<int>(somaB / totalAmostras); // b
        fundo[1] = static_cast<int>(somaG / totalAmostras); // g
        fundo[2] = static_cast<int>(somaR / totalAmostras); // r
    }

    int fundoCinzaPonderado = static_cast<int>(0.299 * fundo[2] + 0.587 * fundo[1] + 0.114 * fundo[0]);
    //int fundoCinzaMedio = static_cast<int>((fundo[2] + fundo[1] + fundo[0])/3);
    
    return fundoCinzaPonderado;
}


vector<vector<int>> binarizar(const Mat& imagem) {
    
    vector<vector<int>> binaria(imagem.rows, vector<int>(imagem.cols));
        // for (int j = 0; j < imagem.cols; j++) {
        //     // exemplo, assumindo imagem em tons de cinza (CV_8UC1)
        //     binaria[i][j] = (imagem.at<uchar>(i, j) > tolerancia) ? 1 : 0;
        // }


    if (binaria.empty()) {
        return binaria;
    }

    // Cor média de referência das bordas já em gray scale
    int fundoCinza = estimarCorFundo(imagem) - 50; // revisar e incluior no relatório
    cout << fundoCinza << endl; // 153


//          /\
//         /__\
//         |  |
//         |  |
//         |__|
//        00  00
//       00    00  

    for (int y = 0; y < imagem.rows; y++) {
        for (int x = 0; x < imagem.cols; x++) {
            cv::Vec3b pixel = imagem.at<cv::Vec3b>(y, x);

            // Converte o pixel para escala de cinza
            int pixel_em_cinza = static_cast<int>(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);

            // Se a diferença de cor for maior que a tolerância, classifica como folha (1), senão fundo (0)
            if (pixel_em_cinza > fundoCinza) { 
                binaria[y][x] = 1;
            } else {
                binaria[y][x] = 0;
            }
        }
    }

    return binaria;
}

// Aplicar morfologia matemática