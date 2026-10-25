#include "funcoes.h"
#include <cmath>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <map>
using namespace cv;
using namespace std;
#include <opencv2/core/utils/logger.hpp> // Para silenciar INFOs do openCV

const int MARGEM_FUNDO = 65;

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

vector<vector<int>> suavizar(const vector<vector<int>>& imagem){

    // soma dos pesos = 256
    vector<vector<int>> gaussiano = {
        {1,  4,  6,  4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1,  4,  6,  4, 1}
    };
    // soma dos pesos = 25
    vector<vector<int>> box = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };
    

    vector<vector<int>> mascara = box;

    int somaPesos = 0;
    for (const auto& linha : mascara) {
        for (const auto& valor : linha) {
            
            somaPesos += valor;
        }
    }
    
    int altura  = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());

    int raio = static_cast<int>(mascara.size()/2);

    vector<vector<int>> imagem_com_padding = adicionar_padding(imagem, raio, 255);

    vector<vector<int>> suavizada(altura, vector<int>(largura, 0));

    
        for (int y = 0; y < altura; y++) { // opera sobre as linhas
            for (int x = 0; x < largura; x++) { // opera sobre os elementos da coluna na linha y
                int soma = 0;
                
                for (int j = -raio; j <= raio; j++) { // Itera sobre as linhas da máscara
                    for (int i = -raio; i <= raio; i++) { // Itera sobre os pixels da linha

                    // a possicao acessada é: posicao na imagem original + deslocamento do padding + posição atual da mascara
                    // isso pra linha e coluna
                        soma += imagem_com_padding[y + raio + j][x + raio + i] * mascara[j + raio][i + raio];
        
                    }
                }
                // Opera o pixel
                suavizada[y][x] = soma / somaPesos; 
                
            }
        }
    return suavizada;
}

vector<vector<int>> binarizar(const vector<vector<int>>& imagem_cinza, int fundoCinza) {

        if (imagem_cinza.empty()) {
        return imagem_cinza; 
    }
    
    int altura  = static_cast<int>(imagem_cinza.size());
    int largura = static_cast<int>(imagem_cinza[0].size());

    vector<vector<int>> binaria(altura, vector<int>(largura, 0));
    
    // Cor média de referência das bordas já em gray scale
    fundoCinza -= MARGEM_FUNDO; // revisar e incluior no relatório

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            // Se a diferença de cor for maior que a tolerância, classifica como folha (1), senão fundo (0)
            if (imagem_cinza[y][x] < fundoCinza) binaria[y][x] = 1;
        }
    }

    return binaria;
}

vector<vector<int>> erosao(const vector<vector<int>> &binaria){ //  coloquei const e  & por padronização, nao se se aplica aqui

    // EE = uma máscara que representa algum objeto (acho que usaremos um quadrado 3x3)
    // Passamos essa máscara por cada pixel da imagem e comparamos a máscara com a 
    // os vizinhos do pixel. Se todos forem iguais, o pixel é 1, senão o pixel é 0.



    vector<vector<int>> elemento_estruturante = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };
    
    int altura = static_cast<int>(binaria.size());
    int largura = static_cast<int>(binaria[0].size());
    int raio = static_cast<int>(elemento_estruturante.size()) / 2;

    vector<vector<int>> imagem_erodida(altura, vector<int>(largura, 0));

    vector<vector<int>> binaria_com_padding = adicionar_padding(binaria, raio, 1);

   
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++){ // Itera cada pixel da linha
            bool erodir = true;

            for (int j = -raio; j <= raio; j++) { // Itera sobre as linhas da máscara
                for (int i = -raio; i <= raio; i++) { // Itera sobre os pixels da linh

                    
                    if (elemento_estruturante[j + raio][i + raio] == 1 && binaria_com_padding[y + raio + j][x + raio + i] == 1) {
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

int contadorBorda(const std::vector<std::vector<int>> &imagem){ // roda 1 vez, arrumar pra pegar toda a imagem de uma vez pra evitar reprocesso

    std::vector<std::vector<int>> imagem_com_padding = adicionar_padding(imagem, 1, 0);
    
    int altura = static_cast<int>(imagem.size());
    int largura = static_cast<int>(imagem[0].size());
    
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

    ponto inicio = {-1, -1}; // salva o começo do contorno
    ponto b = {-1,-1}; // y x
    ponto c = {-1,-1}; // y x
   
    
    for (int y = 1; y < altura-1; y++) { // Itera sobre as linhas // skippa o padding
        for (int x = 1; x < largura-1; x++) { // Itera cada pixel da linha // skippa o padding

            if (imagem[y][x] == 1){ // encontrou uma fronteira // passo 1

                // passo 2
                b = {y, x};
                c = {y,x-1}; // vizinho oeste de b, 
                inicio = b;

                pontos_ordenados.push_back(b); // salva
            }
        }
    }

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

    map<int, ponto> dicionario_invertido;

    dicionario_invertido[0] = {0, -1};
    dicionario_invertido[1] = {1,-1};
    dicionario_invertido[2] = {1,0};
    dicionario_invertido[3] = {1,1};
    dicionario_invertido[4] = {0,1};
    dicionario_invertido[5] = {-1,1};
    dicionario_invertido[6] = {-1,0};
    dicionario_invertido[7] = {-1,-1};
    dicionario_invertido[-1] = {-1,-1}; // Correção para quando chave = -1

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
    
    // continua enquanto não fizer a volta toda
    while(b.x != inicio.x && b.y != inicio.y){
       
        // varre em sentido horário // passo 3

        for(int k = 0; k < 8; k++){

            d = (indice + k) % 8;
            ny = b.y + dy[d];
            nx = b.x + dx[d];
            
            if (imagem[ny][nx] == 1) { // novo pixel de contorno // passo 3 pra  4

                // passo 4
                b = {ny, nx};
                ponto c2 = dicionario_invertido[indice-1]; // indice-1 da a posiução de c relativa a b

                c.y += c2.y; // aqui ja é o c novo
                c.x += c2.x;
                
                d = indice-1; // na prox iteração a busca em sentido horário começa da posição relativa de c para b
                
                pontos_ordenados.push_back(b);
                break;
            }
        }
    }

    return static_cast<int>(pontos_ordenados.size());
}
// Aplicar morfologia matemática
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
