#include <iostream> 
using namespace std;
#include <filesystem>
using namespace filesystem;
#include <vector>
#include "funcoes.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

#include <opencv2/core/utils/logger.hpp> // Para silenciar INFOs do openCV

#define TOLERANCIA NULL // Deixado nulo para escolher depois

int main() {

    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_WARNING); // Para silenciar INFOs do openCV

    // Pega o path pra pasta de imagens
    path pasta = path("imagens");

    cout << "#################################################" << endl;
    cout << "#################################################" << endl;
    cout << "#################################################" << endl;
    cout << "\n   Analisador de Fronteira de Folhas" << endl;
    cout << "   Autores: André Franco e Matheus Barros" << endl;
    cout << endl;
    cout << "#################################################" << endl;
    cout << "#################################################" << endl;
    cout << "#################################################" << endl;
    cout << endl;


    // mudar essa condição do while
    // Deixa o usuário escolher qual imagem processar agora
    while(true) {

        // Captura o caminho da imagem
        string caminho;
        cout << "\nDigite o caminho da imagem (ou 'sair' para encerrar): ";
        getline(cin, caminho);

        // Se usuario digitou sair, encerra
        if (caminho == "sair") {
            cout << "Encerrando o programa..." << endl;
            return 0;
        }

        // Verifica se o arquivo realmente existe ou se caminho vazio
        if (caminho.empty() || !exists(caminho)) {
            cout << "Erro: O arquivo nao foi encontrado: " << caminho << endl;
            continue;
        }

        cv::Mat img = imread(caminho);
        
        // Verifica se abriu corretamente
        if (img.empty()) {
            cout << "Erro ao abrir a imagem: " << caminho << endl;
            return 1;
        }

        ////////////////////////////////////////////////////////////////////
        /* Essa seção comentada tinha a lógica para converter para a nossa representação de imagem e pixel

        // Copia para a estrutura própria ImagemBGR
        ImagemBGR imagem;
        imagem.largura = img.cols;
        imagem.altura = img.rows;

        // Aloca o espaço necessário (Largura * Altura * 3 canais BGR)
        imagem.dados.resize(static_cast<size_t>(imagem.largura) * imagem.altura * 3);
        
        // Copiar dados
        BGR pixel;
        for(int y = 0; y < imagem.altura; y++){ // Iterar sobre altura (y)
            for(int x = 0; x < imagem.largura; x++){ // Iterar sobre largura (x)
                
                // Descobro o index correto
                size_t idx = (static_cast<size_t>(y) * imagem.largura + x) * 3;

                pixel.r = img.data[idx + 2];
                pixel.g = img.data[idx + 1];
                pixel.b = img.data[idx];
                
                imagem.definirPixel(x,y, pixel);
            }
        } */
        ////////////////////////////////////////////////////////////////////
        

        // Binarizar imagem
        // img_bin = binarizar(img, TOLERANCIA)

        // faz negativa
        
        // faz erosao
        
        // imagem = imagem - copia_erosao;

        // exibe em tela a imagem processada
        
        cout << "Aperte ENTER para exibir os contornos e contagem de pixels da imagem processada" << endl;

        // começa varredura de [0, largura) e [0, altura) da imagem processada
            // encontrou contorno
                // conta pixels do contorno
                // exibe contorno e quantidade de pixels encontrados

        cout << "Aperte ESPAÇO para selecionar a proxima imagem ou ESC para encerrar o programa" << endl;

        string NomeJanela = "Janela 1";
        namedWindow(NomeJanela, WINDOW_NORMAL);
        imshow(NomeJanela, img);

        int tecla = waitKey(0); // waitKey só funciona se usar o imshow do cv, o que não podemos fazer. Alternativa: Usar cin do c++ mesmo

        if (tecla == 13) { // ENTER
            cout << "IMPLEMENTAR ANÁLISE";
            destroyWindow(NomeJanela);
            break; 
        } else if (tecla == 27) { // ESC
            break; 
        } else if (tecla == 32) { // ESPAÇO 
            destroyWindow(NomeJanela);
            continue;
            
        }
        else {
            cout << "Tecla inválida. Aperte ESPAÇO para selecionar a proxima imagem ou ESC para encerrar o programa." << endl;
        }
    
    }

    cout << "\nEncerrando programa..." << endl;
    destroyAllWindows();
    return 0;
}
    