# Projeto
cmake_minimum_required(VERSION 3.10)
project(ContadorFolhas)

set(OpenCV_DIR C:/Users/mathe/opencv/build)

# Ativa o padrão C++17 obrigatoriamente para puxar :
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontra o pacote opencv
find_package(OpenCV REQUIRED)
  
# Inclui diretorios do OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Cria executavel
add_executable(ContadorFolhas src/main.cpp src/funcoes.cpp)
add_executable(ContadorFolhasTudo src/main2.cpp src/funcoes.cpp)
add_executable(Catalogador src/catalogar.cpp src/funcoes.cpp)

# Linka com as bibliotecas do openCV
target_link_libraries(ContadorFolhas ${OpenCV_LIBS})
target_link_libraries(ContadorFolhasTudo ${OpenCV_LIBS})
target_link_libraries(Catalogador ${OpenCV_LIBS})
