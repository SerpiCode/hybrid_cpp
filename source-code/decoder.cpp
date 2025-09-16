#include "../headers/decoder.hpp"
#include "../headers/silhouette.hpp"
#include "../headers/davies_bouldin.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <limits>
#include <random>
#include <algorithm>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace BRKGA;

HybridDecoder::HybridDecoder(const HybridInstance& instance, const std::string& metric)
    : instance(instance), lambda_k(instance.lambda_k), metric(metric)
{}

// K-means com OpenMP
static vector<int> kmeans(const vector<vector<double>>& data, int k, int max_iter = 100) {
    int n = (int)data.size();
    int dim = (int)(n > 0 ? data[0].size() : 0);
    if (n == 0 || dim == 0 || k <= 0) return {};

    vector<vector<double>> centers(k, vector<double>(dim));
    for (int i = 0; i < k; ++i)
        centers[i] = data[i % n];

    vector<int> labels(n, -1);
    bool changed = true;
    int iter = 0;

    while (changed && iter < max_iter) {
        changed = false;

        #pragma omp parallel for schedule(static)
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

        vector<int> counts(k, 0);
        centers.assign(k, vector<double>(dim, 0.0));

        #pragma omp parallel for schedule(static)
        for (int i = 0; i < n; ++i) {
            int c = labels[i];
            #pragma omp atomic
            counts[c]++;
            for (int d = 0; d < dim; ++d) {
                #pragma omp atomic
                centers[c][d] += data[i][d];
            }
        }

        for (int c = 0; c < k; ++c) {
            if (counts[c] > 0)
                for (int d = 0; d < dim; ++d)
                    centers[c][d] /= counts[c];
            else
                centers[c] = data[rand() % n];
        }

        iter++;
    }
    return labels;
}

// MiniBatch KMeans com OpenMP
static vector<int> minibatch_kmeans(const vector<vector<double>>& data, int k, int batch_size = 100, int max_iter = 100) {
    int n = (int)data.size();
    int dim = (int)(n > 0 ? data[0].size() : 0);
    if (n == 0 || dim == 0 || k <= 0) return {};

    vector<vector<double>> centers(k, vector<double>(dim));
    for (int i = 0; i < k; ++i)
        centers[i] = data[i % n];

    vector<int> labels(n, -1);
    vector<int> counts(k, 0);

    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, n - 1);

    for (int iter = 0; iter < max_iter; ++iter) {
        vector<int> batch_indices(batch_size);
        for (int i = 0; i < batch_size; ++i)
            batch_indices[i] = dist(rng);

        #pragma omp parallel for schedule(static)
        for (int b = 0; b < batch_size; ++b) {
            int idx = batch_indices[b];
            double best_dist = numeric_limits<double>::max();
            int best_c = -1;
            for (int c = 0; c < k; ++c) {
                double dist_sq = 0.0;
                for (int d = 0; d < dim; ++d) {
                    double diff = data[idx][d] - centers[c][d];
                    dist_sq += diff * diff;
                }
                if (dist_sq < best_dist) {
                    best_dist = dist_sq;
                    best_c = c;
                }
            }

            #pragma omp critical
            {
                counts[best_c]++;
                double eta = 1.0 / counts[best_c];
                for (int d = 0; d < dim; ++d)
                    centers[best_c][d] = (1 - eta) * centers[best_c][d] + eta * data[idx][d];
                labels[idx] = best_c;
            }
        }
    }

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; ++i) {
        double best_dist = numeric_limits<double>::max();
        int best_c = -1;
        for (int c = 0; c < k; ++c) {
            double dist_sq = 0.0;
            for (int d = 0; d < dim; ++d) {
                double diff = data[i][d] - centers[c][d];
                dist_sq += diff * diff;
            }
            if (dist_sq < best_dist) {
                best_dist = dist_sq;
                best_c = c;
            }
        }
        labels[i] = best_c;
    }

    return labels;
}

BRKGA::fitness_t HybridDecoder::decode(Chromosome& chromosome, bool) {
    vector<int> flags;
    for (size_t i = 1; i < chromosome.size(); ++i)
        flags.push_back(chromosome[i] >= 0.5f ? 1 : 0);

    vector<int> selectedCols;
    for (size_t i = 0; i < flags.size(); ++i)
        if (flags[i] == 1)
            selectedCols.push_back((int)i);

    if (selectedCols.empty()) return 1e6;

    vector<vector<double>> X_sel(instance.X.size(), vector<double>(selectedCols.size()));
    for (size_t row = 0; row < instance.X.size(); ++row)
        for (size_t col = 0; col < selectedCols.size(); ++col)
            X_sel[row][col] = instance.X[row][selectedCols[col]];

    set<vector<double>> uniqueRows(X_sel.begin(), X_sel.end());
    int unique_samples = (int)uniqueRows.size() - 1;
    unique_samples = max(unique_samples, 1);

    double k_real = chromosome[0];
    int kmax = unique_samples / 2;
    int k = (int)(2 + (kmax - 2 + 1) * k_real);
    k = min(k, unique_samples);

    if (k <= 1) return 1e6;

    vector<int> labels;
    auto start = chrono::steady_clock::now();

    if (X_sel.size() > 1000) {
        labels = minibatch_kmeans(X_sel, k);
    } else {
        labels = kmeans(X_sel, k);
    }

    auto end = chrono::steady_clock::now();
    chrono::duration<double> duration = end - start;
    // cout << "Tempo K-Means: " << duration.count() << " s" << endl;

    set<int> uniqueLabels(labels.begin(), labels.end());
    if (uniqueLabels.size() <= 1) return 1e6;

    double score = 0.0;

    if (metric == "sil") {
        score = silhouetteScore(X_sel, labels, k);
        return (-score) + k * lambda_k;
    }
    else if (metric == "db") {
        auto start_metric = chrono::steady_clock::now();
        score = daviesBouldinIndex(X_sel, labels, k);
        auto end_metric = chrono::steady_clock::now();
        chrono::duration<double> duration_metric = end_metric - start_metric;
        return score + k * lambda_k;
    }
    else {
        cerr << "Métrica desconhecida: " << metric << ". Usando silhouette." << endl;
        score = silhouetteScore(X_sel, labels, k);
        return (-score) + k * lambda_k;
    }
}
