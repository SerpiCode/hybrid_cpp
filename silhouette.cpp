#include "silhouette.hpp"
#include <vector>
#include <cmath>
#include <cfloat>

using namespace cv;
using namespace std;

float euclideanDistance(const Mat& a, const Mat& b) {
    CV_Assert(a.cols == b.cols);
    float dist = 0.0;
    for (int i = 0; i < a.cols; ++i) {
        float diff = a.at<float>(0, i) - b.at<float>(0, i);
        dist += diff * diff;
    }
    return sqrt(dist);
}

float silhouetteScore(const Mat& X, const Mat& labels, int k) {
    int n_samples = X.rows;
    vector<vector<int>> clusters(k);
    for (int i = 0; i < n_samples; ++i)
        clusters[labels.at<int>(i, 0)].push_back(i);

    float totalScore = 0.0;

    for (int i = 0; i < n_samples; ++i) {
        int label_i = labels.at<int>(i, 0);
        Mat xi = X.row(i);

        float a = 0.0f;
        if (clusters[label_i].size() > 1) {
            for (int j : clusters[label_i]) {
                if (i != j)
                    a += euclideanDistance(xi, X.row(j));
            }
            a /= (clusters[label_i].size() - 1);
        }

        float b = FLT_MAX;
        for (int l = 0; l < k; ++l) {
            if (l == label_i || clusters[l].empty()) continue;
            float dist = 0.0f;
            for (int j : clusters[l]) {
                dist += euclideanDistance(xi, X.row(j));
            }
            dist /= clusters[l].size();
            b = min(b, dist);
        }

        float s = 0.0f;
        if (a < b)
            s = 1.0f - (a / b);
        else if (a > b)
            s = (b / a) - 1.0f;

        totalScore += s;
    }

    return totalScore / n_samples;
}