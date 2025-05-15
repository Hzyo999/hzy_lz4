
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <bitset>
#include <map>
#include <cmath>
#include <algorithm>
#include "lz4.h"
#include "lz4batch.h"
#include "xxhash.h"
#include "../lz4o/lib/lz4frame.h"

void splitFile(const std::string& sourcePath, int chunkSize) {
    std::ifstream sourceFile(sourcePath, std::ios::binary);
    if (!sourceFile.is_open()) {
        std::cerr << "Cannot open the source file." << std::endl;
        return;
    }
    char* compressed = new char[chunkSize*2];
    char* src = new char[chunkSize];
    long long count = 0;
    long long count2 = 0;
    long long total = 0;

    while (sourceFile) {
        // 读取数据至缓冲区
        sourceFile.read(src, chunkSize);
        // 获取读取的字节数
        std::streamsize bytes = sourceFile.gcount();
        if (bytes <= 0) break;
        total+=bytes;

        //size_t const bufferSize =chunkSize;
        //LZ4F_preferences_t prefs = LZ4F_INIT_PREFERENCES;

        // 设置块大小
        //prefs.frameInfo.blockSizeID = LZ4F_max4MB;  // 可选的值有LZ4F_default, LZ4F_max64KB, LZ4F_max256KB, LZ4F_max1MB, LZ4F_max4MB
        //count +=LZ4F_compressFrame(compressed, LZ4F_compressFrameBound(bufferSize, &prefs), src, bufferSize, &prefs);
        count += LZ4_compress_default(src, compressed,bytes,2*chunkSize);  
             
    }
    std::cout<<"Chunksize : "<<chunkSize<<" comp_ratio : "<<(double)total / count<<std::endl;

    delete[] src;
    sourceFile.close();
}



int main(int argc, char* argv[]) {
    std::string sourceFilePath = argv[1];
    int sizetable[]={2048,4096,8192,1<<14,1<<15,1<<16,1<<17,1<<18,1<<19,1<<20,1<<21};
    int cnt=11;
    for(int i=0;i<cnt;i++){
        int chunkSize = sizetable[i]; // 比如每个小文件1KB大小
        splitFile(sourceFilePath, chunkSize);
    }
    // for(int i=0;i<cnt;i++){
    //     int chunkSize = sizetable[i]; // 比如每个小文件1KB大小
    //     splitFile2(sourceFilePath, chunkSize);
    // }
    

    return 0;
}
