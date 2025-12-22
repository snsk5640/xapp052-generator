import argparse
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import sys
import os

def parse_lfsr_file(filepath):
    """
    Parses the LFSR output file to extract metadata and seed values.
    """
    metadata = {}
    seeds = []
    
    try:
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                
                if line.startswith("#"):
                    # Parse header like "# BITS=32"
                    parts = line.lstrip("#").strip().split("=")
                    if len(parts) == 2:
                        key, value = parts[0].strip(), parts[1].strip()
                        # Try to convert to int if possible
                        try:
                            metadata[key] = int(value)
                        except ValueError:
                            metadata[key] = value
                else:
                    # Parse hex values
                    try:
                        seeds.append(int(line, 0))
                    except ValueError:
                        pass
    except FileNotFoundError:
        print(f"Error: File {filepath} not found.")
        sys.exit(1)
        
    return metadata, seeds

def visualize_coverage(metadata, seeds, output_image):
    bits = metadata.get("BITS", 32)
    step = metadata.get("STEP", 0)
    mode = metadata.get("TYPE", "UNKNOWN")
    
    max_val = (1 << bits) - 1
    
    print(f"Visualizing {len(seeds)} segments.")
    print(f"Range: 0 to {max_val} (2^{bits})")
    
    fig, ax = plt.subplots(figsize=(12, 4))
    
    # Setup axis
    ax.set_xlim(0, max_val)
    ax.set_ylim(0, 1)
    ax.set_yticks([])
    ax.set_xlabel(f"LFSR Value Space (0 to $2^{{{bits}}}-1$)")
    ax.set_title(f"LFSR Coverage Map ({mode} Mode)\nBits: {bits}, Step Size: {step}")
    
    # Draw background bar (empty space)
    ax.add_patch(patches.Rectangle((0, 0.3), max_val, 0.4, color="#f0f0f0", label="Unused Space"))

    # Draw segments
    # Note: LFSR values jump around pseudo-randomly, but here we visualize 
    # the "value" coverage if we treat the number as a magnitude.
    # HOWEVER, for Reseed Visualization, usually we care about the "Sequence Overlap".
    # Since we can't easily map "value" to "sequence position" without discrete log,
    # we will visualize the SEEDS as points in the value space.
    
    # If we want to show non-overlapping property, we ideally map to time-domain, 
    # but that requires discrete log calculation which is hard.
    # Instead, let's visualize the distribution of Start Points in the value space.
    
    colors = plt.cm.viridis([i / len(seeds) for i in range(len(seeds))])
    
    for i, seed in enumerate(seeds):
        # Draw a vertical line for the seed start
        ax.axvline(x=seed, color=colors[i], ymin=0.3, ymax=0.7, linewidth=2, alpha=0.8)
        
        # If step is small compared to max_val, it's hard to see the range bar.
        # But we try to draw a small rectangle representing the "consumed" range 
        # IF we assume linear mapping (which is not true for LFSR, but shows magnitude).
        # strictly speaking, LFSR jumps. 
        # So we just plot the SEED LOCATIONS to show they are distributed.
        
    # Create a custom legend
    ax.text(0, 1.05, f"Total Seeds: {len(seeds)}", transform=ax.transAxes)

    plt.tight_layout()
    plt.savefig(output_image)
    print(f"Saved visualization to {output_image}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Visualize LFSR Seed Distribution")
    parser.add_argument("input_file", help="Path to the LFSR output file (txt)")
    parser.add_argument("-o", "--output", default="coverage_map.png", help="Output image file path")
    
    args = parser.parse_args()
    
    meta, seeds = parse_lfsr_file(args.input_file)
    visualize_coverage(meta, seeds, args.output)
