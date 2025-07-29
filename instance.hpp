#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <opencv2/core.hpp>

class HybridInstance
{
public:
    cv::Mat X;
    int kmax;
    float lambda_k;

    HybridInstance() : lambda_k(0.01f), kmax(2) 
    {
        X = cv::Mat();
    }

    // Construtor
    HybridInstance(const cv::Mat& X, float lambda_k = 0.01f)
        : X(X), lambda_k(lambda_k)
    {
        int n_samples = X.rows;
        // Garante que kmax seja pelo menos 2
        kmax = std::max(2, n_samples / 2);
    }
};

#endif // INSTANCE_HPP