#ifndef DECODER_HPP
#define DECODER_HPP

#include <vector>
#include <opencv2/core.hpp>
#include "instance.hpp"

class HybridDecoder {
public:
    HybridInstance instance;
    float lambda_k;

    HybridDecoder(const HybridInstance& instance);

    float decoder(const std::vector<float>& chromosome);
};

#endif