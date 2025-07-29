#ifndef SILHOUETTE_HPP
#define SILHOUETTE_HPP

#include <opencv2/core.hpp>

float silhouetteScore(const cv::Mat& X, const cv::Mat& labels, int k);

#endif