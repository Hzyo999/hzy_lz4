#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <zlib.h>
#include <fstream>

class MicroCompressionTester {
private:
    // 启发式阈值参数
    const int CORESET_THRESHOLD_LOW = 50;
    const int CORESET_THRESHOLD_HIGH = 200;
    const double ENTROPY_THRESHOLD = 5.5;
    const double L2_DISTANCE_THRESHOLD = 0.001;

public:
    // 输入：数据块指针和大小；输出：是否压缩
    bool should_compress(const char* data, size_t size) {
        if (size <= 1024) return true; // 小数据直接压缩

        // 随机采样（最多128个位置，每个位置取16字节）
        size_t sample_size = std::min<size_t>(128 * 16, size);
        std::vector<char> sample = random_sample(data, size, sample_size);

        // 计算核心集大小
        int coreset_size = compute_coreset(sample);
        if (coreset_size < CORESET_THRESHOLD_LOW) return true;
        if (coreset_size > CORESET_THRESHOLD_HIGH) return false;

        // 计算熵
        double entropy = compute_entropy(sample);
        if (entropy < ENTROPY_THRESHOLD) return true;

        // 计算L2距离随机分布
        double l2_distance = compute_l2_distance(sample);
        if (entropy < 6.5) {
            return (l2_distance > L2_DISTANCE_THRESHOLD);
        } else {
            return (l2_distance > 0.02);
        }
    }

private:
    // 随机采样函数
    std::vector<char> random_sample(const char* data, size_t size, size_t sample_size) {
        std::vector<char> sample;
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<size_t> dist(0, size - 16); // 每次取16字节
        for (size_t i = 0; i < sample_size / 16; ++i) {
            size_t pos = dist(gen);
            sample.insert(sample.end(), data + pos, data + pos + 16);
        }
        return sample;
    }

    // 计算核心集大小（覆盖90%数据的唯一符号数）
    int compute_coreset(const std::vector<char>& sample) {
        std::unordered_map<char, int> freq;
        for (char c : sample) freq[c]++;
        std::vector<int> counts;
        for (auto& pair : freq) counts.push_back(pair.second);
        std::sort(counts.rbegin(), counts.rend());

        int total = sample.size();
        int sum = 0;
        int coreset = 0;
        for (int cnt : counts) {
            sum += cnt;
            coreset++;
            if (sum >= total * 0.9) break; // 覆盖90%
        }
        return coreset;
    }

    // 计算字节熵
    double compute_entropy(const std::vector<char>& sample) {
        std::unordered_map<char, int> freq;
        for (char c : sample) freq[c]++;
        double entropy = 0.0;
        for (auto& pair : freq) {
            double p = static_cast<double>(pair.second) / sample.size();
            entropy -= p * log2(p);
        }
        return entropy;
    }

    // 计算L2距离随机分布
    double compute_l2_distance(const std::vector<char>& sample) {
        std::unordered_map<char, int> single_freq;
        std::unordered_map<std::pair<char, char>, int, PairHash> pair_freq;
        for (size_t i = 0; i < sample.size() - 1; ++i) {
            char a = sample[i], b = sample[i+1];
            single_freq[a]++;
            pair_freq[{a, b}]++;
        }

        double l2 = 0.0;
        int total_pairs = sample.size() - 1;
        for (auto& pair : pair_freq) {
            char a = pair.first.first, b = pair.first.second;
            double expected = (single_freq[a] / static_cast<double>(total_pairs)) *
                               (single_freq[b] / static_cast<double>(total_pairs));
            double observed = pair.second / static_cast<double>(total_pairs);
            l2 += pow(expected - observed, 2);
        }
        return sqrt(l2);
    }

    // 辅助结构：用于pair的哈希
    struct PairHash {
        template <typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };
};