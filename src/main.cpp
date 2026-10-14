#include <iostream> 
#include <filesystem>
// #include <funcoes.h>
// #include <interface.h>
using namespace std;
// using namespace cv;
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


struct RGB {
    int r = 0;
    int g = 0;
    int b = 0;
};

struct ImagemRGB {
    int largura = 0;
    int altura = 0;

    std::vector<unsigned char> RGB;
};

int main() {

    // Pega o path  pra pasta de imagens
    fs::path pasta = fs::path("imagens");

    // Source - https://stackoverflow.com/a/354922
    // Posted by ScottF, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-10-14, License - CC BY-SA 4.0

    // mudar essa condição do while
    // Deixa o usuário escolher qual imagem processar agora
    while(true) {
        ImagemRGB imagem = (NULL, "open", pasta, NULL, NULL, SW_SHOWDEFAULT);
        // exibe em tela a imagem original
    
        copia_erosao = Imagem;

        // faz negativa
        // faz erosao

        // imagem = imagem - copia_erosao;
        // exibe em tela a imagem processada
        
        cout << "Aperte ENTER para exibir os contornos e contagem de pixels da imagem processada" << endl;

        // começa varredura de [0, largura) e [0, altura) da imagem processada
            // encontrou contorno
                // conta pixels do contorno
                // exibe contorno e quantidade de pixels encontrados

        cout << "Aperte SPACE para selecionar a próxima imagem ou ESC para encerrar o programa" << endl;

        int tecla = waitKey(0);

        if (tecla == 27) {
            break; // ESC pressed
        } else if (tecla == 32) {
            continue; // SPACE pressed, go to next iterarion
        }
    
    }

   
 
    cv::destroyAllWindows();
    return 0;
}
    