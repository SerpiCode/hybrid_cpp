#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include <filesystem>
#include "brkga_mp_ipr/brkga_mp_ipr.hpp"
#include "instance.hpp"
#include "decoder.hpp"

using namespace std;
using namespace std::filesystem;
using namespace BRKGA;

// Leitura de CSV, removendo a última coluna
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
        if (first_line) {
            first_line = false;
            continue; // pula cabeçalho
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

    return data;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0]
             << " <seed> <config-file> <maximum-running-time-seconds>" << endl;
        return 1;
    }

    const unsigned seed = stoi(argv[1]);
    const string config_file = argv[2];
    const unsigned max_time_seconds = stoi(argv[3]);
    const unsigned num_threads = 8;

    ofstream output_file("results.txt");

    for (const auto& entry : recursive_directory_iterator("data-lakes")) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".csv") continue;

        string dataset_name = entry.path().filename().string();
        cout << "Processando: " << dataset_name << endl;

        try {
            // 1. Leitura do CSV
            vector<vector<double>> X = read_csv_data(entry.path().string());
            if (X.empty() || X[0].empty()) throw runtime_error("Arquivo vazio ou mal formatado.");

            // 2. Instância e Decoder
            HybridInstance instance(X);
            HybridDecoder decoder(instance, "sil");

            // 3. Parâmetros BRKGA
            auto [brkga_params, control_params] = readConfiguration(config_file);
            control_params.maximum_running_time = chrono::seconds{max_time_seconds};

            size_t chromosome_size = X[0].size() + 1;

            BRKGA_MP_IPR<HybridDecoder> algorithm(
                decoder, Sense::MINIMIZE, seed,
                chromosome_size, brkga_params, num_threads
            );
            
            // 4. Executar
            auto start = chrono::steady_clock::now();
            const auto result = algorithm.run(control_params);
            auto end = chrono::steady_clock::now();
            chrono::duration<double> duration = end - start;

            // 5. Salvar no arquivo
            output_file << dataset_name << ", ";
            for (auto gene : algorithm.getBestChromosome())
                output_file << gene << " ";
            output_file << ", " << duration.count() << "s" << endl;

        } catch (const exception& e) {
            cerr << "Erro com " << dataset_name << ": " << e.what() << endl;
        }
    }

    output_file.close();
    cout << "Todos os testes foram concluídos. Resultados salvos em results.txt." << endl;
    return 0;
}
