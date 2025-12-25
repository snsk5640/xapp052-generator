#include "app_modes.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>

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
    // Handle 64-bit mask correctly to avoid undefined behavior with 1ULL << 64
    uint64_t mask = (cfg.bits == 64) ? ~0ULL : (1ULL << cfg.bits) - 1;

    std::cout << "Mode: GENERATE (" << count << " numbers)" << std::endl;

    for (int i = 0; i < count; ++i) {
        outfile << "0x" << std::hex << std::setw((cfg.bits + 3) / 4) << std::setfill('0') << state << std::endl;
        state = stepLFSR(state, cfg, mask);
    }
    
    std::cout << "Done. Output saved to " << output_file << std::endl;
    return 0;
}

int runReseedMode(const LFSRConfig& cfg, uint64_t seed, int count, int step_size, const std::string& output_file) {
    
    uint64_t actual_step = step_size;

    // Auto-calculation logic if step_size is 0
    if (actual_step == 0) {
        if (count <= 0) {
            std::cerr << "Error: Count must be > 0." << std::endl;
            return 1;
        }

        uint64_t total_period;
        if (cfg.bits == 64) {
            total_period = std::numeric_limits<uint64_t>::max(); // 2^64 - 1
        } else {
            total_period = (1ULL << cfg.bits) - 1;
        }

        actual_step = total_period / count;

        if (actual_step == 0) {
            std::cerr << "Error: Count is too large for the period range. Cannot distribute." << std::endl;
            return 1;
        }
        
        std::cout << "Auto-calculated step size: " << actual_step << std::endl;
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
    outfile << "# STEP=" << actual_step << std::endl;

    uint64_t state = seed;
    // Handle 64-bit mask correctly
    uint64_t mask = (cfg.bits == 64) ? ~0ULL : (1ULL << cfg.bits) - 1;

    std::cout << "Mode: RESEED" << std::endl;
    std::cout << " Generating " << count << " seeds." << std::endl;
    std::cout << " Each seed is separated by " << actual_step << " steps." << std::endl;

for (int i = 0; i < count; ++i) {
        // Output the current state as a new start seed
        outfile << "0x" << std::hex << std::setw((cfg.bits + 3) / 4) << std::setfill('0') << state << std::endl;
        
        std::cout << "Calculating seed " << (i + 1) << "/" << (count) << " ... " << std::flush;

        // Advance the LFSR by 'actual_step'
        for (uint64_t j = 0; j < actual_step; ++j) {
            state = stepLFSR(state, cfg, mask);

            if (actual_step > 100000 && j % (actual_step / 10) == 0) {
                std::cout << "." << std::flush;
            }
        }
        std::cout << " Done!" << std::endl;
    }

    std::cout << "Done. Output saved to " << output_file << std::endl;
    return 0;
}