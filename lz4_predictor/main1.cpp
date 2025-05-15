#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <zlib.h>
#include <fstream>
#include <algorithm>
#include <unordered_map>


#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <lz4.h>

// 宏定义：LZ4 字典块大小 64KB
#define BLOCK_SIZE 65536
// 压缩数据大小 压缩1KB数据
#define WINDOW_SIZE 1024 
#define DATA_SIZE (BLOCK_SIZE + WINDOW_SIZE)

class LZ4MacroEstimator {
private:
    double confidence;   // 置信度（如1e-7）
    double accuracy;     // 允许误差（如0.05）
    int min_samples;     // 最小样本量（根据Hoeffding计算）

public:
    LZ4MacroEstimator(double conf, double acc) : confidence(conf), accuracy(acc) {
        // 计算最小样本量：m >= (1/(2*A²)) * ln(2/C)
        min_samples = static_cast<int>(ceil( (1.0 / (2 * pow(acc, 2))) * log(2.0 / conf) ));
    }

    // 从文件中随机读取并压缩样本块，返回平均压缩率
    double estimate_compression_ratio(const std::string& file_path) {
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        size_t file_size = file.tellg();
        file.seekg(0);

        // 生成随机位置（确保不超出文件范围）
        std::vector<size_t> sample_positions = generate_random_positions(file_size, min_samples);

        double total_ratio = 0.0;
        double total_compress_size = 0.0;
        for (auto pos : sample_positions) {
            // pos范围为 [0 file_size-window_size]
            // 读取待压缩的 BLOCK_SIZE 数据块
            size_t read_pos = pos % (file_size - DATA_SIZE); // 确保不越界
            file.seekg(read_pos);
            std::vector<char> dict(BLOCK_SIZE);
            file.read(dict.data(), BLOCK_SIZE);
            read_pos = pos + BLOCK_SIZE;
            std::vector<char> input(WINDOW_SIZE);
            file.read(input.data(), WINDOW_SIZE);

            // 创建 LZ4 流并加载字典
            LZ4_stream_t* lz4Stream = LZ4_createStream();
            //这个函数到底是直接load字典还是使用64KB训练字典
            int dictSize = LZ4_loadDict(lz4Stream, dict.data(), static_cast<int>(dict.size()));
            // 使用 LZ4 压缩当前块
            int max_compressed_size = LZ4_compressBound(DATA_SIZE);
            std::vector<char> compressed(max_compressed_size);
            // 使用字典压缩数据
            int compressed_size = LZ4_compress_fast_continue(
                lz4Stream,
                input.data(),
                compressed.data(),
                static_cast<int>(input.size()),
                static_cast<int>(compressed.size()),
                1
            );

            // 计算压缩率（压缩后大小 / 原始大小）
            if (compressed_size <= 0) {
                // 压缩失败，按不可压缩处理（比率=1.0）
                std::cout<<"pos : "<<pos<<" compress fault\n";
                total_ratio += 1.0;
            } else {

                total_compress_size += compressed_size;
                //std::cout<<"total_compress_size : "<<total_compress_size<<"\n";
                // double ratio = static_cast<double>(compressed_size) / BLOCK_SIZE;
                // total_ratio += ratio;
            }
        }
        return (double) (total_compress_size / (min_samples * WINDOW_SIZE)) ;
    }

private:
    // 生成不重复的随机位置
    std::vector<size_t> generate_random_positions(size_t max_pos, int num_samples) {
        if(max_pos < BLOCK_SIZE + WINDOW_SIZE){
            std::cout<<"file size too small\n";
            return std::vector<size_t>();
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, max_pos - BLOCK_SIZE - WINDOW_SIZE);
        std::vector<size_t> positions;
        for (int i = 0; i < num_samples; ++i) {
            positions.push_back(dist(gen));
        }
        return positions;
    }
};

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


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];

    // 宏观压缩比估计
    LZ4MacroEstimator macro_estimator(1e-7, 0.05);
    double ratio = macro_estimator.estimate_compression_ratio(filePath);
    std::cout << "Estimated compression ratio: " << ratio << std::endl;

    // 微观压缩决策
    MicroCompressionTester micro_tester;
    std::vector<char> write_buffer(8192); // 8KB写入块
    bool compress = micro_tester.should_compress(write_buffer.data(), write_buffer.size());
    std::cout << "Should compress: " << (compress ? "Yes" : "No") << std::endl;

    return 0;
}