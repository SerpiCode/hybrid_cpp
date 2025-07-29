#include "decoder.hpp"
#include "silhouette.hpp"
#include "instance.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>
#include <set>
#include <cmath>

using namespace std;
using namespace cv;


HybridDecoder::HybridDecoder(const HybridInstance& instance)
    {
        this->instance = instance;
        lambda_k = instance.lambda_k;
    }

float HybridDecoder::decoder(const vector<float>& chromosome)
{
    /* Definir a variável X_sel (dataset filtrado) */
    
    // Converter genes em flags
    vector<int> flags;
    for (size_t i = 1; i < chromosome.size(); ++i)
        flags.push_back(chromosome[i] >= 0.5f ? 1 : 0);

    // Obter colunas selecionadas
    vector<int> selectedCols;
    for (size_t i = 0; i < flags.size(); ++i)
        if (flags[i] == 1)
            selectedCols.push_back((int)i);

    // Nenhuma feature selecionada
    if (selectedCols.empty()) return 1e6; // Penalidade alta para cromossomos inválidos

    // Criar X_sel com colunas selecionadas
    Mat X_sel(instance.X.rows, (int)selectedCols.size(), CV_32F);
    for (int col = 0; col < (int)selectedCols.size(); ++col) {
        int idx = selectedCols[col];
        for (int row = 0; row < instance.X.rows; ++row)
            X_sel.at<float>(row, col) = instance.X.at<float>(row, idx);
    }

    /* Encontrar o valor de K real */

    // Calcular número de amostras únicas
    set<vector<float>> uniqueRows;
    for (int i = 0; i < X_sel.rows; ++i) {
        vector<float> row;
        for (int j = 0; j < X_sel.cols; ++j)
            row.push_back(X_sel.at<float>(i, j));
        uniqueRows.insert(row);
    }

    int unique_samples = (int)uniqueRows.size() - 1;
    unique_samples = max(unique_samples, 1);

    float k_real = chromosome[0];
    int kmax = unique_samples / 2;
    int k = (int)(2 + (kmax - 2 + 1) * k_real);
    k = std::min(k, unique_samples); // Evita passar do limite

    if (k <= 1) return 1e6; // Menos de dois grupos

    /* Cálculo do K-Means e fitness (silhouette) */

    // K-Means clustering
    Mat labels, centers;
    TermCriteria criteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 100, 0.01);
    int attempts = 10;

    kmeans(X_sel, k, labels, criteria, attempts, KMEANS_PP_CENTERS, centers);

    set<int> uniqueLabels;
    for (int i = 0; i < labels.rows; ++i)
    {
        uniqueLabels.insert(labels.at<int>(i));
    }

    if (uniqueLabels.size() <= 1) return 1e6; // Menos de dois grupos

    // Silhouette score
    float s_score = silhouetteScore(X_sel, labels, k);

    return (-s_score) + k * lambda_k; // Para problema de minimização
}