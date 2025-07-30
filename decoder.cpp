#include "decoder.hpp"
#include "silhouette.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <limits>
#include <random>
#include <algorithm>

using namespace std;
using namespace BRKGA;

HybridDecoder::HybridDecoder(const HybridInstance& instance)
    : instance(instance), lambda_k(instance.lambda_k)
{}

// Função simples de k-means em vetor nativo (retorna labels)
static vector<int> kmeans(const vector<vector<double>>& data, int k, int max_iter = 100) {
    int n = (int)data.size();
    int dim = (int)(n > 0 ? data[0].size() : 0);
    if (n == 0 || dim == 0 || k <= 0) return {};

    // Inicialização dos centróides: usar primeiros k pontos (ou aleatório)
    vector<vector<double>> centers(k, vector<double>(dim));
    for (int i = 0; i < k; ++i)
        centers[i] = data[i % n];

    vector<int> labels(n, -1);
    vector<int> counts(k, 0);
    bool changed = true;
    int iter = 0;

    while (changed && iter < max_iter) {
        changed = false;
        // passo 1: atribuir pontos ao centroide mais próximo
        for (int i = 0; i < n; ++i) {
            double best_dist = numeric_limits<double>::max();
            int best_c = -1;
            for (int c = 0; c < k; ++c) {
                double dist = 0.0;
                for (int d = 0; d < dim; ++d) {
                    double diff = data[i][d] - centers[c][d];
                    dist += diff * diff;
                }
                if (dist < best_dist) {
                    best_dist = dist;
                    best_c = c;
                }
            }
            if (labels[i] != best_c) {
                labels[i] = best_c;
                changed = true;
            }
        }

        // passo 2: recalcular centróides
        centers.assign(k, vector<double>(dim, 0.0));
        counts.assign(k, 0);

        for (int i = 0; i < n; ++i) {
            int c = labels[i];
            counts[c]++;
            for (int d = 0; d < dim; ++d)
                centers[c][d] += data[i][d];
        }

        for (int c = 0; c < k; ++c) {
            if (counts[c] > 0)
                for (int d = 0; d < dim; ++d)
                    centers[c][d] /= counts[c];
            else
                centers[c] = data[rand() % n];  // reinitialize empty cluster
        }

        iter++;
    }

    return labels;
}

BRKGA::fitness_t HybridDecoder::decode(Chromosome& chromosome, bool)
{
    // Converter genes em flags para features selecionadas
    vector<int> flags;
    for (size_t i = 1; i < chromosome.size(); ++i)
        flags.push_back(chromosome[i] >= 0.5f ? 1 : 0);

    // Obter índices das colunas selecionadas
    vector<int> selectedCols;
    for (size_t i = 0; i < flags.size(); ++i)
        if (flags[i] == 1)
            selectedCols.push_back((int)i);

    if (selectedCols.empty()) return 1e6;  // penalidade

    // Criar dataset filtrado X_sel
    vector<vector<double>> X_sel(instance.X.size(), vector<double>(selectedCols.size()));
    for (size_t row = 0; row < instance.X.size(); ++row) {
        for (size_t col = 0; col < selectedCols.size(); ++col) {
            X_sel[row][col] = instance.X[row][selectedCols[col]];
        }
    }

    // Calcular número de amostras únicas
    set<vector<double>> uniqueRows(X_sel.begin(), X_sel.end());
    int unique_samples = (int)uniqueRows.size() - 1;
    unique_samples = max(unique_samples, 1);

    double k_real = chromosome[0];
    int kmax = unique_samples / 2;
    int k = (int)(2 + (kmax - 2 + 1) * k_real);
    k = min(k, unique_samples);

    if (k <= 1) return 1e6;

    // Rodar kmeans simples
    vector<int> labels = kmeans(X_sel, k);

    // Verificar número de clusters únicos
    set<int> uniqueLabels(labels.begin(), labels.end());
    if (uniqueLabels.size() <= 1) return 1e6;

    // Calcular silhouette score com nova função (sem OpenCV)
    double s_score = silhouetteScore(X_sel, labels, k);

    return (-s_score) + k * lambda_k;
}