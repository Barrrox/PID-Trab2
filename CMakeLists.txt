cmake_minimum_required(VERSION 3.10)
project(ContadorFolhas)

# Força o CMake a buscar bibliotecas estáticas (.lib em vez de .dll)
set(OpenCV_STATIC ON)
set(OpenCV_DIR "C:/Users/mathe/opencv/build")

# Ativa o padrão C++17 obrigatoriamente
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Se estiver usando o Visual Studio (MSVC), garante que o próprio runtime do C++ seja estático (/MT)
# Isso evita que o usuário final precise instalar o "Visual C++ Redistributable"
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Encontra o pacote opencv
find_package(OpenCV REQUIRED)
  
# Nota: include_directories() global foi removido. É melhor prática usar target_include_directories por alvo,
# mas como o find_package da OpenCV já faz o gerenciamento moderno de targets, o target_link_libraries
# abaixo já herda os includes automaticamente de ${OpenCV_LIBS}!

# Cria executáveis
add_executable(ContadorFolhas src/main.cpp src/funcoes.cpp)
add_executable(ContadorFolhasTudo src/main2.cpp src/funcoes.cpp)
add_executable(Catalogador src/catalogar.cpp src/funcoes.cpp)

# Linka com as bibliotecas do openCV (usando PRIVATE para boas práticas)
target_link_libraries(ContadorFolhas PRIVATE ${OpenCV_LIBS})
target_link_libraries(ContadorFolhasTudo PRIVATE ${OpenCV_LIBS})
target_link_libraries(Catalogador PRIVATE ${OpenCV_LIBS})
