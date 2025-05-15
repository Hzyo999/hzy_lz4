
#include <iostream>
#include <vector>
#include <random>
#include <lz4.h> // 修改为LZ4头文件
#include <fstream>
#include <cmath>
#include <zlib.h>
// 计算单个数据块的LZ4压缩率
double compute_compression_ratio_lz4(const std::vector<char>& data) {
    int max_compressed_size = LZ4_compressBound(data.size());
    std::vector<char> compressed(max_compressed_size);
    
    // 使用LZ4快速压缩
    int compressed_size = LZ4_compress_default(
        data.data(), 
        compressed.data(), 
        data.size(), 
        max_compressed_size
    );
    
    if (compressed_size <= 0) {
        return 1.0; // 压缩失败返回原大小
    }
    return static_cast<double>(compressed_size) / data.size();
}

// 基于LZ4的宏尺度压缩率估计
double macro_compress_estimate_lz4(const std::string& file_path,
                                   double accuracy = 0.05,
                                   double confidence = 1e-7) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 基于Hoeffding不等式计算样本量（公式不变）
    size_t m = static_cast<size_t>(std::ceil(std::log(2 / confidence) / (2 * accuracy * accuracy)));
    std::vector<double> samples;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, file_size - 4096); // 4KB块

    // LZ4压缩采样
    for (size_t i = 0; i < m; ++i) {
        size_t pos = dist(gen);
        file.seekg(pos);
        std::vector<char> buffer(4096);
        file.read(buffer.data(), 4096);
        samples.push_back(compute_compression_ratio_lz4(buffer));
    }

    // 计算平均压缩率（逻辑不变）
    double avg = std::accumulate(samples.begin(), samples.end(), 0.0) / m;
    return avg;
}

int main() {
    std::string file = "large_data.bin";
    double ratio = macro_compress_estimate_lz4(file);
    std::cout << "LZ4 Estimated Compression Ratio: " << ratio << std::endl;
    return 0;
}


// // 计算单个数据块的压缩率
// double compute_compression_ratio(const std::vector<char>& data) {
//     uLongf compressed_size = compressBound(data.size());
//     std::vector<Bytef> compressed(compressed_size);
//     if (compress2(compressed.data(), &compressed_size,
//                   reinterpret_cast<const Bytef*>(data.data()), data.size(),
//                   Z_BEST_SPEED) != Z_OK) {
//         return 1.0; // 压缩失败时返回原大小
//     }
//     return static_cast<double>(compressed_size) / data.size();
// }

// // 宏尺度压缩率估计
// double macro_compress_estimate(const std::string& file_path, 
//                                double accuracy = 0.05, 
//                                double confidence = 1e-7) {
//     std::ifstream file(file_path, std::ios::binary | std::ios::ate);
//     size_t file_size = file.tellg();
//     file.seekg(0, std::ios::beg);

//     // 计算所需样本数（基于Hoeffding不等式）
//     size_t m = static_cast<size_t>(std::ceil(std::log(2 / confidence) / (2 * accuracy * accuracy)));
//     std::vector<double> samples;
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_int_distribution<size_t> dist(0, file_size - 4096); // 4KB块

//     // 随机采样并压缩
//     for (size_t i = 0; i < m; ++i) {
//         size_t pos = dist(gen);
//         file.seekg(pos);
//         std::vector<char> buffer(4096);
//         file.read(buffer.data(), 4096);
//         samples.push_back(compute_compression_ratio(buffer));
//     }

//     // 计算平均压缩率
//     double avg = std::accumulate(samples.begin(), samples.end(), 0.0) / m;
//     return avg;
// }

// int main() {
//     std::string file = "large_data.bin";
//     double ratio = macro_compress_estimate(file);
//     std::cout << "Estimated Compression Ratio: " << ratio << std::endl;
//     return 0;
// }