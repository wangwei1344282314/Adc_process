#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

std::vector<float> sim_data(int N) {
    std::vector<float> y(N);
    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0, 4.0);

    for (int i = 0; i < N; ++i) {
        float x = static_cast<float>(i) * (200.0f / N);
        y[i] = 2 * std::cos(2 * M_PI * 300 * x) +
               5 * std::sin(2 * M_PI * 100 * x) +
               distribution(generator);
    }

    return y;
}

std::vector<int> AMPD(const std::vector<float>& data) {
    std::vector<int> p_data(data.size(), 0);
    int count = data.size();
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

    int min_index = std::distance(arr_rowsum.begin(), std::min_element(arr_rowsum.begin(), arr_rowsum.end()));
    int max_window_length = min_index;

    for (int k = 1; k <= max_window_length; ++k) {
        for (int i = k; i < count - k; ++i) {
            if (data[i] > data[i - k] && data[i] > data[i + k]) {
                p_data[i] += 1;
            }
        }
    }

    std::vector<int> peaks;
    for (int i = 0; i < p_data.size(); ++i) {
        if (p_data[i] == max_window_length) {
            peaks.push_back(i);
        }
    }

    return peaks;
}

int main() {

    std::ifstream file("/home/gzpeite/test/0.txt"); // 打开记事本文件
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

    std::vector<float> y = values;

    // 找到波峰
    std::vector<int> peaks = AMPD(y);

    // 打印波峰索引及其值和相邻的两个数值
    for (int index : peaks) {
        float peak_value = y[index];
        float prev_value = (index > 0) ? y[index - 1] : 0.0f;  // 前一个值
        float next_value = (index < 2000 - 1) ? y[index + 1] : 0.0f;  // 后一个值
        std::cout << "Peak at index " << index << ": value = " << peak_value
                  << ", prev = " << prev_value << ", next = " << next_value << std::endl;
    }

    return 0;
}
