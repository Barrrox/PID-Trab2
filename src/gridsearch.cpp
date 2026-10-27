#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include "funcoes.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>

using namespace std;
using namespace filesystem;
using namespace cv;

struct ImagemPreCarregada {
    string nome;
    int corFundo;
    vector<vector<int>> cinza;
};

string nomeFiltro(int metodo) {
    if (metodo == 0) return "Gauss";
    if (metodo == 1) return "Media";
    if (metodo == 2) return "Mediana";
    return "Desconhecido";
}

int main() {

    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_WARNING);

    std::cout << "#################################################" << std::endl;
    std::cout << "   Grid Search - Analisador de Fronteira de Folhas" << std::endl;
    std::cout << "   Autores: André Franco e Matheus Barros" << std::endl;
    std::cout << "#################################################\n" << std::endl;

    ///////////////////////////////////////////////////////////////////////////
    //////////////////// Hiperparâmetros Parametrizáveis //////////////////////
    ///////////////////////////////////////////////////////////////////////////

    // Intervalo fechado [40, 50] para MARGEM_FUNDO
    vector<int> margens_fundo = {40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60};

    // 3 filtros de suavização: 0 = Gaussiano, 1 = Média (Box), 2 = Mediana
    vector<int> metodos_suavizacao = {0, 1, 2};

    // 4 tamanhos de filtro de suavização: 3x3, 5x5, 7x7, 9x9
    vector<int> tamanhos_suavizacao = {3, 5, 7, 9, 11};

    // Tamanhos do elemento estruturante da erosão: 3x3, 5x5, 7x7, 9x9
    vector<int> tamanhos_ee_erosao = {3, 5, 7, 9, 11};

    // Critério de validação do número de folhas por imagem
    const int MIN_FOLHAS = 6;
    const int MAX_FOLHAS = 20;

    ///////////////////////////////////////////////////////////////////////////
    //////////////////////// Lista de Imagens do Banco ////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    vector<string> nomes_imagens = {
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
        "IMG_20230504_091510.bmp"
    };

    // Pré-carrega as imagens em escala de cinza e estima o fundo 1 única vez
    vector<ImagemPreCarregada> banco_imagens;
    std::cout << "Carregando e convertendo imagens para escala de cinza..." << std::endl;

    for (const string& nome : nomes_imagens) {
        string caminho = "imagens/" + nome;
        if (!exists(caminho)) {
            std::cout << "Aviso: Imagem nao encontrada (" << caminho << "), ignorando." << std::endl;
            continue;
        }
        cv::Mat img = imread(caminho);
        if (img.empty()) continue;

        int corFundo = estimarCorFundo(img);
        vector<vector<vector<int>>> mat_rgb = CV2vectorvector(img);
        vector<vector<int>> img_cinza = grayscale(mat_rgb);

        banco_imagens.push_back({nome, corFundo, move(img_cinza)});
    }

    std::cout << "Total de imagens carregadas: " << banco_imagens.size() << "\n" << std::endl;
    if (banco_imagens.empty()) {
        std::cout << "Erro: Nenhuma imagem carregada." << std::endl;
        return 1;
    }

    create_directories("output_gridsearch");
    ofstream csv("output_gridsearch/resultados_validos.csv");
    csv << "filtro_suavizacao;tam_suavizacao;margem_fundo;tam_ee_erosao";
    for (const auto& item : banco_imagens) {
        csv << ";" << item.nome;
    }
    csv << "\n";

    int total_combinacoes = static_cast<int>(
        metodos_suavizacao.size() *
        tamanhos_suavizacao.size() *
        margens_fundo.size() *
        tamanhos_ee_erosao.size()
    );

    int combinacao_atual = 0;
    int total_aprovadas = 0;

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////// Laço Grid Search ////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    for (int metodo : metodos_suavizacao) { // 3 
        for (int tam_suav : tamanhos_suavizacao) { // 4 

            // Cache sob demanda da imagem suavizada para o par (metodo, tam_suav) atual
            vector<vector<vector<int>>> cache_suavizada(banco_imagens.size());
            vector<bool> suavizada_calculada(banco_imagens.size(), false);

            for (int margem : margens_fundo) { // 10 
                for (int tam_ee : tamanhos_ee_erosao) { // 4 

                    combinacao_atual++;
                    bool bateria_valida = true;
                    vector<int> contagem_por_imagem(banco_imagens.size(), 0);

                    for (size_t idx = 0; idx < banco_imagens.size(); idx++) { // 35

                        // Calcula a suavização apenas na 1ª vez que esta imagem é exigida para (metodo, tam_suav)
                        if (!suavizada_calculada[idx]) {
                            cache_suavizada[idx] = suavizar(banco_imagens[idx].cinza, metodo, tam_suav);
                            suavizada_calculada[idx] = true;
                        }

                        vector<vector<int>> img_bin = binarizar(cache_suavizada[idx], banco_imagens[idx].corFundo, margem);
                        vector<vector<int>> img_erodida = erosao(img_bin, tam_ee);
                        vector<vector<int>> bordas = subtrair(img_bin, img_erodida);

                        // Passa MAX_FOLHAS para abortar cedo dentro da própria imagem se passar de 10 folhas
                        int num_folhas = identificar_folhas(bordas, MAX_FOLHAS);

                        // Critério de descarte imediato (poda da bateria)
                        if (num_folhas < MIN_FOLHAS || num_folhas > MAX_FOLHAS) {
                            bateria_valida = false;
                            std::cout << "[" << combinacao_atual << "/" << total_combinacoes << "] "
                                      << "DESCARTADO (" << nomeFiltro(metodo) << " " << tam_suav << "x" << tam_suav
                                      << ", Margem=" << margem << ", EE=" << tam_ee << "x" << tam_ee << ") -> "
                                      << banco_imagens[idx].nome << " teve " << num_folhas << " folhas." << std::endl;
                            break; // Já parte para a próxima combinação de hiperparâmetros sem salvar!
                        }

                        contagem_por_imagem[idx] = num_folhas;
                    }

                    // Se passou em TODAS as imagens no intervalo [6, 10]
                    if (bateria_valida) {
                        total_aprovadas++;
                        std::cout << "[" << combinacao_atual << "/" << total_combinacoes << "] "
                                  << ">>> APROVADO! (" << nomeFiltro(metodo) << " " << tam_suav << "x" << tam_suav
                                  << ", Margem=" << margem << ", EE=" << tam_ee << "x" << tam_ee << ")" << std::endl;

                        csv << nomeFiltro(metodo) << ";"
                            << tam_suav << ";"
                            << margem << ";"
                            << tam_ee;
                        for (int c : contagem_por_imagem) {
                            csv << ";" << c;
                        }
                        csv << "\n";
                        csv.flush();
                    }
                }
            }
        }
    }

    csv.close();
    std::cout << "\nGrid Search finalizado! Total de combinacoes aprovadas: "
              << total_aprovadas << " de " << total_combinacoes << "." << std::endl;
    std::cout << "Resultados validos salvos em: output_gridsearch/resultados_validos.csv" << std::endl;

    return 0;
}
