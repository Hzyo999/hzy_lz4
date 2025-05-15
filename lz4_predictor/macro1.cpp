#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <zlib.h>
#include <fstream>

// 宏定义：压缩块大小（根据论文设置为32KB）
#define LOCAL_WINDOW_SIZE 32768

class MacroCompressionEstimator {
private:
    double confidence;   // 置信度（如1e-7）
    double accuracy;     // 允许误差（如0.05）
    int min_samples;     // 最小样本量（根据Hoeffding计算）

public:
    MacroCompressionEstimator(double conf, double acc) : confidence(conf), accuracy(acc) {
        // 计算最小样本量：m >= (1/(2*A²)) * ln(2/C)
        min_samples = static_cast<int>(ceil( (1.0 / (2 * pow(acc, 2)) * log(2.0 / conf) )));
    }

    // 从文件中随机读取并压缩样本块，返回平均压缩率
    double estimate_compression_ratio(const std::string& file_path) {
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        size_t file_size = file.tellg();
        file.seekg(0);

        // 生成随机位置
        std::vector<size_t> sample_positions = generate_random_positions(file_size, min_samples);

        double total_ratio = 0.0;
        for (auto pos : sample_positions) {
            // 读取局部数据（考虑LZ77的32KB滑动窗口）
            size_t start = (pos >= LOCAL_WINDOW_SIZE) ? (pos - LOCAL_WINDOW_SIZE) : 0;
            size_t read_size = LOCAL_WINDOW_SIZE + 256; // 32KB窗口 + 256B待测数据
            file.seekg(start);
            std::vector<char> buffer(read_size);
            file.read(buffer.data(), read_size);

            // 模拟压缩上下文：预热滑动窗口（压缩前32KB但不保存结果）
            z_stream warmup_stream;
            deflateInit2(&warmup_stream, Z_BEST_SPEED, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
            deflate(&warmup_stream, Z_FULL_FLUSH); // 仅初始化上下文

            // 压缩待测的256B数据
            z_stream test_stream;
            deflateInit2(&test_stream, Z_BEST_SPEED, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
            std::vector<Bytef> compressed(256);
            test_stream.next_in = (Bytef*)(buffer.data() + LOCAL_WINDOW_SIZE);
            test_stream.avail_in = 256;
            test_stream.next_out = compressed.data();
            test_stream.avail_out = 256;
            deflate(&test_stream, Z_FINISH);
            double ratio = (test_stream.total_out) / 256.0;

            total_ratio += ratio;
            deflateEnd(&warmup_stream);
            deflateEnd(&test_stream);
        }
        return total_ratio / min_samples;
    }

private:
    // 生成不重复的随机位置
    std::vector<size_t> generate_random_positions(size_t max_pos, int num_samples) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, max_pos - LOCAL_WINDOW_SIZE - 256);
        std::vector<size_t> positions;
        for (int i = 0; i < num_samples; ++i) {
            positions.push_back(dist(gen));
        }
        return positions;
    }
};