#include <iostream>
#include <opencv2/core.hpp>

using namespace std;

class HybridInstance
{
    public:
        Mat X;
        int kmax;
        float lambda_k;
    
    HybridInstance(Mat X, float lambda_k=0.01f)
    {
        this->X = X;
        int n_samples = X.rows;
        kmax = max(2, n_samples / 2);
        this->lambda_k = lambda_k;
    }
};