cmake_minimum_required(VERSION 3.10)
project(tests VERSION 0.1.0)

# Caminho para o OpenCV
set(OpenCV_DIR "C:/opencv/build")

# Encontrar OpenCV
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")

# Adicionar todos os arquivos fonte ao executável
add_executable(main
    main.cpp
    decoder.cpp
    silhouette.cpp
)

# Incluir diretórios do OpenCV
target_include_directories(main PRIVATE ${OpenCV_INCLUDE_DIRS})

# Linkar as bibliotecas do OpenCV
target_link_libraries(main PRIVATE ${OpenCV_LIBS})