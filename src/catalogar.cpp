#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include "funcoes.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>

using namespace std;
using namespace filesystem;
using namespace cv;

const int METODO_SUAVIZACAO = 1; // 0 = Gaussiano, 1 = Media, 2 = Mediana
const int TAM_SUAVIZACAO = 7;
const int TAM_EE = 5;
const int TAM_EE_extracao_borda = 3; // Especifico para extração de borda
const float KB = 0.43f;
const float KG = 0.68f;
const float KR = 0.68f;
const size_t LIMIAR_MIN_PIXELS = 500;
const string ARQUIVO_CSV = "analise_folhas.csv";

struct RegistroFolha {
    string imagem;
    size_t idx_folha;
    int id_especie;
    string nome_especie;
    size_t pixels_borda;
    int y_inicial;
    int x_inicial;
};

string nomeEspecie(int id) {
    switch (id) {
        case 0: return "Ruido";
        case 1: return "Folhado";
        case 2: return "Araca";
        case 3: return "Quaresmeira";
        case 4: return "Pessegueiro";
        case 5: return "Coleus";
        case 6: return "Uva_do_mato";
        default: return "Desconhecido";
    }
}

set<string> carregarImagensJaCatalogadas(const string& caminho_csv) {
    set<string> processadas;
    if (!exists(caminho_csv)) {
        return processadas;
    }

    ifstream in(caminho_csv);
    string linha;
    bool primeira = true;
    while (getline(in, linha)) {
        if (primeira) {
            primeira = false;
            continue;
        }
        if (linha.empty()) continue;
        stringstream ss(linha);
        string nome_img;
        if (getline(ss, nome_img, ',') && !nome_img.empty()) {
            processadas.insert(nome_img);
        }
    }
    return processadas;
}

int main() {
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_WARNING);

    cout << "#################################################" << endl;
    cout << "   Catalogador Interativo de Especies - PID 2026" << endl;
    cout << "   Autores: Andre Franco e Matheus Barros" << endl;
    cout << "#################################################\n" << endl;

    path pasta("imagens");
    if (!exists(pasta) || !is_directory(pasta)) {
        cout << "Erro: Pasta 'imagens/' nao encontrada." << endl;
        return 1;
    }

    vector<string> imagens;
    for (const auto& entry : directory_iterator(pasta)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bmp") {
            imagens.push_back(entry.path().filename().string());
        }
    }
    sort(imagens.begin(), imagens.end());

    set<string> concluidas = carregarImagensJaCatalogadas(ARQUIVO_CSV);

    // Se o arquivo CSV nao existir, cria com o cabecalho
    if (!exists(ARQUIVO_CSV)) {
        ofstream out(ARQUIVO_CSV, ios::out);
        out << "imagem,idx_folha,id_especie,nome_especie,pixels_borda,y_inicial,x_inicial\n";
        out.close();
    }

    cout << "Total de imagens no banco: " << imagens.size() << endl;
    cout << "Imagens ja catalogadas:    " << concluidas.size() << endl;
    cout << "\nTeclas de classificacao na janela da imagem:" << endl;
    cout << "  [1] Folhado      [2] Araca        [3] Quaresmeira" << endl;
    cout << "  [4] Pessegueiro  [5] Coleus       [6] Uva do mato" << endl;
    cout << "  [0] ou [R] Ruido" << endl;
    cout << "  [ESC] Encerrar (salva ate a ultima imagem concluida)\n" << endl;

    namedWindow("Catalogador - Folha Atual", WINDOW_NORMAL | WINDOW_KEEPRATIO);
    resizeWindow("Catalogador - Folha Atual", 1200, 850);

    bool encerrar = false;

    for (size_t idx_img = 0; idx_img < imagens.size(); idx_img++) {
        const string& nome_imagem = imagens[idx_img];

        if (concluidas.count(nome_imagem)) {
            cout << "[" << (idx_img + 1) << "/" << imagens.size() << "] Pulando (ja catalogada): " << nome_imagem << endl;
            continue;
        }

        string caminho_imagem = "imagens/" + nome_imagem;
        Mat img = imread(caminho_imagem);
        if (img.empty()) {
            cout << "Erro ao abrir: " << caminho_imagem << endl;
            continue;
        }

        cout << "\n=================================================" << endl;
        cout << "Processando [" << (idx_img + 1) << "/" << imagens.size() << "]: " << nome_imagem << endl;

        // Pipeline de processamento
        vector<int> corFundoBGR = estimarCorFundoBGR(img);
        vector<vector<vector<int>>> imagem = CV2vectorvector(img);
        vector<vector<vector<int>>> img_suavizada = suavizarBGR(imagem, METODO_SUAVIZACAO, TAM_SUAVIZACAO);
        vector<vector<int>> img_bin = binarizarBGR(img_suavizada, corFundoBGR, KB, KG, KR);
        vector<vector<int>> imagem_dilatada = dilatacao(img_bin, TAM_EE);
        vector<vector<int>> imagem_fechamento = erosao(imagem_dilatada, TAM_EE);
        vector<vector<int>> imagem_erodida = erosao(imagem_fechamento, TAM_EE_extracao_borda);
        vector<vector<int>> bordas = subtrair(imagem_fechamento, imagem_erodida);

        vector<vector<ponto>> contornos = identificar_folhas(bordas);
        cout << "Contornos detectados: " << contornos.size() << endl;

        Mat mat_com_bordas = img.clone();

        vector<RegistroFolha> registros_imagem;

        for (size_t i = 0; i < contornos.size(); i++) {
            size_t num_pixels = contornos[i].size();
            int y0 = contornos[i].empty() ? -1 : contornos[i][0].y;
            int x0 = contornos[i].empty() ? -1 : contornos[i][0].x;

            // Pre-filtro automatico de ruido (< 500 pixels)
            if (num_pixels < LIMIAR_MIN_PIXELS) {
                registros_imagem.push_back({
                    nome_imagem, i + 1, 0, nomeEspecie(0), num_pixels, y0, x0
                });
                continue;
            }

            // Destaca apenas os pixels do contorno atual em vermelho direto no cv::Mat
            for (const ponto& p : contornos[i]) {
                mat_com_bordas.at<Vec3b>(p.y, p.x) = Vec3b(0, 0, 255);
            }

            imshow("Catalogador - Folha Atual", mat_com_bordas);

            cout << "  Contorno " << (i + 1) << "/" << contornos.size()
                 << " | Pixels: " << num_pixels
                 << " | Digite [1-6] Especie ou [0/R] Ruido: " << flush;

            int id_escolhido = -1;
            while (true) {
                int tecla = waitKey(0);
                if (tecla == 27) { // ESC
                    encerrar = true;
                    break;
                }
                if (tecla >= '0' && tecla <= '6') {
                    id_escolhido = tecla - '0';
                    break;
                }
                if (tecla == 'r' || tecla == 'R') {
                    id_escolhido = 0;
                    break;
                }
            }

            if (encerrar) {
                cout << "\n[ESC] Encerrando sem salvar a imagem incompleta atual..." << endl;
                break;
            }

            cout << id_escolhido << " (" << nomeEspecie(id_escolhido) << ")" << endl;

            registros_imagem.push_back({
                nome_imagem, i + 1, id_escolhido, nomeEspecie(id_escolhido), num_pixels, y0, x0
            });

            // Marca contorno ja processado em branco direto no cv::Mat
            for (const ponto& p : contornos[i]) {
                mat_com_bordas.at<Vec3b>(p.y, p.x) = Vec3b(255, 255, 255);
            }
        }

        if (encerrar) {
            break;
        }

        // Salva progresso da imagem concluida no CSV
        ofstream out(ARQUIVO_CSV, ios::app);
        for (const auto& reg : registros_imagem) {
            out << reg.imagem << ","
                << reg.idx_folha << ","
                << reg.id_especie << ","
                << reg.nome_especie << ","
                << reg.pixels_borda << ","
                << reg.y_inicial << ","
                << reg.x_inicial << "\n";
        }
        out.close();

        cout << "-> Progresso salvo em " << ARQUIVO_CSV << " (" << registros_imagem.size() << " contornos)." << endl;
    }

    destroyAllWindows();
    cout << "\nCatalogacao finalizada." << endl;
    return 0;
}
