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

//#define TOLERANCIA 0.3 // Deixado nulo para escolher depois

int main() {

    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_WARNING); // Para silenciar INFOs do openCV

    // Pega o path pra pasta de imagens
    path pasta = path("imagens");

    std::cout << "#################################################" << std::endl;
    std::cout << "#################################################" << std::endl;
    std::cout << "#################################################" << std::endl;
    std::cout << "\n   Analisador de Fronteira de Folhas" << std::endl;
    std::cout << "   Autores: André Franco e Matheus Barros" << std::endl;
    std::cout << std::endl;
    std::cout << "#################################################" << std::endl;
    std::cout << "#################################################" << std::endl;
    std::cout << "#################################################" << std::endl;
    std::cout << std::endl;

    std::vector<std::string> imagens = {
        "IMG_20230504_090659.bmp",
        "IMG_20230504_090748.bmp",
        "IMG_20230504_090851.bmp",
        "IMG_20230504_090854.bmp",
        "IMG_20230504_090900.bmp",
        "IMG_20230504_090908.bmp",
        "IMG_20230504_091023.bmp",
        "IMG_20230504_091027.bmp",
        "IMG_20230504_091033.bmp",
        "IMG_20230504_091037.bmp",
        "IMG_20230504_091116.bmp",
        "IMG_20230504_091121.bmp",
        "IMG_20230504_091129.bmp",
        "IMG_20230504_091133.bmp",
        "IMG_20230504_091140.bmp",
        "IMG_20230504_091235.bmp",
        "IMG_20230504_091238.bmp",
        "IMG_20230504_091241.bmp",
        "IMG_20230504_091243.bmp",
        "IMG_20230504_091321.bmp",
        "IMG_20230504_091322.bmp",
        "IMG_20230504_091325.bmp",
        "IMG_20230504_091328.bmp",
        "IMG_20230504_091331.bmp",
        "IMG_20230504_091412.bmp",
        "IMG_20230504_091414.bmp",
        "IMG_20230504_091416.bmp",
        "IMG_20230504_091421.bmp",
        "IMG_20230504_091423.bmp",
        "IMG_20230504_091425.bmp",
        "IMG_20230504_091459.bmp",
        "IMG_20230504_091501.bmp",
        "IMG_20230504_091503.bmp",
        "IMG_20230504_091505.bmp",
        "IMG_20230504_091510.bmp",
    };

    // Usando const& para evitar cópias desnecessárias de string
    


    // mudar essa condição do while
    // Deixa o usuário escolher qual imagem processar agora
   for (const std::string& nome_imagem : imagens) {

        string caminho_imagem = "imagens/" + nome_imagem;

        // Verifica se o arquivo realmente existe ou se caminho vazio
        if (caminho_imagem.empty() || !exists(caminho_imagem)) {
            std::cout << "Erro: O arquivo nao foi encontrado: " << caminho_imagem << std::endl;
            continue;
        }

        cv::Mat img = imread(caminho_imagem);

        ///////////////////////////////////////////////////////////////////////////
        //////////////////////// Pipeline de processamento ////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        // Estima a cor do fundo em cinza
        int corFundo = estimarCorFundo(img);

        // Converte tipo cv::Mat para matriz C++
        vector<vector<vector<int>>> imagem = CV2vectorvector(img);

        // Transforma para gray scale
        vector<vector<int>> img_cinza = grayscale(imagem);

        // suavizar imagem com kernel 5x5 de média.
        vector<vector<int>> img_suavizada = suavizar(img_cinza);

        // Binarizar imagem, já convertendo para uma matriz no C++
        vector<vector<int>> img_bin = binarizar(img_suavizada, corFundo);

        // faz erosao fixa de 1 pixel
        vector<vector<int>> imagem_erodida = erosao(img_bin);

        vector<vector<int>> bordas = subtrair(img_bin, imagem_erodida);
        
        // Destaca as bordas em vermelho na imagem original

        vector<vector<vector<int>>> imagem_com_bordas = adicionar_bordas(imagem, bordas);

        ///////////////////////////////////////////////////////////////////////////
        ////////////////////////////////// PLOTS //////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        // Converte imagem com bordas vermelhas (3 canais BGR) para cv::Mat (CV_8UC3)
        cv::Mat mat_com_bordas(static_cast<int>(imagem_com_bordas.size()), static_cast<int>(imagem_com_bordas[0].size()), CV_8UC3);
        for (int y = 0; y < static_cast<int>(imagem_com_bordas.size()); y++) {
            for (int x = 0; x < static_cast<int>(imagem_com_bordas[0].size()); x++) {
                mat_com_bordas.at<cv::Vec3b>(y, x) = cv::Vec3b(
                    static_cast<uchar>(imagem_com_bordas[y][x][0]),
                    static_cast<uchar>(imagem_com_bordas[y][x][1]),
                    static_cast<uchar>(imagem_com_bordas[y][x][2])
                );
            }
        }

        // Cria pasta output/ se nao existir e salva as imagens de cada etapa
        create_directories("output2");

        string caminho_salvar = "output/" + nome_imagem;
        imwrite(caminho_salvar, mat_com_bordas);
        
        std::cout << "Imagem salva em" << caminho_salvar << std::endl;
    }
    std::cout << "Imagens das etapas salvas em output/" << std::endl;

}
