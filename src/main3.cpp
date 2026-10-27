#include <iostream> 
using namespace std;
#include <filesystem>
using namespace filesystem;
#include <vector>
#include <string_view>
#include "funcoes.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

#include <opencv2/core/utils/logger.hpp> // Para silenciar INFOs do openCV

/* Suaviz a imagem. Métodos:
0 = Filtro Gaussiano 5x5
1 = Filtro Box Média 5x5
2 = Filtro por Mediana 5x5
*/
#define metodoSuavizao 1

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

    create_directories("output3");

    for (const std::string& nome_imagem : imagens) {

        string caminho_imagem = "imagens/" + nome_imagem;

        // Verifica se o arquivo realmente existe ou se caminho vazio
        if (caminho_imagem.empty() || !exists(caminho_imagem)) {
            std::cout << "Erro: O arquivo nao foi encontrado: " << caminho_imagem << std::endl;
            continue;
        }

        cv::Mat img = imread(caminho_imagem);

        if (img.empty()) {
            std::cout << "Erro ao abrir a imagem: " << caminho_imagem << std::endl;
            continue;
        }

        ///////////////////////////////////////////////////////////////////////////
        //////////////////////// Pipeline de processamento ////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        // Estima a cor do fundo em cinza
        int corFundo = estimarCorFundo(img);

        // Converte tipo cv::Mat para matriz C++
        vector<vector<vector<int>>> imagem = CV2vectorvector(img);

        // Transforma para gray scale
        vector<vector<int>> img_cinza = grayscale(imagem);

        // Suavizar imagem
        vector<vector<int>> img_suavizada = suavizar(img_cinza, metodoSuavizao);

        // Binarizar imagem
        vector<vector<int>> img_bin = binarizar(img_suavizada, corFundo);

        // Passo 5.1 - Dilatacao
        vector<vector<int>> imagem_dilatada = dilatacao(img_bin);

        // Passo 5.2 - Fechamento (erosao da imagem dilatada)
        vector<vector<int>> imagem_fechamento = erosao(imagem_dilatada);

        ///////////////////////////////////////////////////////////////////////////
        ////////////////////////////////// PLOTS //////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        // Converte 5.1 (dilatacao) para cv::Mat (0/255)
        cv::Mat mat_dilatada(static_cast<int>(imagem_dilatada.size()), static_cast<int>(imagem_dilatada[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(imagem_dilatada.size()); y++) {
            for (int x = 0; x < static_cast<int>(imagem_dilatada[0].size()); x++) {
                mat_dilatada.at<uchar>(y, x) = static_cast<uchar>(imagem_dilatada[y][x] * 255);
            }
        }

        // Converte 5.2 (fechamento) para cv::Mat (0/255)
        cv::Mat mat_fechamento(static_cast<int>(imagem_fechamento.size()), static_cast<int>(imagem_fechamento[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(imagem_fechamento.size()); y++) {
            for (int x = 0; x < static_cast<int>(imagem_fechamento[0].size()); x++) {
                mat_fechamento.at<uchar>(y, x) = static_cast<uchar>(imagem_fechamento[y][x] * 255);
            }
        }

        string_view nome_base = nome_imagem;
        nome_base.remove_suffix(4); // remove ".bmp"

        string caminho_salvar_D = "output3/" + static_cast<std::string>(nome_base) + "_D.bmp";
        string caminho_salvar_F = "output3/" + static_cast<std::string>(nome_base) + "_F.bmp";

        imwrite(caminho_salvar_D, mat_dilatada);
        imwrite(caminho_salvar_F, mat_fechamento);

        std::cout << "Imagens salvas: " << caminho_salvar_D << " e " << caminho_salvar_F << std::endl;
    }

    std::cout << "Imagens das etapas salvas em output3/" << std::endl;
    return 0;
}
