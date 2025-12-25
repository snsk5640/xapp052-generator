import argparse
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import sys
import os

# --------------------------------------------------
# File Parsing Function
# --------------------------------------------------
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

# --------------------------------------------------
# Visualization Function (Timeline View)
# --------------------------------------------------
def visualize_coverage(metadata, seeds, output_image):
    """
    Visualizes the seeds as segments along the LFSR sequence timeline.
    This assumes the file was generated in 'RESEED' mode where seeds
    are separated by a fixed 'STEP'.
    """
    bits = metadata.get("BITS", 32)
    step = metadata.get("STEP", 0)
    mode = metadata.get("TYPE", "UNKNOWN")
    count = len(seeds)

    # This visualization requires RESEED mode
    if mode != "RESEED" or step == 0 or count == 0:
         print("-" * 40)
         print("Error: This visualization method requires a file generated in 'RESEED' mode")
         print("       with a valid 'STEP' size defined in the header.")
         print(f"Current metadata: TYPE={mode}, STEP={step}, Count={count}")
         print("-" * 40)
         sys.exit(1)

    # Python handles arbitrary-precision integers, so >64 bits is fine
    # Calculate total period (2^BITS - 1)
    max_period = (1 << bits) - 1
    
    print(f"Visualizing {count} seed segments in RESEED mode.")
    print(f" - Total Period Space: {max_period} steps (2^{bits}-1)")
    print(f" - Step Size per Segment: {step:,} steps")
    
    # Setup figure (wider aspect ratio)
    fig, ax = plt.subplots(figsize=(14, 6))

    # ------------------------------
    # Axis Setup: X-axis = Time (Steps)
    # ------------------------------
    # Plot limit is the max of Total Period or Final Position
    final_pos = count * step
    plot_limit = max(max_period, final_pos)

    ax.set_xlim(0, plot_limit)
    ax.set_ylim(0, 1)
    ax.set_yticks([]) # No Y-axis ticks needed
    
    # X-axis label setup
    prefix = "Initial Seed"
    ax.set_xlabel(f"LFSR Sequence Timeline (Steps from {prefix})\nTotal Period: $2^{{{bits}}}-1$")
    ax.set_title(f"LFSR Reseed Coverage Map (Timeline View)\n{bits}-bit LFSR, {count} segments of {step:,} steps each")
    
    # Background (Grey bar showing total period space)
    if max_period <= plot_limit:
        ax.add_patch(patches.Rectangle((0, 0.25), max_period, 0.5, color="#e0e0e0", zorder=0, label="Total Period Space"))

    # ------------------------------
    # Loop to draw seed segments
    # ------------------------------
    # Vary colors to show order (using viridis colormap)
    cmap = plt.get_cmap('viridis', count)

    for i, seed_val in enumerate(seeds):
        # Calculate X position: steps from initial seed
        start_step = i * step
        
        # Width of the segment covered by this seed
        # (We use the STEP size to show equal distribution)
        rect_width = step
        
        color = cmap(i)
        
        # 1. Draw the segment as a colored bar
        # (x, y), width, height
        rect = patches.Rectangle((start_step, 0.25), rect_width, 0.5, 
                                 color=color, alpha=0.8, linewidth=0, zorder=1)
        ax.add_patch(rect)
        
        # 2. Draw a divider line at the start
        ax.axvline(x=start_step, color='black', linestyle='-', linewidth=1, alpha=0.3, ymin=0.2, ymax=0.8, zorder=2)
        
        # 3. Draw labels (Only show for first few and last few to avoid clutter)
        if i < 2 or i >= count - 2:
            # Format hex string
            hex_str = f"0x{seed_val:X}"
            # Alternating vertical positions to prevent overlap
            y_pos = 0.8 if i % 2 == 0 else 0.15
            # Draw text
            ax.text(start_step, y_pos, 
                    f"#{i+1}\n{hex_str}", 
                    fontsize=9, rotation=45, ha='left', va='center', zorder=3)

    # ------------------------------
    # Final touches
    # ------------------------------
    # Red line showing end of generated sequence
    if final_pos > 0:
        ax.axvline(x=final_pos, color='red', linestyle=':', linewidth=2, zorder=4)
        ax.text(final_pos, 0.5, " End of Generated Sequence", color='red', ha='left', va='center', fontsize=10, fontweight='bold')

    # Use scientific notation for large X-axis values
    if plot_limit > 1e5:
        ax.ticklabel_format(style='sci', axis='x', scilimits=(0,0), useMathText=True)

    # Layout adjustment and save
    plt.tight_layout()
    # Add bottom margin for x-axis labels
    plt.subplots_adjust(bottom=0.2) 
    
    plt.savefig(output_image, dpi=150)
    print(f"Saved visualization to {output_image}")


# --------------------------------------------------
# Main Execution
# --------------------------------------------------
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Visualize LFSR Seed Distribution (Timeline View)")
    parser.add_argument("input_file", help="Path to the LFSR output file (txt) from Reseed mode")
    parser.add_argument("-o", "--output", default="coverage_map.png", help="Output image file path")
    
    args = parser.parse_args()
    
    meta, seeds = parse_lfsr_file(args.input_file)
    visualize_coverage(meta, seeds, args.output)