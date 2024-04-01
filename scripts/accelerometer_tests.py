DESCRIPTION = """
A tool for acquiring accelerometer data and plotting/modeling it

To acquire data:
1. Flash the pico with the accelerometer_data.uf2 program
2. Plug in the wand with its battery removed
3. Run this program in data acquisition mode

To plot data:
1. Run this program in plot mode
"""

import argparse
import serial, os
from matplotlib import pyplot as plt
from pathlib import Path

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=DESCRIPTION, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("com_port", type=str, help="The COM port the wand is plugged into")
    parser.add_argument("data_path", type=Path, help="The path to a csv file store/load data from")
    parser.add_argument("--plot_mode", type=bool, default=True, help="Mode: True to plot acquired data, False to acquire new data from the wand")

    args = parser.parse_args()

    assert args.data_path.suffix == ".csv", "data_path must be a csv file"

    if args.plot_mode == True:
        # Open the csv file at data_path and plot the data in matplotlib
        if not args.data_path.exists():
            raise Exception(f"Could not find a csv file at {args.data_path}")
    else:
        # Open the com port and start polling for data
        