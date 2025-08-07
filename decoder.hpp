#ifndef DECODER_HPP
#define DECODER_HPP

#include <vector>
#include <string>
#include "instance.hpp"
#include "brkga_mp_ipr/fitness_type.hpp"
#include "brkga_mp_ipr/chromosome.hpp"

class HybridDecoder {
public:
    HybridInstance instance;
    double lambda_k;
    std::string metric; // "sil" ou "db"

    HybridDecoder(const HybridInstance& instance, const std::string& metric = "sil");

    BRKGA::fitness_t decode(BRKGA::Chromosome& chromosome, bool rewrite);
};

#endif
