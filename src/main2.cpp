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

const int METODO_SUAVIZACAO = 2; // 0 = Gaussiano, 1 = Media, 2 = Mediana
const int TAM_SUAVIZACAO = 3;
const int TAM_EE = 5;
const float KB = 0.43f;
const float KG = 0.68f;
const float KR = 0.68f;

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

    create_directories("output2");

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

        // 1. Estima a cor do fundo em BGR e converte cv::Mat para matriz C++
        vector<int> corFundoBGR = estimarCorFundoBGR(img);
        vector<vector<vector<int>>> imagem = CV2vectorvector(img);

        // 2. Suaviza em BGR (Mediana 3x3) e binariza pelos limiares dinâmicos
        vector<vector<vector<int>>> img_suavizada = suavizarBGR(imagem, METODO_SUAVIZACAO, TAM_SUAVIZACAO);
        vector<vector<int>> img_bin = binarizarBGR(img_suavizada, corFundoBGR, KB, KG, KR);

        // 3. Fechamento morfologico (dilatacao + erosao) para fechar buracos internos
        vector<vector<int>> imagem_dilatada = dilatacao(img_bin, TAM_EE);
        vector<vector<int>> imagem_fechamento = erosao(imagem_dilatada, TAM_EE);

        // 4. Extrai fronteira morfologica: bordas = Fechamento - (Fechamento (-) B)
        vector<vector<int>> imagem_erodida = erosao(imagem_fechamento, TAM_EE);
        vector<vector<int>> bordas = subtrair(imagem_fechamento, imagem_erodida);

        // 5. Destaca todas as bordas em vermelho na imagem original (antes de rotular bordas)
        vector<vector<vector<int>>> imagem_com_bordas = adicionar_bordas(imagem, bordas);

        // 6. Rastreia as fronteiras de cada folha (Gonzalez & Woods) e conta pixels
        vector<vector<ponto>> folhas = identificar_folhas(bordas);

        std::cout << "\n[" << nome_imagem << "] Total de folhas: " << folhas.size() << std::endl;
        for (size_t k = 0; k < folhas.size(); k++) {
            std::cout << "  Folha " << (k + 1) << ": " << folhas[k].size() << " pixels na fronteira" << std::endl;
        }

        ///////////////////////////////////////////////////////////////////////////
        ////////////////////////////////// PLOTS //////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        // Converte imagem com bordas vermelhas (3 canais BGR) para cv::Mat (CV_8UC3)
        int altura = static_cast<int>(imagem_com_bordas.size());
        int largura = static_cast<int>(imagem_com_bordas[0].size());
        cv::Mat mat_saida(altura, largura, CV_8UC3);
        for (int y = 0; y < altura; y++) {
            for (int x = 0; x < largura; x++) {
                mat_saida.at<cv::Vec3b>(y, x) = cv::Vec3b(
                    static_cast<uchar>(imagem_com_bordas[y][x][0]),
                    static_cast<uchar>(imagem_com_bordas[y][x][1]),
                    static_cast<uchar>(imagem_com_bordas[y][x][2])
                );
            }
        }

        string caminho_salvar = "output2/" + nome_imagem;
        imwrite(caminho_salvar, mat_saida);
        std::cout << "  -> Salva em: " << caminho_salvar << std::endl;
    }
    std::cout << "\nProcessamento concluido! Imagens salvas em output2/" << std::endl;
    return 0;
}
