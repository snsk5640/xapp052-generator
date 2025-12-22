#include "lfsr_core.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

bool loadConfig(int target_bits, LFSRConfig& config) {
    std::ifstream file("data/taps.csv");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open data/taps.csv" << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip CSV header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string b_str, taps_str;

        std::getline(ss, b_str, ',');
        try {
            if (std::stoi(b_str) == target_bits) {
                config.bits = target_bits;
                
                // Extract taps part
                std::getline(ss, taps_str);
                // Remove quotes
                taps_str.erase(std::remove(taps_str.begin(), taps_str.end(), '\"'), taps_str.end());
                
                std::stringstream ss_taps(taps_str);
                std::string tap;
                while (std::getline(ss_taps, tap, ',')) {
                    config.taps.push_back(std::stoi(tap));
                }
                return true;
            }
        } catch (...) {
            continue; // Skip malformed lines
        }
    }
    return false;
}

int calculateXNOR(uint64_t state, const LFSRConfig& cfg) {
    int count = 0;
    for (int tap : cfg.taps) {
        // According to XAPP 052, bits are labeled 1 to n.
        // We map Qn to the (tap-1) bit position in our state variable.
        if ((state >> (tap - 1)) & 1) {
            count++;
        }
    }
    // XNOR feedback returns 1 if the number of set bits is even.
    return (count % 2 == 0) ? 1 : 0;
}

uint64_t stepLFSR(uint64_t state, const LFSRConfig& cfg, uint64_t mask) {
    int fb = calculateXNOR(state, cfg);
    // Shift right and insert feedback bit into the most significant bit (Q1 position)
    return ((state >> 1) | (static_cast<uint64_t>(fb) << (cfg.bits - 1))) & mask;
}
