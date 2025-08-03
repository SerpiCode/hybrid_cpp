#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include "brkga_mp_ipr/brkga_mp_ipr.hpp"
#include "instance.hpp"
#include "decoder.hpp"

using namespace std;
using namespace BRKGA;

vector<vector<double>> read_csv_data(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Não foi possível abrir o arquivo: " + filename);
    }

    vector<vector<double>> data;
    string line;

    bool first_line = true;
    while (getline(file, line)) {
        if (line.empty()) continue;

        // Ignora o cabeçalho
        if (first_line) {
            first_line = false;
            continue;
        }

        stringstream ss(line);
        string cell;
        vector<double> row;
        while (getline(ss, cell, ',')) {
            try {
                row.push_back(stod(cell));
            } catch (...) {
                throw runtime_error("Valor inválido no CSV: \"" + cell + "\"");
            }
        }

        if (!row.empty()) {
            row.pop_back();  // Remove a última coluna (label)
            data.push_back(row);
        }
    }

    file.close();
    return data;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
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

        // 2. Lê dados do arquivo CSV (removendo a última coluna)
        vector<vector<double>> X = read_csv_data("tab-12.csv");

        if (X.empty() || X[0].empty()) {
            throw runtime_error("Arquivo CSV vazio ou mal formatado.");
        }

        // 3. Instância e Decoder
        HybridInstance instance(X);
        HybridDecoder decoder(instance);

        // 4. Leitura da configuração do BRKGA
        auto [brkga_params, control_params] = readConfiguration(config_file);
        control_params.maximum_running_time = chrono::seconds{max_time_seconds};

        // 5. Inicializa o BRKGA
        const size_t chromosome_size = X[0].size() + 1; // 1 gene para k, resto para features

        BRKGA_MP_IPR<HybridDecoder> algorithm(
            decoder, Sense::MINIMIZE, seed,
            chromosome_size,
            brkga_params, num_threads
        );

        // 6. Executa
        cout << "Executando BRKGA..." << endl;
        const auto final_status = algorithm.run(control_params, &cout);

        // 7. Resultado
        cout << "Algorithm status: " << final_status << endl;
        cout << "Best cost: " << final_status.best_fitness << endl;
    }
    catch (const exception& e) {
        cerr << "\nErro: " << e.what() << endl;
        return 1;
    }

    return 0;
}
