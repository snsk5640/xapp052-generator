#include <iostream>
#include <string>
#include <getopt.h>
#include "lfsr_core.h"
#include "app_modes.h"

int main(int argc, char* argv[]) {
    int bits = 0;
    uint64_t seed = 0;
    int count = 0;
    int step_size = 0;
    std::string output_file = "output.txt";
    std::string mode = "generate"; // Default mode

    struct option long_options[] = {
        {"bits", required_argument, 0, 'b'},
        {"seed", required_argument, 0, 's'},
        {"count", required_argument, 0, 'n'},
        {"output", required_argument, 0, 'o'},
        {"mode", required_argument, 0, 'm'},
        {"step", required_argument, 0, 'k'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "b:s:n:o:m:k:", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'b': bits = std::stoi(optarg); break;
            case 's': seed = std::stoull(optarg, nullptr, 0); break;
            case 'n': count = std::stoi(optarg); break;
            case 'o': output_file = optarg; break;
            case 'm': mode = optarg; break;
            case 'k': step_size = std::stoi(optarg); break;
        }
    }

    // Basic Validation
    if (bits == 0 || count == 0) {
        std::cout << "Usage: ./lfsr-generator -b <bits> -s <seed> -n <count> [-m generate|reseed] [-k <step>]" << std::endl;
        return 1;
    }

    if (bits > 64) {
        std::cerr << "Error: This program currently supports up to 64 bits." << std::endl;
        std::cerr << "       (Requested: " << bits << " bits)" << std::endl;
        return 1;
    }

    // Load Configuration
    LFSRConfig cfg;
    if (!loadConfig(bits, cfg)) {
        std::cerr << "Error: No tap configuration found for " << bits << " bits." << std::endl;
        return 1;
    }

    // Mode Dispatch
    if (mode == "generate") {
        return runGenerateMode(cfg, seed, count, output_file);
    } else if (mode == "reseed") {
        return runReseedMode(cfg, seed, count, step_size, output_file);
    } else {
        std::cerr << "Error: Unknown mode '" << mode << "'" << std::endl;
        return 1;
    }
}
