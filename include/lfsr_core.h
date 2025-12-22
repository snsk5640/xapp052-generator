#ifndef LFSR_CORE_H
#define LFSR_CORE_H

#include <vector>
#include <string>
#include <cstdint>

/**
 * Structure to hold LFSR configuration based on XAPP 052.
 */
struct LFSRConfig {
    int bits;
    std::vector<int> taps; // Store tap positions (1 to n)
};

/**
 * Load tap configuration for a specific bit length from CSV.
 * @param target_bits The number of bits for the LFSR.
 * @param config Output structure to store taps.
 * @return true if found, false otherwise.
 */
bool loadConfig(int target_bits, LFSRConfig& config);

/**
 * Calculate XNOR feedback logic as described in XAPP 052.
 * XNOR acts as an even-parity circuit.
 */
int calculateXNOR(uint64_t state, const LFSRConfig& cfg);

/**
 * Advance the LFSR by one step.
 * Shifts right and inserts the feedback bit at the MSB.
 */
uint64_t stepLFSR(uint64_t state, const LFSRConfig& cfg, uint64_t mask);

#endif // LFSR_CORE_H
