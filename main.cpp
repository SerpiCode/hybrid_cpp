#include <iostream>
#include <opencv2/core.hpp>
#include "decoder.hpp"
#include "instance.hpp"

using namespace std;
using namespace cv;

int main()
{
    // Cromossomo de exemplo
    vector<float> chromosome = {0.7f, 0.1f, 0.8f, 0.4f, 0.9f}; // 5 genes

    // Dados de exemplo: 4 amostras x 4 features
    Mat X = (Mat_<float>(4,4) << 
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16);

    HybridInstance instance(X, 0.01f);
    HybridDecoder decoder(instance);

    float result = decoder.decoder(chromosome);

    cout << "Silhouette invertido: " << result << endl;

    return 0;
}