#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <limits>
#include <algorithm>

std::vector<int> AMPD(const std::vector<float>& data) {
    int count = data.size();
    std::vector<int> p_data(count, 0);
    std::vector<int> arr_rowsum;

    for (int k = 1; k <= count / 2; ++k) {
        int row_sum = 0;
        for (int i = k; i < count - k; ++i) {
            if (data[i] > data[i - k] && data[i] > data[i + k]) {
                row_sum -= 1;
            }
        }
        arr_rowsum.push_back(row_sum);
    }

    auto min_index = std::min_element(arr_rowsum.begin(), arr_rowsum.end()) - arr_rowsum.begin();
    int max_window_length = min_index;

    for (int k = 1; k <= max_window_length; ++k) {
        for (int i = k; i < count - k; ++i) {
            if (data[i] > data[i - k] && data[i] > data[i + k]) {
                p_data[i] += 1;
            }
        }
    }

    std::vector<int> peaks;
    for (int i = 0; i < count; ++i) {
        if (p_data[i] == max_window_length) {
            peaks.push_back(i);
        }
    }

    return peaks;
}

// 移动平均滤波器
std::vector<float> movingAverageFilter(const std::vector<float>& data, int windowSize) {
    std::vector<float> filteredData;
    int halfWindow = windowSize / 2;
    for (size_t i = 0; i < data.size(); ++i) {
        float sum = 0;
        int count = 0;
        for (int j = -halfWindow; j <= halfWindow; ++j) {
            if (i + j >= 0 && i + j < data.size()) {
                sum += data[i + j];
                count++;
            }
        }
        filteredData.push_back(sum / count);
    }
    return filteredData;
}

// 中值滤波器
std::vector<float> medianFilter(const std::vector<float>& data, int windowSize) {
    std::vector<float> filteredData;
    int halfWindow = windowSize / 2;
    for (size_t i = 0; i < data.size(); ++i) {
        std::vector<float> window;
        for (int j = -halfWindow; j <= halfWindow; ++j) {
            if (i + j >= 0 && i + j < data.size()) {
                window.push_back(data[i + j]);
            }
        }
        std::sort(window.begin(), window.end());
        filteredData.push_back(window[window.size() / 2]); // 取中值
    }
    return filteredData;
}

// 指数平滑滤波器
std::vector<float> exponentialSmoothingFilter(const std::vector<float>& data, float alpha) {
    std::vector<float> filteredData;
    if (data.empty()) return filteredData;

    filteredData.push_back(data[0]); // 初始化第一值
    for (size_t i = 1; i < data.size(); ++i) {
        filteredData.push_back(alpha * data[i] + (1 - alpha) * filteredData[i - 1]);
    }
    return filteredData;
}

class FindPV {
public:
    std::vector<int> Pos_Peak;   // 波峰位置
    std::vector<int> Pos_Valley; // 波谷位置
    int Pcnt;                    // 识别的波峰计数
    int Vcnt;                    // 识别的波谷计数

    FindPV() : Pcnt(0), Vcnt(0) {
        // Pos_Peak.reserve(SAMPLE_MAX); // 预留空间
        // Pos_Valley.reserve(SAMPLE_MAX);
    }
};

void initialFindPV(FindPV &findPV) {
    // SampleDiff.assign(SAMPLE_MAX, 0.0);
    findPV.Pos_Peak.clear();
    findPV.Pos_Valley.clear();
    findPV.Pcnt = 0;
    findPV.Vcnt = 0;
}

void FindPVAlgorithm(FindPV &findPV, const std::vector<float>& Sample) {
    // 根据输入的样本大小动态分配 SampleDiff
    std::vector<float> SampleDiff(Sample.size(), 0.0);

    // Step 1: Forward difference and normalization
    for (size_t i = 0; i < Sample.size() - 1; i++) {
        float samplei1 = Sample[i + 1];
        float samplei = Sample[i];
        if (samplei1 - samplei > 0)
            SampleDiff[i] = 1;
        else if (samplei1 - samplei < 0)
            SampleDiff[i] = -1;
        else
            SampleDiff[i] = 0;
    }

    // Step 2: Edge slope processing
    for (size_t i = Sample.size() - 1; i > 0; i--) {
        if (SampleDiff[i] == 0) {
            if (i == (Sample.size() - 2)) {
                SampleDiff[i] = (SampleDiff[i - 1] >= 0) ? 1 : -1;
            } else {
                SampleDiff[i] = (SampleDiff[i + 1] >= 0) ? 1 : -1;
            }
        }
    }

    // Step 3: Peak and valley recognition
    for (size_t i = 0; i < Sample.size() - 1; i++) {
        if (SampleDiff[i + 1] - SampleDiff[i] == -2) { // Peak recognition
            findPV.Pos_Peak.push_back(i + 1);
            findPV.Pcnt++;
        } else if (SampleDiff[i + 1] - SampleDiff[i] == 2) { // Valley recognition
            findPV.Pos_Valley.push_back(i + 1);
            findPV.Vcnt++;
        }
    }
}

// 波峰筛选函数，将找到的波峰过滤掉小于阈值的波峰
void FilterPeaks(FindPV &findPV, const std::vector<float>& Sample, float threshold) {
    // 删除小于阈值的波峰
    for (auto it = findPV.Pos_Peak.begin(); it != findPV.Pos_Peak.end(); ) {
        int peakIndex = *it;
        float peakValue = Sample[peakIndex];

        if (peakValue < threshold) {
            // 删除当前的波峰位置
            it = findPV.Pos_Peak.erase(it);
            findPV.Pcnt--; // 更新波峰计数
        } else {
            ++it;
        }
    }
}

int main() {
    std::ifstream file("/home/gzpeite/Adc_process/0.txt"); // 打开记事本文件
    std::vector<float> values;      // 存储每一行的浮点数

    if (!file) {
        std::cerr << "无法打开文件!" << std::endl;
        return 1; // 返回错误
    }

    std::string line;
    while (std::getline(file, line)) {
        float value = std::stod(line); // 将每一行转换为浮点数
        values.push_back(value); // 将浮点数保存到 vector 中
    }

    file.close(); // 关闭文件

    // 打印保存的浮点数
    for (size_t i = 0; i < values.size() && i < 5; ++i) {
        std::cout << "Value " << i + 1 << ": " << values[i] << std::endl;
    }

    // 应用移动平均滤波
    int windowSize = 5; // 窗口大小为5
    // 应用指数平滑滤波
    float alpha = 0.2f; // 平滑因子
    std::vector<float> movingAvgFiltered = values;// movingAverageFilter(values, windowSize);

    // movingAvgFiltered={1,2,3,4,4,4,5,2,1,0,0,5,1,0,0,1,2,3,4,0,1};

    values = movingAvgFiltered;

    FindPV stFindPV;
    initialFindPV(stFindPV);
    FindPVAlgorithm(stFindPV, movingAvgFiltered);

    // 定义阈值，过滤掉小于阈值的波峰
    float threshold = 0.5;
    FilterPeaks(stFindPV, movingAvgFiltered, threshold);

    std::cout << "Peak count: " << stFindPV.Pcnt << std::endl;
    for (const auto &peak : stFindPV.Pos_Peak) {
        std::cout << "Peak at position " << peak + 1 << ": " << values[peak]
                  << ", Neighbors: [" 
                  << (peak > 0 ? values[peak - 1] : 0) << ", " 
                  << (peak < values.size() - 1 ? values[peak + 1] : 0) << "]" << std::endl;
    }

    std::cout << std::endl << "Valley count: " << stFindPV.Vcnt << std::endl;
    for (const auto &valley : stFindPV.Pos_Valley) {
        std::cout << "Valley at position " << valley + 1 << ": " << values[valley]
                  << ", Neighbors: [" 
                  << (valley > 0 ? values[valley - 1] : 0) << ", " 
                  << (valley < values.size() - 1 ? values[valley + 1] : 0) << "]" << std::endl;

    }

    std::cout << std::endl << std::endl;

    return 0;
}
