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

        // Estima a cor do fundo em cinza
        int corFundo = estimarCorFundo(img);

        // Converte tipo cv::Mat para matriz C++
        vector<vector<vector<int>>> imagem = CV2vectorvector(img);

        // Transforma para gray scale
        vector<vector<int>> img_cinza = grayscale(imagem);

        // suavizar imagem com kernel 5x5 de média.
        vector<vector<int>> img_suavizada = suavizar(img_cinza, metodoSuavizao);

        // Binarizar imagem, já convertendo para uma matriz no C++
        vector<vector<int>> img_bin = binarizar(img_suavizada, corFundo);

        vector<vector<int>> imagem_dilatada = dilatacao(img_bin);

        vector<vector<int>> imagem_dilatadassa = dilatacao(imagem_dilatada);

        // faz erosao fixa de 1 pixel
        vector<vector<int>> imagem_erodida = erosao(img_bin);

        // vector<vector<int>> imagem_fechamento = erosao(imagem_dilatada);

        // vector<vector<int>> imagem_abertura = dilatacao(imagem_erodida);

        vector<vector<int>> bordas = subtrair(img_bin, imagem_erodida);

        // vector<vector<int>> bordas_fechamento = subtrair(img_bin, imagem_fechamento);

        // vector<vector<int>> bordas_abertura = subtrair(img_bin, imagem_abertura);
        
        // Destaca as bordas em vermelho na imagem original

        vector<vector<vector<int>>> imagem_com_bordas = adicionar_bordas(imagem, bordas);
        
        int contagemBorda = contadorBorda(bordas);


        cout << "Altura:" << img.rows << endl;
        cout << "Largura:" << img.cols << endl;
        cout << "Total Pixels: "<< img.rows * img.cols << endl; 
        cout << "Número de pixels DE UMA FOLHA:" << contagemBorda << endl;

// 1. Cria cv::Mat de 1 canal com as dimensões de img_bin                            
        ///////////////////////////////////////////////////////////////////////////
        ////////////////////////////////// PLOTS //////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        
        // Conversões das imanges para cv::Mat para plot e salvamento com imshow

        // Converte etapas em tons de cinza (0..255) de vector<vector<int>> para cv::Mat
        cv::Mat mat_cinza(static_cast<int>(img_cinza.size()), static_cast<int>(img_cinza[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(img_cinza.size()); y++) {
            for (int x = 0; x < static_cast<int>(img_cinza[0].size()); x++) {
                mat_cinza.at<uchar>(y, x) = static_cast<uchar>(img_cinza[y][x]);
            }
        }

        cv::Mat mat_suavizada(static_cast<int>(img_suavizada.size()), static_cast<int>(img_suavizada[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(img_suavizada.size()); y++) {
            for (int x = 0; x < static_cast<int>(img_suavizada[0].size()); x++) {
                mat_suavizada.at<uchar>(y, x) = static_cast<uchar>(img_suavizada[y][x]);
            }
        }

        // Converte etapas binarias (0/1) de vector<vector<int>> para cv::Mat (0/255)
        cv::Mat mat_bin(static_cast<int>(img_bin.size()), static_cast<int>(img_bin[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(img_bin.size()); y++) {
            for (int x = 0; x < static_cast<int>(img_bin[0].size()); x++) {
                mat_bin.at<uchar>(y, x) = static_cast<uchar>(img_bin[y][x] * 255);
            }
        }

        // cv::Mat mat_dilatada(static_cast<int>(imagem_dilatada.size()), static_cast<int>(imagem_dilatada[0].size()), CV_8UC1);
        // for (int y = 0; y < static_cast<int>(imagem_dilatada.size()); y++) {
        //     for (int x = 0; x < static_cast<int>(imagem_dilatada[0].size()); x++) {
        //         mat_dilatada.at<uchar>(y, x) = static_cast<uchar>(imagem_dilatada[y][x] * 255);
        //     }
        // }

        // cv::Mat mat_dilatadassa(static_cast<int>(imagem_dilatadassa.size()), static_cast<int>(imagem_dilatadassa[0].size()), CV_8UC1);
        // for (int y = 0; y < static_cast<int>(imagem_dilatadassa.size()); y++) {
        //     for (int x = 0; x < static_cast<int>(imagem_dilatadassa[0].size()); x++) {
        //         mat_dilatadassa.at<uchar>(y, x) = static_cast<uchar>(imagem_dilatadassa[y][x] * 255);
        //     }
        // }

        cv::Mat mat_erodida(static_cast<int>(imagem_erodida.size()), static_cast<int>(imagem_erodida[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(imagem_erodida.size()); y++) {
            for (int x = 0; x < static_cast<int>(imagem_erodida[0].size()); x++) {
                mat_erodida.at<uchar>(y, x) = static_cast<uchar>(imagem_erodida[y][x] * 255);
            }
        }

        // cv::Mat mat_abertura(static_cast<int>(imagem_abertura.size()), static_cast<int>(imagem_abertura[0].size()), CV_8UC1);
        // for (int y = 0; y < static_cast<int>(imagem_abertura.size()); y++) {
        //     for (int x = 0; x < static_cast<int>(imagem_abertura[0].size()); x++) {
        //         mat_abertura.at<uchar>(y, x) = static_cast<uchar>(imagem_abertura[y][x] * 255);
        //     }
        // }

        // cv::Mat mat_fechamento(static_cast<int>(imagem_fechamento.size()), static_cast<int>(imagem_fechamento[0].size()), CV_8UC1);
        // for (int y = 0; y < static_cast<int>(imagem_fechamento.size()); y++) {
        //     for (int x = 0; x < static_cast<int>(imagem_fechamento[0].size()); x++) {
        //         mat_fechamento.at<uchar>(y, x) = static_cast<uchar>(imagem_fechamento[y][x] * 255);
        //     }
        // }

        cv::Mat mat_bordas(static_cast<int>(bordas.size()), static_cast<int>(bordas[0].size()), CV_8UC1);
        for (int y = 0; y < static_cast<int>(bordas.size()); y++) {
            for (int x = 0; x < static_cast<int>(bordas[0].size()); x++) {
                mat_bordas.at<uchar>(y, x) = static_cast<uchar>(bordas[y][x] * 255);
            }
        }

        // cv::Mat mat_bordas_fechamento(static_cast<int>(bordas_fechamento.size()), static_cast<int>(bordas_fechamento[0].size()), CV_8UC1);
        // for (int y = 0; y < static_cast<int>(bordas_fechamento.size()); y++) {
        //     for (int x = 0; x < static_cast<int>(bordas_fechamento[0].size()); x++) {
        //         mat_bordas_fechamento.at<uchar>(y, x) = static_cast<uchar>(bordas_fechamento[y][x] * 255);
        //     }
        // }
        
        // cv::Mat mat_bordas_abertura(static_cast<int>(bordas_abertura.size()), static_cast<int>(bordas_abertura[0].size()), CV_8UC1);
        // for (int y = 0; y < static_cast<int>(bordas_abertura.size()); y++) {
        //     for (int x = 0; x < static_cast<int>(bordas_abertura[0].size()); x++) {
        //         mat_bordas_abertura.at<uchar>(y, x) = static_cast<uchar>(bordas_abertura[y][x] * 255);
        //     }
        // }

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
        create_directories("output");
        imwrite("output/1_original.bmp", img);
        imwrite("output/2_cinza.bmp", mat_cinza);
        imwrite("output/3_suavizada.bmp", mat_suavizada);
        imwrite("output/4_binarizada.bmp", mat_bin);
        imwrite("output/5_erodida.bmp", mat_erodida);
        // imwrite("output/5_1_dilatacao.bmp", mat_dilatada);
        // imwrite("output/5_1_1_dilatacao2x.bmp", mat_dilatadassa);
        // imwrite("output/5_2_fechamento.bmp", mat_fechamento);
        // imwrite("output/5_3_abertura.bmp", mat_abertura);
        imwrite("output/6_bordas.bmp", mat_bordas);
        // imwrite("output/6_1_bordas_fechamento.bmp", mat_bordas_fechamento);
        // imwrite("output/6_2_bordas_abertura.bmp", mat_bordas_abertura);
        imwrite("output/7_com_bordas.bmp", mat_com_bordas);
        std::cout << "Imagens das etapas salvas em output/" << std::endl;

        // Exibe todas as etapas em janelas separadas lado a lado
        int larg_jan = 300;
        int alt_jan = 300;
        int espacamento = 310;
        int pos_y = 100;
        int pos_x = 100;

        namedWindow("1 - Original", WINDOW_NORMAL);
        resizeWindow("1 - Original", larg_jan, alt_jan);
        moveWindow("1 - Original", 0 * espacamento, pos_y);
        imshow("1 - Original", img);

        namedWindow("2 - Cinza", WINDOW_NORMAL);
        resizeWindow("2 - Cinza", larg_jan, alt_jan);
        moveWindow("2 - Cinza", 1 * espacamento, pos_y);
        imshow("2 - Cinza", mat_cinza);

        namedWindow("3 - Suavizada", WINDOW_NORMAL);
        resizeWindow("3 - Suavizada", larg_jan, alt_jan);
        moveWindow("3 - Suavizada", 2 * espacamento, pos_y);
        imshow("3 - Suavizada", mat_suavizada);

        namedWindow("4 - Binarizada", WINDOW_NORMAL);
        resizeWindow("4 - Binarizada", larg_jan, alt_jan);
        moveWindow("4 - Binarizada", 3 * espacamento, pos_y);
        imshow("4 - Binarizada", mat_bin);

        namedWindow("5 - Erodida", WINDOW_NORMAL);
        resizeWindow("5 - Erodida", larg_jan, alt_jan);
        moveWindow("5 - Erodida", 4 * espacamento, pos_y);
        imshow("5 - Erodida", mat_erodida);

        // namedWindow("5.1 - Dilatacao", WINDOW_NORMAL);
        // resizeWindow("5.1 - Dilatacao", larg_jan, alt_jan);
        // moveWindow("5.1 - Dilatacao", 0 * espacamento, pos_y);
        // imshow("5.1 - Dilatacao", mat_dilatada);

        //  namedWindow("5.1_1 - Dilatcao2x", WINDOW_NORMAL);
        // resizeWindow("5.1_1 - Dilatcao2x", larg_jan, alt_jan);
        // moveWindow("5.1_1 - Dilatcao2x", 0 * espacamento, pos_y);
        // imshow("5.1_1 - Dilatcao2x", mat_dilatadassa);

        // namedWindow("5.2 - Fechamento", WINDOW_NORMAL);
        // resizeWindow("5.2 - Fechamento", larg_jan, alt_jan);
        // moveWindow("5.2 - Fechamento", 1 * espacamento, pos_y);
        // imshow("5.2 - Fechamento", mat_fechamento);

        // namedWindow("5.3 - Abertura", WINDOW_NORMAL);
        // resizeWindow("5.3 - Abertura", larg_jan, alt_jan);
        // moveWindow("5.3 - Abertura", 2 * espacamento, pos_y);
        // imshow("5.3 - Abertura", mat_abertura);

        namedWindow("6 - Bordas", WINDOW_NORMAL);
        resizeWindow("6 - Bordas", larg_jan, alt_jan);
        moveWindow("6 - Bordas", 3 * espacamento, pos_y + espacamento + 30);
        imshow("6 - Bordas", mat_bordas);

        // namedWindow("6.1 - Bordas_fechamento", WINDOW_NORMAL);
        // resizeWindow("6.1 - Bordas_fechamento", larg_jan, alt_jan);
        // moveWindow("6.1 - Bordas_fechamento", 4 * espacamento, pos_y + espacamento + 30);
        // imshow("6.1 - Bordas_fechamento", mat_bordas_fechamento);

        // namedWindow("6.2 - Bordas_abertura", WINDOW_NORMAL);
        // resizeWindow("6.2 - Bordas_abertura", larg_jan, alt_jan);
        // moveWindow("6.2 - Bordas_abertura", 0 * espacamento, pos_y + espacamento + 30);
        // imshow("6.2 - Bordas_abertura", mat_bordas_abertura);

        namedWindow("7 - Com Bordas", WINDOW_NORMAL);
        resizeWindow("7 - Com Bordas", larg_jan, alt_jan);
        moveWindow("7 - Com Bordas", 1 * espacamento, pos_y + espacamento + 30);
        imshow("7 - Com Bordas", mat_com_bordas);

        std::cout << "Aperte ESPAÇO para proxima imagem ou ESC para encerrar" << std::endl;
        int tecla = waitKey(0);
        if (tecla == 27) { // ESC
            destroyAllWindows();
            break;
        }
        destroyAllWindows();
    }
}
