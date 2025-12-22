# LFSR Generator & Visualizer

A configurable **Linear Feedback Shift Register (LFSR)** generator tool based on **Xilinx XAPP 052** specifications.
In addition to standard random number generation, it features a **Reseed Mode** for calculating non-overlapping seeds for parallel simulations, and a Python-based **visualization tool** to verify seed distribution.

## Features

* **XAPP 052 Compliant**: Implements XNOR feedback logic to guarantee Maximum Length Sequences.
* **High Performance**: Lightweight implementation using C++17.
* **Reseed Mode**: Calculates a sequence of seeds separated by a user-defined step size (e.g., 100 million steps) to ensure non-overlapping random number streams.
* **Visualization**: Includes a Python script to visualize how the generated seeds are distributed across the value space.
* **Flexible Configuration**: Supports arbitrary bit lengths (currently implemented up to 64-bit) via `data/taps.csv`.

## Requirements

* **C++ Build Environment**:
    * CMake (3.10 or higher)
    * Ninja (or Make)
    * C++17 compatible compiler (GCC, Clang, etc.)
* **Visualization Tool (Optional)**:
    * Python 3.x
    * Matplotlib (`pip install matplotlib` or `sudo apt install python3-matplotlib`)

## Build

```bash
mkdir build
cd build
cmake -G "Ninja" ..
ninja
```

## Usage

### Generate Mode (Standard Random Numbers)

Generates random numbers starting from a specific seed and saves them to a file.

#### Example: Generate 10 numbers using a 16-bit LFSR with seed 0x1234

```bash
./lfsr-generator -m generate -b 16 -s 0x1234 -n 10 -o output.txt
```

### Reseed Mode (Non-Overlapping Seeds)

Calculates multiple seeds starting from an initial seed, where each new seed is separated by a specific step size (--step). This is useful for initializing parallel workers.

#### Example: Generate 5 seeds for 32-bit LFSR, each separated by 1,000,000 steps

```bash
./lfsr-generator -m reseed -b 32 -s 0x12345678 -n 5 -k 1000000 -o seeds.txt
```
Using these seeds guarantees that the random number sequences will not overlap for at least 1 million steps.

## Visualization

Visualizes the distribution of the seeds generated in Reseed Mode.

```bash
python3 ../scripts/visualize.py seeds.txt -o distribution.png
```

## Project Structure

```text
.
├── data/       # Tap configuration CSV file
├── include/    # Header files
├── scripts/    # Python visualization script
└── src/        # C++ source code (Main logic and CLI)
```

## License

[MIT](LICENSE) © [@snsk5640](https://github.com/snsk5640)
