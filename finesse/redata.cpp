#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

// 定义4KB的大小
const size_t chunkSize = 8192;

// 函数用于读取文件并将其分割成4KB的块
std::vector<std::string> splitFileIntoChunks(const std::string& filename) {
    std::vector<std::string> chunks;
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return chunks;
    }

    // 获取文件大小
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 读取文件并分割成块
    std::string chunk;
    while (size > 0) {
        chunk.resize(std::min(chunkSize, static_cast<size_t>(size)));
        file.read(&chunk[0], chunkSize);
        size -= chunkSize;
        chunks.push_back(chunk);
    }

    return chunks;
}

// 函数用于将块按照指定顺序重组并写入新文件
void redata(const std::vector<std::string>& chunks, const std::string& outputFilename, const std::vector<size_t>& order) {
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "无法创建输出文件" << std::endl;
        return;
    }

    // 按照指定顺序写入块
    for (size_t i : order) {
        if (i < chunks.size()) {
            outputFile.write(chunks[i].c_str(), chunks[i].size());
        }
    }
}

int main() {
    std::string inputFileName = "../dataset/calgary/bib"; // 输入文件名
    std::string outputFileName = "../dataset/calgary/rebib"; // 输出文件名

    // 读取文件并分割成4KB的块
    std::vector<std::string> chunks = splitFileIntoChunks(inputFileName);

    // 指定块的重组顺序
    std::vector<size_t>sx = {0,1,4,5,6,8,9,10,31,11,12,2,3,7}; // 例如，按照这个顺序重组块

    // 重组文件
    redata(chunks, outputFileName, sx);

    return 0;
}