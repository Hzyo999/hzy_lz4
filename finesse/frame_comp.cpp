#include <iostream>
#include <fstream>
#include <vector>
#include <lz4frame.h>

// LZ4压缩函数
std::vector<char> lz4_compress(const char* src, size_t src_size) {
    LZ4F_compressionContext_t ctx = nullptr;
    LZ4F_errorCode_t ret = LZ4F_createCompressionContext(&ctx, LZ4F_VERSION);
    if (LZ4F_isError(ret)) {
        std::cerr << "创建压缩上下文失败: " << LZ4F_getErrorName(ret) << std::endl;
        return {};
    }

    // typedef struct {
    // LZ4F_blockSizeID_t     blockSizeID;         /* max64KB, max256KB, max1MB, max4MB; 0 == default (LZ4F_max64KB) */
    // LZ4F_blockMode_t       blockMode;           /* LZ4F_blockLinked, LZ4F_blockIndependent; 0 == default (LZ4F_blockLinked) */
    // LZ4F_contentChecksum_t contentChecksumFlag; /* 1: add a 32-bit checksum of frame's decompressed data; 0 == default (disabled) */
    // LZ4F_frameType_t       frameType;           /* read-only field : LZ4F_frame or LZ4F_skippableFrame */
    // unsigned long long     contentSize;         /* Size of uncompressed content ; 0 == unknown */
    // unsigned               dictID;              /* Dictionary ID, sent by compressor to help decoder select correct dictionary; 0 == no dictID provided */
    // LZ4F_blockChecksum_t   blockChecksumFlag;   /* 1: each block followed by a checksum of block's compressed data; 0 == default (disabled) */
    // } LZ4F_frameInfo_t;

    // 配置压缩参数
    LZ4F_preferences_t prefs = {
        .frameInfo = { 
            .blockSizeID = LZ4F_max64KB,
            .blockMode = LZ4F_blockIndependent,
            .contentChecksumFlag = LZ4F_contentChecksumEnabled  // 启用校验和
        },
        .compressionLevel = 1,
        .autoFlush = 1
    };

    // 计算所需缓冲区大小
    const size_t max_compressed = LZ4F_compressBound(src_size, &prefs);
    std::vector<char> dst_buffer(max_compressed + LZ4F_HEADER_SIZE_MAX);

    // 执行压缩
    size_t offset = 0;
    
    // 写入帧头
    ret = LZ4F_compressBegin(ctx, dst_buffer.data(), dst_buffer.size(), &prefs);
    if (LZ4F_isError(ret)) {
        std::cerr << "压缩头错误: " << LZ4F_getErrorName(ret) << std::endl;
        LZ4F_freeCompressionContext(ctx);
        return {};
    }
    offset += ret;

    // 压缩数据
    ret = LZ4F_compressUpdate(ctx, 
                            dst_buffer.data() + offset, 
                            dst_buffer.size() - offset,
                            src, 
                            src_size, 
                            nullptr);
    if (LZ4F_isError(ret)) {
        std::cerr << "压缩错误: " << LZ4F_getErrorName(ret) << std::endl;
        LZ4F_freeCompressionContext(ctx);
        return {};
    }
    offset += ret;

    // 结束压缩
    ret = LZ4F_compressEnd(ctx, 
                         dst_buffer.data() + offset, 
                         dst_buffer.size() - offset, 
                         nullptr);
    if (LZ4F_isError(ret)) {
        std::cerr << "压缩结束错误: " << LZ4F_getErrorName(ret) << std::endl;
        LZ4F_freeCompressionContext(ctx);
        return {};
    }
    offset += ret;

    LZ4F_freeCompressionContext(ctx);
    dst_buffer.resize(offset);
    return dst_buffer;
}

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <输入文件> <输出文件.lz4>" << std::endl;
        return 1;
    }

    const char* input_path = argv[1];
    const char* output_path = argv[2];

    // 读取输入文件
    std::ifstream input_file(input_path, std::ios::binary | std::ios::ate);
    if (!input_file.is_open()) {
        std::cerr << "无法打开输入文件: " << input_path << std::endl;
        return 2;
    }

    // 获取文件大小
    const size_t file_size = input_file.tellg();
    input_file.seekg(0);

    // 读取文件内容
    std::vector<char> input_data(file_size);
    if (!input_file.read(input_data.data(), file_size)) {
        std::cerr << "读取文件失败: " << input_path << std::endl;
        return 3;
    }

    // 执行压缩
    auto compressed = lz4_compress(input_data.data(), file_size);
    if (compressed.empty()) {
        std::cerr << "压缩失败" << std::endl;
        return 4;
    }

    // 写入输出文件
    // std::ofstream output_file(output_path, std::ios::binary);
    // if (!output_file.write(compressed.data(), compressed.size())) {
    //     std::cerr << "写入输出文件失败: " << output_path << std::endl;
    //     return 5;
    // }

    std::cout << "压缩成功: " << file_size << " → " << compressed.size() 
              << " (" << (100.0 * compressed.size() / file_size) << "%)" 
              << std::endl;
    return 0;
}