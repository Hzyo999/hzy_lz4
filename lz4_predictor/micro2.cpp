
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <lz4.h>

// 基于文件路径的微尺度压缩检测
bool micro_compress_check_lz4(const std::string& file_path, 
                             double threshold = 0.7,
                             size_t sample_size = 4096) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 启发式采样策略
    const size_t prefix_size = 1024; // 头部采样1KB
    const size_t random_samples = 3; // 随机位置采样次数
    std::vector<char> buffer(sample_size);

    // 1. 头部快速检测
    file.read(buffer.data(), prefix_size);
    size_t compressed_size = LZ4_compress_default(
        buffer.data(), 
        new char[LZ4_compressBound(prefix_size)], 
        prefix_size, 
        LZ4_compressBound(prefix_size)
    );
    if (compressed_size < prefix_size * threshold) {
        return true; // 头部可压缩
    }

    // 2. 随机位置采样检测
    std::uniform_int_distribution<size_t> dist(0, file_size - sample_size);
    std::mt19937 gen(std::random_device{}());
    
    for (int i = 0; i < random_samples; ++i) {
        size_t pos = dist(gen);
        file.seekg(pos);
        file.read(buffer.data(), sample_size);

        compressed_size = LZ4_compress_default(
            buffer.data(), 
            new char[LZ4_compressBound(sample_size)], 
            sample_size, 
            LZ4_compressBound(sample_size)
        );
        if (compressed_size < sample_size * threshold) {
            return true; // 发现可压缩区块
        }
    }

    // 3. 全块最终验证
    file.seekg(0);
    while (file.read(buffer.data(), sample_size)) {
        compressed_size = LZ4_compress_default(
            buffer.data(), 
            new char[LZ4_compressBound(sample_size)], 
            sample_size, 
            LZ4_compressBound(sample_size)
        );
        if (compressed_size < sample_size * threshold) {
            return true;
        }
    }
    
    return false; // 判定为不可压缩文件
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    bool compressible = micro_compress_check_lz4(file_path);
    
    std::cout << "File: " << file_path 
              << " is " << (compressible ? "compressible" : "incompressible")
              << " with LZ4" << std::endl;
    
    return 0;
}

// // 启发式快速检测（基于熵和重复模式）
// bool is_compressible_heuristic(const std::vector<char>& data) {
//     size_t repeat_count = 0;
//     for (size_t i = 1; i < data.size(); ++i) {
//         if (data[i] == data[i-1]) repeat_count++;
//     }
//     return (repeat_count > data.size() / 4); // 简单重复模式阈值
// }

// // 微尺度在线检测
// void micro_compress_check(const std::vector<char>& data) {
//     const size_t prefix_size = 512; // 检测前512字节
//     std::vector<char> prefix(data.begin(), data.begin() + std::min(prefix_size, data.size()));
    
//     // 启发式快速检测
//     if (!is_compressible_heuristic(prefix)) {
//         // 直接存储原始数据
//         return; 
//     }

//     // 前缀压缩检测
//     int max_compressed = LZ4_compressBound(prefix_size);
//     std::vector<char> compressed(max_compressed);
//     int compressed_size = LZ4_compress_default(prefix.data(), compressed.data(), prefix_size, max_compressed);
    
//     if (compressed_size < prefix_size * 0.9) { // 压缩率阈值
//         // 压缩全部数据
//         std::vector<char> full_compressed(LZ4_compressBound(data.size()));
//         LZ4_compress_default(data.data(), full_compressed.data(), data.size(), full_compressed.size());
//     } else {
//         // 存储原始数据
//     }
// }

// int main() {
//     std::vector<char> buffer(32768, 'A'); // 32KB测试数据
//     micro_compress_check(buffer);
//     return 0;
// }