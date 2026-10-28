#include "funcoes.h"
#include <cmath>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <map>
#include <queue>
#include <algorithm>
using namespace cv;
using namespace std;
#include <opencv2/core/utils/logger.hpp> // Para silenciar INFOs do openCV

//const int MARGEM_FUNDO = 45;

// vector<vector<int>> elemento_estruturante = {
//     {1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1}
// };

// vector<vector<int>> elemento_estruturante = {
//     {1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1},
//     {1, 1, 1, 1, 1, 1, 1}
// };

vector<vector<int>> elemento_estruturante = {
    {1, 1, 1, 1, 1,},
    {1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1}
};

// vector<vector<int>> elemento_estruturante = {
//     {1, 1, 1,},
//     {1, 1, 1,},
//     {1, 1, 1}




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

vector<int> estimarCorFundoBGR(const Mat& imagem) {

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

    vector<int> fundo(3);
    if (totalAmostras > 0) {
        fundo[0] = static_cast<int>(somaB / totalAmostras); // b
        fundo[1] = static_cast<int>(somaG / totalAmostras); // g
        fundo[2] = static_cast<int>(somaR / totalAmostras); // r
    }

    return fundo;
}


vector<vector<vector<int>>> CV2vectorvector(const Mat& imagem){
   
    vector<vector<vector<int>>> convertida(
    imagem.rows,
    vector<vector<int>>(imagem.cols, vector<int>(3)));

    for (int y = 0; y < imagem.rows; y++) {
        for (int x = 0; x < imagem.cols; x++) {
            Vec3b pixel = imagem.at<Vec3b>(y, x);
            convertida[y][x][0] = pixel[0]; // B uchar converte para int implicitamente sem aviso.
            convertida[y][x][1] = pixel[1]; // G
            convertida[y][x][2] = pixel[2]; // R
        }
    }

    return convertida;
}


void salvar_histograma(const HistogramaBGR& histograma, const std::string& caminho){

    int altura = 300;
    int largura = 256;
    cv::Mat imagem_hist(altura, largura, CV_8UC3, cv::Scalar(0, 0, 0));

    for (int x = 0; x < largura; x++) {
            for (int canal = 0; canal < 3; canal++) {
                // Normaliza altura real para imagem não ficar extratosferica.
                int h_barra = (histograma.F[canal][x] * (altura - 1)) / histograma.max_global;
                int y_topo = (altura - 1) - h_barra;
                imagem_hist.at<Vec3b>(y_topo, x)[canal] = 255;
                
            }
        }
        cv::imwrite(caminho, imagem_hist);
        
}

HistogramaBGR gerar_histograma(const vector<vector<vector<int>>>& imagem){

    int altura = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());
    
    // Aloca
    HistogramaBGR histograma;
    histograma.F.assign(3, vector<int>(256, 0));

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            histograma.F[0][imagem[y][x][0]]++;
            histograma.F[1][imagem[y][x][1]]++;
            histograma.F[2][imagem[y][x][2]]++;
        }
    }

    int maior_pico = 0; // valor do maior pico entre os 3 canais;
    int picos[3] = {0,0,0}; // BGR

    for(int canal = 0; canal < 3; canal++){
        for(int freq = 0; freq < 256; freq++){
            if (histograma.F[canal][freq] > picos[canal]){
                picos[canal] = histograma.F[canal][freq];
            }
        }
    }

    histograma.max_global = max({picos[0], picos[1], picos[2]});

    return histograma;

}

vector<vector<int>> adicionar_padding(const vector<vector<int>>& imagem, int tamanho, int valor) {

    int altura = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());

    vector<vector<int>> matriz_com_padding(altura + 2 * tamanho,
                                           vector<int>(largura + 2 * tamanho, valor));

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            matriz_com_padding[y + tamanho][x + tamanho] = imagem[y][x];
        }
    }
    return matriz_com_padding;
}

vector<vector<vector<int>>> adicionar_padding(const vector<vector<vector<int>>>& imagem, int tamanho, int valor) {

    int altura = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());

    vector<vector<vector<int>>> matriz_com_padding(
        altura + 2 * tamanho,
        vector<vector<int>>(largura + 2 * tamanho, vector<int>(3, valor))
    );

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            matriz_com_padding[y + tamanho][x + tamanho] = imagem[y][x];
        }
    }
    return matriz_com_padding;
}

vector<vector<int>> remover_padding(const vector<vector<int>>& imagem, int tamanho) {

    int altura = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());

    int remocao = 2 * tamanho;

    vector<vector<int>> matriz_sem_padding(altura - remocao,
                                           vector<int>(largura - remocao, 0));
    
    for (int y = 0; y < altura-remocao; y++) {
        for (int x = 0; x < largura-remocao; x++) {
            matriz_sem_padding[y][x] = imagem[y + tamanho][x + tamanho];
        }
    }
    
    return matriz_sem_padding;
}

vector<vector<int>> grayscale(const vector<vector<vector<int>>>& imagem) {
    
    int altura = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());
            
    // Aloca memoria
    vector<vector<int>> imagemCinza(altura, vector<int>(largura, 0));
   
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            //vector<int> pixel = imagem[y][x];
            const vector<int>& pixel = imagem[y][x];
                
            // Converte o pixel para escala de cinza
            imagemCinza[y][x] = static_cast<int>(round(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]));
        }
    }
    return imagemCinza;
}

int aplica_filtro(int x, int y, const int raio, const vector<vector<int>>& imagem_com_padding, int metodo, const vector<vector<int>>& mascara, int somaPesos){

    // Se filtro gaussiano (0) ou box (1)
    if (metodo <= 1){
        
        int soma = 0;
            
            for (int j = -raio; j <= raio; j++) { // Itera sobre as linhas da máscara
                for (int i = -raio; i <= raio; i++) { // Itera sobre os pixels da linha

                // a possicao acessada é: posicao na imagem original + deslocamento do padding + posição atual da mascara
                // isso pra linha e coluna
                    soma += imagem_com_padding[y + raio + j][x + raio + i] * mascara[j + raio][i + raio];
    
                }
            }
            // Opera o pixel
            return static_cast<int>(round(soma / somaPesos)); 


    } else if (metodo == 2) // Filtro por mediana
    {
        // Flatenizar a mascara para aplicar sorting
        const int tam_mascara = 2*raio+1; // dimensao da mascara
        vector<int> vetor(tam_mascara*tam_mascara);
        for (int j = 0; j < tam_mascara; j++) { // Itera sobre as linhas da máscara
                for (int i = 0; i < tam_mascara; i++) { // Itera sobre os pixels da linha
                vetor[j*tam_mascara + i] = imagem_com_padding[y + j][x + i];
                
                }
            }

        sort(vetor.begin(), vetor.end());

        const size_t metade = vetor.size()/2;

        return vetor[metade];

    } else {
        cout << "Valor" << metodo << "para parâmetro Método de aplica_filtro() não existe" << endl;
        return -1;
    }
    

}

vector<vector<int>> suavizar(const vector<vector<int>>& imagem, int metodo, int tam_mascara){
    
    int altura  = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());

    vector<vector<int>> mascara(tam_mascara, vector<int>(tam_mascara, 1));
    int somaPesos = 0;

    if (metodo == 0) { // Filtro Gaussiano (3x3, 5x5, 7x7, 9x9 via Triângulo de Pascal)
        vector<int> pesos1D;
        if (tam_mascara == 3) {
            pesos1D = {1, 2, 1};
        } else if (tam_mascara == 5) {
            pesos1D = {1, 4, 6, 4, 1};
        } else if (tam_mascara == 7) {
            pesos1D = {1, 6, 15, 20, 15, 6, 1};
        } else if (tam_mascara == 9) {
            pesos1D = {1, 8, 28, 56, 70, 56, 28, 8, 1};
        } else {
            pesos1D.assign(tam_mascara, 1);
        }

        somaPesos = 0;
        for (int j = 0; j < tam_mascara; j++) {
            for (int i = 0; i < tam_mascara; i++) {
                mascara[j][i] = pesos1D[j] * pesos1D[i];
                somaPesos += mascara[j][i];
            }
        }
    } else if (metodo == 1) { // Filtro Box Média (tam_mascara x tam_mascara)
        somaPesos = tam_mascara * tam_mascara;
    }

    int raio = tam_mascara / 2; // Raio inteiro da máscara (ex: 5x5 -> raio = 2)

    vector<vector<int>> imagem_com_padding = adicionar_padding(imagem, raio, 255);

    vector<vector<int>> suavizada(altura, vector<int>(largura, 0));

    for (int y = 0; y < altura; y++) { // opera sobre as linhas
        for (int x = 0; x < largura; x++) { // opera sobre os elementos da coluna na linha y
            suavizada[y][x] = aplica_filtro(
                x, y,
                raio,
                imagem_com_padding,
                metodo,
                mascara,somaPesos);
        }
    }
    return suavizada;
}



vector<int> aplica_filtroBGR(int x, int y, const int raio, const vector<vector<vector<int>>>& imagem_com_paddingBGR, int metodo, const vector<vector<int>>& mascara, int somaPesos){

    // Se filtro gaussiano (0) ou box (1)
    if (metodo <= 1){
        
        int somaB = 0;
        int somaG = 0;
        int somaR = 0;
            
            for (int j = -raio; j <= raio; j++) { // Itera sobre as linhas da máscara
                for (int i = -raio; i <= raio; i++) { // Itera sobre os pixels da linha

                // a possicao acessada é: posicao na imagem original + deslocamento do padding + posição atual da mascara
                // isso pra linha e coluna
                    somaB += imagem_com_paddingBGR[y + raio + j][x + raio + i][0] * mascara[j + raio][i + raio];
                    somaG += imagem_com_paddingBGR[y + raio + j][x + raio + i][1] * mascara[j + raio][i + raio];
                    somaR += imagem_com_paddingBGR[y + raio + j][x + raio + i][2] * mascara[j + raio][i + raio];
                }
            }
            // Retorna o pixel em vetor
            return {static_cast<int>(round(somaB / static_cast<double>(somaPesos))),
                    static_cast<int>(round(somaG / static_cast<double>(somaPesos))),
                    static_cast<int>(round(somaR / static_cast<double>(somaPesos)))}; 


    } else if (metodo == 2) // Filtro por mediana
    {
        // Flatenizar a mascara para aplicar sorting
        const int tam_mascara = 2*raio+1; // dimensao da mascara
        
        vector<int> vetorB(tam_mascara*tam_mascara);
        vector<int> vetorG(tam_mascara*tam_mascara);
        vector<int> vetorR(tam_mascara*tam_mascara);
        
        for (int j = 0; j < tam_mascara; j++) { // Itera sobre as linhas da máscara
                for (int i = 0; i < tam_mascara; i++) { // Itera sobre os pixels da linha
                vetorB[j*tam_mascara + i] = imagem_com_paddingBGR[y + j][x + i][0];
                vetorG[j*tam_mascara + i] = imagem_com_paddingBGR[y + j][x + i][1];
                vetorR[j*tam_mascara + i] = imagem_com_paddingBGR[y + j][x + i][2];
                
                }
            }

        sort(vetorB.begin(), vetorB.end());
        sort(vetorG.begin(), vetorG.end());
        sort(vetorR.begin(), vetorR.end());

        const size_t metade = vetorB.size()/2; // index da posição mediana

        return {vetorB[metade], vetorG[metade], vetorR[metade]};

    } else {
        cout << "Valor" << metodo << "para parâmetro Método de aplica_filtro() não existe" << endl;
        return {};
    }
    

}


vector<vector<vector<int>>> suavizarBGR(const vector<vector<vector<int>>>& imagem, int metodo, int tam_mascara){
    
    int altura  = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());

    vector<vector<int>> mascara(tam_mascara, vector<int>(tam_mascara, 1));
    int somaPesos = 0;

    if (metodo == 0) { // Filtro Gaussiano (via Triângulo de Pascal)
        vector<int> pesos1D;
        if (tam_mascara == 3) {
            pesos1D = {1, 2, 1};
        } else if (tam_mascara == 5) {
            pesos1D = {1, 4, 6, 4, 1};
        } else if (tam_mascara == 7) {
            pesos1D = {1, 6, 15, 20, 15, 6, 1};
        } else if (tam_mascara == 9) {
            pesos1D = {1, 8, 28, 56, 70, 56, 28, 8, 1};
        } else {
            pesos1D.assign(tam_mascara, 1);
        }

        somaPesos = 0;
        for (int j = 0; j < tam_mascara; j++) {
            for (int i = 0; i < tam_mascara; i++) {
                mascara[j][i] = pesos1D[j] * pesos1D[i];
                somaPesos += mascara[j][i];
            }
        }
    } else if (metodo == 1) { // Filtro Box Média (tam_mascara x tam_mascara)
        somaPesos = tam_mascara * tam_mascara;
    }

    int raio = tam_mascara / 2; // Raio inteiro da máscara (ex: 5x5 -> raio = 2)

    vector<vector<vector<int>>> imagem_com_padding = adicionar_padding(imagem, raio, 255);

    vector<vector<vector<int>>> suavizada(altura, vector<vector<int>>(largura, vector<int>(3, 0)));

    for (int y = 0; y < altura; y++) { // opera sobre as linhas
        for (int x = 0; x < largura; x++) { // opera sobre os elementos da coluna na linha y
            suavizada[y][x] = aplica_filtroBGR(
                x, y,
                raio,
                imagem_com_padding,
                metodo,
                mascara,somaPesos);
        }
    }
    return suavizada;
}

vector<vector<int>> binarizar(const vector<vector<int>>& imagem_cinza, int fundoCinza, int margem_fundo) {

    if (imagem_cinza.empty()) {
        return imagem_cinza; 
    }
    
    int altura  = static_cast<int>(imagem_cinza.size());
    int largura = static_cast<int>(imagem_cinza[0].size());

    vector<vector<int>> binaria(altura, vector<int>(largura, 0));
    
    // Cor média de referência das bordas já em gray scale
    fundoCinza -= margem_fundo; // revisar e incluir no relatório

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            // Se a diferença de cor for maior que a tolerância, classifica como folha (1), senão fundo (0)
            if (imagem_cinza[y][x] < fundoCinza) binaria[y][x] = 1;
        }
    }

    return binaria;
}

std::vector<std::vector<int>> binarizarBGR(
        const std::vector<std::vector<std::vector<int>>>& imagem, 
        const std::vector<int>& corFundo,
        float kB,
        float kG,
        float kR
    
    ) {

    if (imagem.empty()) {
        return {{NULL}}; 
    }
    
    int altura  = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());

    vector<vector<int>> binaria(altura, vector<int>(largura, 0));
    
    // constantes K por canal

    
    // Limares por canal
    int LB = static_cast<int>(round(kB * corFundo[0])); // B
    int LG = static_cast<int>(round(kG * corFundo[1])); // G
    int LR = static_cast<int>(round(kR * corFundo[2])); // R



    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            // Se a diferença de cor for maior que a tolerância, classifica como folha (1), senão fundo (0)
            if (imagem[y][x][0] < LB || imagem[y][x][1] < LG || imagem[y][x][2] < LR) 
                binaria[y][x] = 1;
        }
    }

    return binaria;
}

vector<vector<int>> erosao(const vector<vector<int>> &binaria, int tam_ee){

    vector<vector<int>> ee(tam_ee, vector<int>(tam_ee, 1));

    int altura = static_cast<int>(binaria.size());
    int largura = static_cast<int>(binaria[0].size());
    int raio = tam_ee / 2;

    vector<vector<int>> imagem_erodida(altura, vector<int>(largura, 0));

    vector<vector<int>> binaria_com_padding = adicionar_padding(binaria, raio, 1);

   
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++){ // Itera cada pixel da linha
            bool erodir = true;

            for (int j = -raio; j <= raio; j++) { // Itera sobre as linhas da máscara
                for (int i = -raio; i <= raio; i++) { // Itera sobre os pixels da linh

                    
                    if (ee[j + raio][i + raio] == 1 && binaria_com_padding[y + raio + j][x + raio + i] == 1) {
                        continue; // Se o pixel da máscara é 1 e o pixel correspondente na imagem binária também é 1, continue
                    } else {
                        erodir = false; 
                        break; // se falso já encerra

                    }
                }
                if (!erodir) break; // se encontrou um pixel não igual já para o processamento pra aql pixel
            }

            imagem_erodida[y][x] = erodir ? 1 : 0;
        }
    }
    
    return imagem_erodida;

}

vector<vector<int>> dilatacao(const vector<vector<int>> &binaria, int tam_ee){

    vector<vector<int>> ee(tam_ee, vector<int>(tam_ee, 1));

    int altura = static_cast<int>(binaria.size());
    int largura = static_cast<int>(binaria[0].size());
    int raio = tam_ee / 2;

    vector<vector<int>> imagem_dilatada(altura, vector<int>(largura, 0));

    vector<vector<int>> binaria_com_padding = adicionar_padding(binaria, raio, 0);

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++){ // Itera cada pixel da linha
            bool dilatar = false;

            for (int j = -raio; j <= raio; j++) { // Itera sobre as linhas da máscara
                for (int i = -raio; i <= raio; i++) { // Itera sobre os pixels da linha

                    if (ee[j + raio][i + raio] == 1 && binaria_com_padding[y + raio + j][x + raio + i] == 1) {
                        dilatar = true;
                        break; // Se encontrou pelo menos um pixel 1, já ativa e encerra a linha
                    }
                }
                if (dilatar) break; // Já encontrou um pixel ativo para este (y, x), para o processamento da máscara
            }

            imagem_dilatada[y][x] = dilatar ? 1 : 0;
        }
    }
    
    return imagem_dilatada;

}

vector<vector<int>> subtrair(const vector<vector<int>> &A, const vector<vector<int>> &B){

    vector<vector<int>> C = A;
    int altura = static_cast<int>(C.size());
    int largura = static_cast<int>(C[0].size());

    for (int y = 0; y < altura; y++) { // Itera sobre as linhas
        for (int x = 0; x < largura; x++) { // Itera cada pixel da linha
            C[y][x] -= B[y][x];
        }
    }    

    return C;
}

vector<ponto> contornarBorda(const std::vector<std::vector<int>> &imagem, ponto ponto_inicial){ // roda 1 vez, arrumar pra pegar toda a imagem de uma vez pra evitar reprocesso

    int padding = 1;

    std::vector<std::vector<int>> imagem_com_padding = adicionar_padding(imagem, padding, 0);
    
    int altura = static_cast<int>(imagem_com_padding.size());
    int largura = static_cast<int>(imagem_com_padding[0].size());
    
    vector<ponto> pontos_ordenados;
   

    // esquema pra for horário
    // parametros

    // índice:          0   1   2   3   4   5   6   7
    //                  N   NE   E  SE   S  SO   O  NO
    const int dx[8] = { 0,  1, 1, 1, 0, -1, -1, -1 };
    const int dy[8] = {-1, -1, 0, 1, 1,  1,  0, -1 };

    int d0 = 6; // oeste (direcao inicial caso tenha pelo menos uma fronteira)
    int d = 0;
    int nx = 0;
    int ny =  0;

    ponto inicio = {ponto_inicial.y + padding, ponto_inicial.x + padding}; // salva o começo do contorno (ajustado pelo padding)
    ponto b = inicio; // y x
    ponto c = {inicio.y, inicio.x - 1}; // vizinho oeste de b
    pontos_ordenados.push_back(b);
   
    
    // for (int y = 1; y < altura-1; y++) { // Itera sobre as linhas // skippa o padding
    //     for (int x = 1; x < largura-1; x++) { // Itera cada pixel da linha // skippa o padding

    //         if (imagem_com_padding[y][x] == 1){ // encontrou uma fronteira // passo 1

    //             // passo 2
    //             inicio = {y, x};
    //             b = inicio;
    //             c = {y,x-1}; // vizinho oeste de b, 
                

    //             pontos_ordenados.push_back(b); // salva

    //             break;
    //         }
    //     if (inicio.x != -1){
    //         break;
    //     }
            
    //     }
    // }

    // Dicionarios para guardar a conversão entre posição relativa
    // espacial e a direção cardial
    //  const int dx[8] = { 0,  1,  1, 1,   0, -1, -1, -1 };
    //  const int dy[8] = {-1, -1,  0, 1,   1,  1,  0, -1 };

    map<pair<int, int>, int> dicionario;

    dicionario[{0,-1}] = 0;
    dicionario[{1,-1}] = 1;
    dicionario[{1,0}] = 2;
    dicionario[{1,1}] = 3;
    dicionario[{0,1}] = 4;
    dicionario[{-1,1}] = 5;
    dicionario[{-1,0}] = 6;
    dicionario[{-1,-1}] = 7;

    // map<int, ponto> dicionario_invertido;

    // dicionario_invertido[0] = {0, -1};
    // dicionario_invertido[1] = {1,-1};
    // dicionario_invertido[2] = {1,0};
    // dicionario_invertido[3] = {1,1};
    // dicionario_invertido[4] = {0,1};
    // dicionario_invertido[5] = {-1,1};
    // dicionario_invertido[6] = {-1,0};
    // dicionario_invertido[7] = {-1,-1};
    // dicionario_invertido[-1] = {-1,-1}; // Correção para quando chave = -1

    int sentido_x = c.x - b.x; 
    int sentido_y = c.y - b.y;

    int indice = dicionario[{sentido_x, sentido_y}];
    
//  índice:             0   1   2   3   4   5   6   7
//                      N   NE  E  SE   S  SO   O  NO
//  const int dx[8] = { 0,  1,  1, 1,   0, -1, -1, -1 };
//  const int dy[8] = {-1, -1,  0, 1,   1,  1,  0, -1 };
/*           y, x
    NO N  NE   [-1,-1] [0, -1] [1, -1]
    L  b  O    [-1, 0] [0, 0 ] [1, 0 ]
    SO S  SE   [-1, 1] [0, 1 ] [1, 1 ]

               
    c = n7
    n8 n1 n2
c/n7 b  n3
    n6 n5 n4
    
    verdade apenas para a primeira iteração da busca pelo contorno

    // c está em imagem[b.y + ny][b.x + nx];
*/
    
    // Dicionário: converte {dx, dy} na direção cardeal (0..7)                                                                                       
                                                                                                                                          
                                                                                                                                                            
    int passos = 0;
    const int max_passos = altura * largura;

    do {                                                                                                                                             
        bool achou_vizinho = false;                                                                                                                  
                                                                                                                                                        
        for (int k = 0; k < 8; k++) {                                                                                                                
            int d = (indice + k) % 8;                                                                                                                
            int ny = b.y + dy[d];                                                                                                                    
            int nx = b.x + dx[d];                                                                                                                    
                                                                                                                                                        
            if (imagem_com_padding[ny][nx] == 1) {                                                                                                   
                // c recebe o vizinho anterior (de fundo) na busca horária: (d + 7) % 8                                                              
                int d_ant = (d + 7) % 8;                                                                                                             
                c = { b.y + dy[d_ant], b.x + dx[d_ant] };                                                                                            
                                                                                                                                                        
                // b avança para o novo pixel de borda                                                                                               
                b = { ny, nx };                                                                                                                      
                                                                                                                                                        
                // Atualiza o índice inicial da próxima busca horária (posição de c relativa ao novo b)                                              
                indice = dicionario[{c.x - b.x, c.y - b.y}];                                                                                         
                                                                                                                                                        
                if (b.x != inicio.x || b.y != inicio.y) {                                                                                            
                    pontos_ordenados.push_back(b);                                                                                                   
                }                                                                                                                                    
                achou_vizinho = true;                                                                                                                
                break;                                                                                                                               
            }                                                                                                                                        
        }                                                                                                                                            
                                                                                                                                                        
        if (!achou_vizinho) break; // Caso de pixel isolado (sem vizinhos == 1)                                                                      
        passos++;

    } while ((b.x != inicio.x || b.y != inicio.y) && passos < max_passos);

    // remove o padding
    for (ponto& p : pontos_ordenados) {                                                                                                                  
        p.y -= 1;                                                                                                                                        
        p.x -= 1;                                                                                                                                        
    } 

//    return static_cast<int>(pontos_ordenados.size());
    return pontos_ordenados;
}
// Aplicar morfologia matemática

vector<vector<ponto>> identificar_folhas(std::vector<std::vector<int>>& imagem_binarizada, int limite_max_folhas){

    int altura = static_cast<int>(imagem_binarizada.size());
    int largura = static_cast<int>(imagem_binarizada[0].size()); 

    vector<vector<ponto>> contornos = {};
    vector<ponto> pontos_de_borda;

    const int dx[8] = { 0,  1, 1, 1, 0, -1, -1, -1 };
    const int dy[8] = {-1, -1, 0, 1, 1,  1,  0, -1 };

    int index = 2;

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {

            if (imagem_binarizada[i][j] == 1) {

                ponto ponto_inicial = {i, j};
                pontos_de_borda = contornarBorda(imagem_binarizada, ponto_inicial);

                // Marca os pontos do contorno externo
                for (const ponto& p : pontos_de_borda) {
                    imagem_binarizada[p.y][p.x] = index;
                }

                contornos.push_back(pontos_de_borda);

                // Limpeza em vizinhança-8 (Flood Fill / BFS) para rotular toda a espessura daquela borda
                queue<ponto> fila;
                fila.push(ponto_inicial);
                imagem_binarizada[i][j] = index;

                while (!fila.empty()) {
                    ponto atual = fila.front();
                    fila.pop();

                    for (int k = 0; k < 8; k++) {
                        int ny = atual.y + dy[k];
                        int nx = atual.x + dx[k];

                        if (ny >= 0 && ny < altura && nx >= 0 && nx < largura) {
                            if (imagem_binarizada[ny][nx] == 1) {
                                imagem_binarizada[ny][nx] = index;
                                fila.push({ny, nx});
                            }
                        }
                    }
                }

                index++;
            }
        }
    }

    return contornos;
}

vector<vector<vector<int>>> adicionar_bordas(vector<vector<vector<int>>>& imagem, vector<vector<int>>& bordas){

    int altura = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());

    vector<vector<vector<int>>> imagem_bordas = imagem;

    for (int y = 0; y < altura; y++) { // Itera sobre as linhas
        for (int x = 0; x < largura; x++) { // Itera cada pixel da linha
            if (bordas[y][x] == 1){
                imagem_bordas[y][x][0] = 0;
                imagem_bordas[y][x][1] = 0;
                imagem_bordas[y][x][2] = 255;
            }

        }
    }
    
    return imagem_bordas;

}

vector<vector<vector<int>>> adicionar_bordas_unitaria(
        vector<vector<vector<int>>>& imagem,
        const vector<ponto>& borda,
        const vector<int>& cor
    ){


    for (const ponto& p : borda) {


        imagem[p.y][p.x][0] = cor[0]; // B
        imagem[p.y][p.x][1] = cor[1]; // G
        imagem[p.y][p.x][2] = cor[2]; // R
    }
    
    return imagem;

}
