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

/* Suaviz a imagem. Métodos:
0 = Filtro Gaussiano 5x5
1 = Filtro Box Média 5x5
2 = Filtro por Mediana 5x5
*/

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


    // mudar essa condição do while
    // Deixa o usuário escolher qual imagem processar agora
    while(true) {

        // Captura o caminho da imagem
        string caminho;
        std::cout << "\nDigite o caminho da imagem (ou 'sair' para encerrar): ";
        getline(cin, caminho);

        // Se usuario digitou sair, encerra
        if (caminho == "sair") {
            std::cout << "Encerrando o programa..." << std::endl;
            return 0;
        }

        // Verifica se o arquivo realmente existe ou se caminho vazio
        if (caminho.empty() || !exists(caminho)) {
            std::cout << "Erro: O arquivo nao foi encontrado: " << caminho << std::endl;
            continue;
        }

        cv::Mat img = imread(caminho);
        
        // Verifica se abriu corretamente
        if (img.empty()) {
            std::cout << "Erro ao abrir a imagem: " << caminho << std::endl;
            return 1;
        }

        ///////////////////////////////////////////////////////////////////////////
        //////////////////////// Pipeline de processamento ////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        vector<int> corFundoBGR = estimarCorFundoBGR(img);

        // Converte tipo cv::Mat para matriz C++
        vector<vector<vector<int>>> imagem = CV2vectorvector(img);        

        HistogramaBGR hist = gerar_histograma(imagem);

        vector<vector<vector<int>>> img_suavizadaBGR = suavizarBGR(imagem, 1, 7);

        vector<vector<int>> img_bin = binarizarBGR(img_suavizadaBGR, corFundoBGR,
                0.43, 0.68, 0.68);

        vector<vector<int>> imagem_dilatada = dilatacao(img_bin, 5);

        vector<vector<int>> imagem_fechamento = erosao(imagem_dilatada, 5); // erosao da imagem dilatada == fechamento

        vector<vector<int>> imagem_erodida = erosao(imagem_fechamento, 3); // erosao do fechamento para extrair a borda

        vector<vector<int>> bordas = subtrair(imagem_fechamento, imagem_erodida);

        vector<vector<vector<int>>> imagem_com_bordas = adicionar_bordas(imagem, bordas);

        // Identifica as folhas e extrai os contornos ordenados por indice
        vector<vector<ponto>> contornos_todos = identificar_folhas(bordas);

        // Filtra ruidos (< 500 pixels)
        const size_t LIMIAR_MIN_PIXELS = 500;
        vector<vector<ponto>> contornos;
        for (const auto& c : contornos_todos) {
            if (c.size() >= LIMIAR_MIN_PIXELS) {
                contornos.push_back(c);
            }
        }


        cout << "Altura:" << img.rows << endl;
        cout << "Largura:" << img.cols << endl;
        cout << "Total Pixels: "<< img.rows * img.cols << endl; 
        cout << "Total de folhas identificadas: " << contornos.size() << endl;


        // Converte 2_suavizada (3 canais BGR) para cv::Mat (CV_8UC3)
        cv::Mat mat_suavizada(static_cast<int>(img_suavizadaBGR.size()), static_cast<int>(img_suavizadaBGR[0].size()), CV_8UC3);
        for (int y = 0; y < static_cast<int>(img_suavizadaBGR.size()); y++) {
            for (int x = 0; x < static_cast<int>(img_suavizadaBGR[0].size()); x++) {
                mat_suavizada.at<cv::Vec3b>(y, x) = cv::Vec3b(
                    static_cast<uchar>(img_suavizadaBGR[y][x][0]),
                    static_cast<uchar>(img_suavizadaBGR[y][x][1]),
                    static_cast<uchar>(img_suavizadaBGR[y][x][2])
                );
            }
        }

        // Converte etapas binarias (0/1) de vector<vector<int>> para cv::Mat (0/255)
        cv::Mat mat_bin(static_cast<int>(img_bin.size()), static_cast<int>(img_bin[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(img_bin.size()); y++) {
            for (int x = 0; x < static_cast<int>(img_bin[0].size()); x++) {
                mat_bin.at<uchar>(y, x) = static_cast<uchar>(img_bin[y][x] * 255);
            }
        }

        cv::Mat mat_dilatada(static_cast<int>(imagem_dilatada.size()), static_cast<int>(imagem_dilatada[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(imagem_dilatada.size()); y++) {
            for (int x = 0; x < static_cast<int>(imagem_dilatada[0].size()); x++) {
                mat_dilatada.at<uchar>(y, x) = static_cast<uchar>(imagem_dilatada[y][x] * 255);
            }
        }

        cv::Mat mat_fechamento(static_cast<int>(imagem_fechamento.size()), static_cast<int>(imagem_fechamento[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(imagem_fechamento.size()); y++) {
            for (int x = 0; x < static_cast<int>(imagem_fechamento[0].size()); x++) {
                mat_fechamento.at<uchar>(y, x) = static_cast<uchar>(imagem_fechamento[y][x] * 255);
            }
        }

        cv::Mat mat_erodida(static_cast<int>(imagem_erodida.size()), static_cast<int>(imagem_erodida[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(imagem_erodida.size()); y++) {
            for (int x = 0; x < static_cast<int>(imagem_erodida[0].size()); x++) {
                mat_erodida.at<uchar>(y, x) = static_cast<uchar>(imagem_erodida[y][x] * 255);
            }
        }

        cv::Mat mat_bordas(static_cast<int>(bordas.size()), static_cast<int>(bordas[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(bordas.size()); y++) {
            for (int x = 0; x < static_cast<int>(bordas[0].size()); x++) {
                mat_bordas.at<uchar>(y, x) = static_cast<uchar>((bordas[y][x] != 0) ? 255 : 0);
            }
        }

        // Converte imagem com bordas (3 canais BGR) para cv::Mat (CV_8UC3)
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
        create_directories("output");
        imwrite("output/1_original.bmp", img);
        salvar_histograma(hist, "output/histograma.bmp");
        imwrite("output/2_suavizada.bmp", mat_suavizada);
        imwrite("output/3_binarizada.bmp", mat_bin);
        imwrite("output/4_dilatada.bmp", mat_dilatada);
        imwrite("output/5_fechamento.bmp", mat_fechamento);
        imwrite("output/6_erodida.bmp", mat_erodida);
        imwrite("output/7_bordas.bmp", mat_bordas);
        imwrite("output/8_com_bordas.bmp", mat_com_bordas);
        std::cout << "Imagens das etapas 1 a 8 salvas em output/" << std::endl;

        bool encerrar = false;
        for (size_t i = 0; i < contornos.size(); i++) {
            // Pinta o contorno atual de vermelho
            imagem = adicionar_bordas_unitaria(imagem, contornos[i], {0, 0, 255});

            for (int y = 0; y < static_cast<int>(imagem.size()); y++) {
                for (int x = 0; x < static_cast<int>(imagem[0].size()); x++) {
                    mat_com_bordas.at<cv::Vec3b>(y, x) = cv::Vec3b(
                        static_cast<uchar>(imagem[y][x][0]),
                        static_cast<uchar>(imagem[y][x][1]),
                        static_cast<uchar>(imagem[y][x][2])
                    );
                }
            }

            namedWindow("8 - Com Bordas", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
            resizeWindow("8 - Com Bordas", 1200, 850); // Cabe inteira na tela sem distorcer
            imshow("8 - Com Bordas", mat_com_bordas);

            std::cout << "Folha " << (i + 1) << " de " << contornos.size()
                      << " | Pixels na fronteira: " << contornos[i].size()
                      << " (Pressione ENTER para proxima folha)" << std::endl;

            while (true) {
                int tecla_folha = waitKey(0);
                if (tecla_folha == 13 || tecla_folha == 10) { //  enter pra  windows e linux
                    break;
                }
                if (tecla_folha == 27) { //  tecla ESC
                    encerrar = true;
                    break;
                }
            }
            if (encerrar) break;

            // Volta o contorno atual para BRANCO antes de pintar o proximo
            imagem = adicionar_bordas_unitaria(imagem, contornos[i], {255, 255, 255}); //  B G R
        }

        if (encerrar) {
            destroyAllWindows();
            break;
        }

        std::cout << "Aperte ESPAÇO para proxima imagem ou ESC para encerrar" << std::endl;
        int tecla = waitKey(0);
        if (tecla == 27) { // ESC
            destroyAllWindows();
            break;
        }
        destroyAllWindows();
    }
}
