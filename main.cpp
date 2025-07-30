#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include "brkga_mp_ipr/brkga_mp_ipr.hpp"
#include "instance.hpp"
#include "decoder.hpp"

using namespace std;
using namespace BRKGA;

int main(int argc, char* argv[]) {
    if(argc < 4) {
        cerr << "Usage: " << argv[0]
             << " <seed> <config-file> <maximum-running-time-seconds>"
             << endl;
        return 1;
    }

    try {
        // 1. Argumentos
        const unsigned seed = stoi(argv[1]);
        const string config_file = argv[2];
        const unsigned max_time_seconds = stoi(argv[3]);
        const unsigned num_threads = 4;

        // 2. Dados de exemplo (substitua pela leitura real)
        vector<vector<double>> X = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12},
            {13, 14, 15, 16}
        };

        // 3. Instância e Decoder
        HybridInstance instance(X);
        HybridDecoder decoder(instance);

        // 4. Leitura da configuração do BRKGA (implemente a função readConfiguration)
        auto [brkga_params, control_params] = readConfiguration(config_file);
        control_params.maximum_running_time = chrono::seconds{max_time_seconds};

        // 5. Inicializa o BRKGA
        const size_t chromosome_size = X[0].size() + 1; // 1 gene para k, resto para features

        BRKGA_MP_IPR<HybridDecoder> algorithm(
            decoder, Sense::MINIMIZE, seed,
            chromosome_size,
            brkga_params, num_threads
        );

        // 6. Executa o algoritmo (ajuste control_params conforme seu uso)
        cout << "Executando BRKGA..." << endl;
        const auto final_status = algorithm.run(control_params, &cout);

        // 7. Exibe resultado
        cout << "Algorithm status: " << final_status << endl;
        cout << "Best cost: " << final_status.best_fitness << endl;
    }
    catch (const exception& e) {
        cerr << "\nErro: " << e.what() << endl;
        return 1;
    }

    return 0;
}