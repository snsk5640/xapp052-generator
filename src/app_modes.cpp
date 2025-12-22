#include "app_modes.h"
#include <iostream>
#include <fstream>
#include <iomanip>

int runGenerateMode(const LFSRConfig& cfg, uint64_t seed, int count, const std::string& output_file) {
    std::ofstream outfile(output_file);
    if (!outfile) {
        std::cerr << "Error: Cannot open output file " << output_file << std::endl;
        return 1;
    }

    // Write Header
    outfile << "# TYPE=GENERATE" << std::endl;
    outfile << "# BITS=" << cfg.bits << std::endl;
    outfile << "# COUNT=" << count << std::endl;

    uint64_t state = seed;
    uint64_t mask = (1ULL << cfg.bits) - 1;

    std::cout << "Mode: GENERATE (" << count << " numbers)" << std::endl;

    for (int i = 0; i < count; ++i) {
        outfile << "0x" << std::hex << std::setw((cfg.bits + 3) / 4) << std::setfill('0') << state << std::endl;
        state = stepLFSR(state, cfg, mask);
    }
    
    std::cout << "Done. Output saved to " << output_file << std::endl;
    return 0;
}

int runReseedMode(const LFSRConfig& cfg, uint64_t seed, int count, int step_size, const std::string& output_file) {
    if (step_size <= 0) {
        std::cerr << "Error: --step (-k) must be specified > 0 for reseed mode." << std::endl;
        return 1;
    }

    std::ofstream outfile(output_file);
    if (!outfile) {
        std::cerr << "Error: Cannot open output file " << output_file << std::endl;
        return 1;
    }

    // Write Header (Important for Visualization)
    outfile << "# TYPE=RESEED" << std::endl;
    outfile << "# BITS=" << cfg.bits << std::endl;
    outfile << "# COUNT=" << count << std::endl;
    outfile << "# STEP=" << step_size << std::endl;

    uint64_t state = seed;
    uint64_t mask = (1ULL << cfg.bits) - 1;

    std::cout << "Mode: RESEED" << std::endl;
    std::cout << " Generating " << count << " seeds." << std::endl;
    std::cout << " Each seed is separated by " << step_size << " steps." << std::endl;

    for (int i = 0; i < count; ++i) {
        // Output the current state as a new start seed
        outfile << "0x" << std::hex << std::setw((cfg.bits + 3) / 4) << std::setfill('0') << state << std::endl;
        
        // Advance the LFSR by 'step_size'
        for (int j = 0; j < step_size; ++j) {
            state = stepLFSR(state, cfg, mask);
        }
    }

    std::cout << "Done. Output saved to " << output_file << std::endl;
    return 0;
}
