import numpy as np
import matplotlib.pyplot as plt

def moving_average_filter(data, window_size):
    filtered_data = []
    half_window = window_size // 2  # 

    for i in range(len(data)):
        sum_values = 0
        count = 0

        # 
        for j in range(-half_window, half_window + 1):
            if 0 <= i + j < len(data):
                sum_values += data[i + j]
                count += 1

        # 
        filtered_data.append(sum_values / count)

    return filtered_data

class FindPV:
    def __init__(self):
        self.Pos_Peak = []
        self.Pos_Valley = []
        self.Pcnt = 0
        self.Vcnt = 0

def initialFindPV(findPV):
    findPV.Pos_Peak.clear()
    findPV.Pos_Valley.clear()
    findPV.Pcnt = 0
    findPV.Vcnt = 0

def FindPVAlgorithm(findPV, Sample):
    # Step 1: Forward difference and normalization
    SampleDiff = [0.0] * len(Sample)
    for i in range(len(Sample) - 1):
        samplei1 = Sample[i + 1]
        samplei = Sample[i]
        if samplei1 - samplei > 0:
            SampleDiff[i] = 1
        elif samplei1 - samplei < 0:
            SampleDiff[i] = -1
        else:
            SampleDiff[i] = 0

    # Step 2: Edge slope processing
    for i in range(len(Sample) - 1, 0, -1):
        if SampleDiff[i] == 0:
            if i == len(Sample) - 1:
                SampleDiff[i] = 1 if SampleDiff[i - 1] >= 0 else -1
            else:
                SampleDiff[i] = 1 if SampleDiff[i + 1] >= 0 else -1

    # Step 3: Peak and valley recognition
    for i in range(len(Sample) - 1):
        if SampleDiff[i + 1] - SampleDiff[i] == -2:  # Peak recognition
            findPV.Pos_Peak.append(i + 1)
            findPV.Pcnt += 1
        elif SampleDiff[i + 1] - SampleDiff[i] == 2:  # Valley recognition
            findPV.Pos_Valley.append(i + 1)
            findPV.Vcnt += 1

def FilterPeaks(findPV, values, threshold):
    filtered_peaks = []
    for peak in findPV.Pos_Peak:
        if values[peak] > threshold:
            filtered_peaks.append(peak)
    findPV.Pos_Peak = filtered_peaks
    findPV.Pcnt = len(filtered_peaks)

# Plotting function
def plot_data(values, peaks, valleys):
    plt.figure(figsize=(10, 6))
    plt.plot(values, label='Data', color='blue', linewidth=1)
    
    # # Plot peaks
    # if peaks:
    #     plt.plot(peaks, [values[p] for p in peaks], 'ro', label='Peaks')
    
    # Plot valleys
    if valleys:
        plt.plot(valleys, [values[v] for v in valleys], 'ro', label='Valleys')

    # 
    for i in range(len(valleys) - 1):
        valley1 = valleys[i]
        valley2 = valleys[i + 1]
        distance = valley2 - valley1
        # 
        mid_point = (valley1 + valley2) // 2

            # 
        if distance > 100:
            color = 'red'  # 
        else:
            color = 'blue'   # 

        plt.text(mid_point, (values[valley1] + values[valley2]) / 2, str(distance), color=color, fontsize=12)

    
    plt.title('Data from TXT file with Peaks and Valleys')
    plt.xlabel('Index')
    plt.ylabel('Value')
    plt.legend()
    plt.grid(True)
    plt.show()

# Main function equivalent in Python
def main():
    # Open file and read values
    try:
        with open(r"/home/gzpeite/test/0.txt") as file:
            values = [float(line.strip()) for line in file.readlines()]
    except FileNotFoundError:
        print("无法打开文件!")
        return 1

    # Print the first 5 values
    for i, value in enumerate(values[:5]):
        print(f"Value {i + 1}: {value}")

    window_size = 12
    # Example filtered data
    movingAvgFiltered = moving_average_filter(values, window_size)

    # movingAvgFiltered = values

    # Find peaks and valleys
    stFindPV = FindPV()
    initialFindPV(stFindPV)
    FindPVAlgorithm(stFindPV, movingAvgFiltered)

    # Define threshold and filter peaks
    threshold = 0.5
    FilterPeaks(stFindPV, movingAvgFiltered, threshold)

    # Output peaks
    print(f"Peak count: {stFindPV.Pcnt}")
    for peak in stFindPV.Pos_Peak:
        neighbors = [movingAvgFiltered[peak - 1] if peak > 0 else 0,
                     movingAvgFiltered[peak + 1] if peak < len(movingAvgFiltered) - 1 else 0]
        print(f"Peak at position {peak + 1}: {movingAvgFiltered[peak]}, Neighbors: {neighbors}")

    # Output valleys
    print(f"\nValley count: {stFindPV.Vcnt}")
    for valley in stFindPV.Pos_Valley:
        neighbors = [movingAvgFiltered[valley - 1] if valley > 0 else 0,
                     movingAvgFiltered[valley + 1] if valley < len(movingAvgFiltered) - 1 else 0]
        print(f"Valley at position {valley + 1}: {movingAvgFiltered[valley]}, Neighbors: {neighbors}")

    # Plot the data along with peaks and valleys
    plot_data(movingAvgFiltered, stFindPV.Pos_Peak, stFindPV.Pos_Valley)
if __name__ == "__main__":
    main()