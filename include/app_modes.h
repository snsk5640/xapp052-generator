#ifndef APP_MODES_H
#define APP_MODES_H

#include "lfsr_core.h"
#include <string>

/**
 * Execute the standard random number generation mode.
 */
int runGenerateMode(const LFSRConfig& cfg, uint64_t seed, int count, const std::string& output_file);

/**
 * Execute the reseed mode.
 * Generates non-overlapping seeds separated by 'step_size'.
 */
int runReseedMode(const LFSRConfig& cfg, uint64_t seed, int count, int step_size, const std::string& output_file);

#endif // APP_MODES_H
