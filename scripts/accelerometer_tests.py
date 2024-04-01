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
import numpy as np
from matplotlib import pyplot as plt
from pathlib import Path


def calc_direction_from_jerk(jerks: np.array):
    """Calculates the direction implied by the jerk of the wand"""

    dirs = np.zeros_like(jerks)

    for i in range(1, len(dirs)):
        if jerks[i] < 0:
            dirs[i] = -10
        elif jerks[i] > 0:
            dirs[i] = 10
        else:
            dirs[i] = dirs[i - 1]  # hold direction if there was no jerk

    return dirs


def hysteresize_direction(dirs: np.array, hyst_len: int):
    """
    Applies a hysteresis effect to the direction
    The direction will not change until hyst_len samples all agree
    """

    new_dirs = np.zeros_like(jerks)

    for i in range(hyst_len, len(dirs)):
        if (dirs[i-hyst_len:i] < 0).all():
            new_dirs[i] = -10
        elif (dirs[i-hyst_len:i] > 0).all():
            new_dirs[i] = 10
        else:
            new_dirs[i] = new_dirs[i - 1]  # hold direction if there was no consistent series       

    return new_dirs


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=DESCRIPTION, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("com_port", type=str, help="The COM port the wand is plugged into")
    parser.add_argument("data_path", type=Path, help="The path to a csv file store/load data from")
    parser.add_argument("--data_mode", default=False, action='store_true', help="Data acquire mode")
    parser.add_argument("--ptime_start", type=float, default=0, help="The time in seconds to start plotting from")
    parser.add_argument("--ptime_end", type=float, default=-1, help="The time in seconds to end plotting at")

    args = parser.parse_args()
    print(args)

    assert args.data_path.suffix == ".csv", "data_path must be a csv file"

    if args.data_mode == False:
        # Open the csv file at data_path and plot the data in matplotlib
        if not args.data_path.exists():
            raise Exception(f"Could not find a csv file at {args.data_path}")
        
        start = args.ptime_start * 1_000_000
        end = args.ptime_end * 1_000_000 if args.ptime_end >= 0 else np.inf

        # plot accel from start to end
        data = np.loadtxt(args.data_path, delimiter=',', skiprows=1, dtype=np.double)

        print(f"Loaded data parameters:\n\tstart time: {data[0, -1] / 1_000_000}s\n\tend time: {data[-1, -1] / 1_000_000}s")

        # trim data to specified time scale
        data = data[data[:, -1] < end]
        data = data[data[:, -1] > start]

        # get the various data ranges to plot
        accels = data[:, 0]
        times = data[:, 1] / 1_000_000
        
        # calculate jerk
        jerks = np.zeros_like(accels)
        for i in range(1, len(jerks)):
            jerks[i] = (accels[i] - accels[i-1]) / ((times[i] - times[i-1]))


        f, axes = plt.subplots(nrows=2, ncols=2)
        # plot accel
        axes[0][0].plot(times, accels)
        axes[0][0].set_title("Accelerometer data")
        axes[0][0].set_xlabel("Time (s)")
        axes[0][0].set_ylabel("Acceleration (m/s^2)")

        # plot jerk
        axes[1][0].plot(times, jerks)
        axes[1][0].set_title("Jerk data")
        axes[1][0].set_xlabel("Time (s)")
        axes[1][0].set_ylabel("Jerk (m/s^3)")

        # get and plot the direction implied by the jerk with no averaging window
        direction = calc_direction_from_jerk(jerks)
        axes[0][1].plot(times, accels)
        axes[0][1].plot(times, direction)
        axes[0][1].set_title("Jerk Dir imposed over Accel")
        axes[0][1].set_xlabel("Time (s)")
        axes[0][1].set_ylabel("Acceleration (m/s^2)")

        # get and plot the direction implied by the jerk with a hysteresis effect
        hyst_len = 25
        new_direction = hysteresize_direction(direction, hyst_len)
        axes[1][1].plot(times, accels)
        axes[1][1].plot(times, new_direction)
        axes[1][1].set_title(f"Jerk Dir imposed over Accel (with hysteresis {hyst_len=})")
        axes[1][1].set_xlabel("Time (s)")
        axes[1][1].set_ylabel("Acceleration (m/s^2)")
        f.tight_layout()
        plt.show()

    else:
        # Open the com port and start polling for data
        with serial.Serial(args.com_port, baudrate=115200, timeout=10) as ser:
            # Clear the data file / write header
            with open(args.data_path, 'w') as f:
                f.write("Acceleration (mss), Time since boot (us)")

            print("Acquiring data from the wand... Ctrl+C to finish")

            # Open the data file and start polling / appending
            with open(args.data_path, 'a') as f:
                while True:
                    m = ser.readline().decode('utf-8')[:-1] # decode and strip newline
                    f.write(m)
