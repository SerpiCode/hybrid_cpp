#ifndef DECODER_HPP
#define DECODER_HPP

#include <vector>
#include "instance.hpp"
#include "brkga_mp_ipr/fitness_type.hpp"
#include "brkga_mp_ipr/chromosome.hpp"

class HybridDecoder {
public:
    HybridInstance instance;
    double lambda_k;

    HybridDecoder(const HybridInstance& instance);

    BRKGA::fitness_t decode(BRKGA::Chromosome& chromosome, bool rewrite);
};

#endif