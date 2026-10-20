#ifndef FUNCOES_H
#define FUNCOES_H

#include <vector>
#include <cstddef>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;


int estimarCorFundo(cv::Mat& imagem);

std::vector<std::vector<int>> binarizar(const cv::Mat& imagem);
/* Binariza a imagem, transformando para gray scale e limiarizando

*/

#endif // FUNCOES_H