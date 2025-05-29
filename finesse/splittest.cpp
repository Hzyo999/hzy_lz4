
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <bitset>
#include <map>
#include <cmath>
#include <algorithm>
#include <chrono>

//#include "lz4batch.h"

#include "lz42.h"
//#include "lz4.h"
//#include "../../lz4_origin/lib/lz4hc.h"
//#include "xxhash.h"
//#include "../lz4o/lib/lz4frame.h"

void splitFile(const std::string& sourcePath, int chunkSize) {
    std::ifstream sourceFile(sourcePath, std::ios::binary);
    if (!sourceFile.is_open()) {
        std::cerr << "Cannot open the source file." << std::endl;
        return;
    }
    char* compressed = new char[chunkSize*2];
    char* src = new char[chunkSize];
    long long count = 0;
    long long count_hash4 = 0;
    long long count_hash5 =0;
    long long count_frame = 0 ;
    double totalTime_by_hash5 = 0.0;
    double totalTime_by_hash4 =0.0;
    double totalTime = 0.0;
    long long total = 0;

    while (sourceFile) {
        // 读取数据至缓冲区
        sourceFile.read(src, chunkSize);
        // 获取读取的字节数
        std::streamsize bytes = sourceFile.gcount();
        if (bytes <= 0) break;
        total+=bytes;

        // size_t const bufferSize =chunkSize;
        // LZ4F_preferences_t prefs = LZ4F_INIT_PREFERENCES;

        // // 设置块大小
        // prefs.frameInfo.blockSizeID = LZ4F_max4MB;  // 可选的值有LZ4F_default, LZ4F_max64KB, LZ4F_max256KB, LZ4F_max1MB, LZ4F_max4MB
        // count_frame += LZ4F_compressFrame(compressed, LZ4F_compressFrameBound(bufferSize, &prefs), src, bufferSize, &prefs);

        //count_frame += LZ4_compress_HC(src, compressed, bytes, LZ4_compressBound(bytes), 3);
        const int numberOfTests = 2;  // 测试次数
        std::vector<double> times_hash5 , times_hash4 , times_hash6;  // 存储每次测试的时间

        for (int i = 0; i < numberOfTests; i++) {
            // 开始计时
            auto start = std::chrono::high_resolution_clock::now();

            // 执行函数
            LZ4_compress_default(src, compressed,bytes,2*chunkSize,0);
            //LZ4_compress_default(src, compressed,bytes,2*chunkSize);

            // 结束计时
            auto end = std::chrono::high_resolution_clock::now();

            // 计算执行时间（以秒为单位）
            //double duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
            // 计算执行时间（以微秒为单位）
            double duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            times_hash5.push_back(duration);

            // 开始计时
            start = std::chrono::high_resolution_clock::now();

            // 执行函数
            LZ4_compress_default(src, compressed,bytes,2*chunkSize,1);
            //LZ4_compress_default(src, compressed,bytes,2*chunkSize);

            // 结束计时
             end = std::chrono::high_resolution_clock::now();

            // 计算执行时间（以秒为单位）
            // duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
            // 计算执行时间（以微秒为单位）
            duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            times_hash4.push_back(duration);

            // 开始计时
            start = std::chrono::high_resolution_clock::now();

            // 执行函数
            LZ4_compress_default(src, compressed,bytes,2*chunkSize,2);
            //LZ4_compress_default(src, compressed,bytes,2*chunkSize);

            // 结束计时
             end = std::chrono::high_resolution_clock::now();

            // 计算执行时间（以秒为单位）
            // duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
            // 计算执行时间（以微秒为单位）
            duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            times_hash6.push_back(duration);
        }

        // 计算平均执行时间
        double averageTime_hash6 = 0.0;
        double averageTime_hash5 = 0.0;
        double averageTime_hash4 = 0.0;
        for (double time : times_hash5) {
            averageTime_hash5 += time;
        }
        averageTime_hash5 /= times_hash5.size();

        for (double time : times_hash4) {
            averageTime_hash4 += time;
        }
        averageTime_hash4 /= times_hash4.size();

        double averageTime_hash6 = 0.0;
        for (double time : times_hash6) {
            averageTime_hash6 += time;
        }
        averageTime_hash6 /= times_hash6.size();
        

        // double avgTime_by_hash5 += averageTime_hash5;
        // double avgTime_by_hash5 += averageTime_hash5;
        // double avgTime += averageTime_hash5 < averageTime_hash4 ? averageTime_hash5 : averageTime_hash4;
        long long comp_by_hash5 = LZ4_compress_default(src, compressed,bytes,2*chunkSize,0);
        long long comp_by_hash4 = LZ4_compress_default(src, compressed,bytes,2*chunkSize,1);
        long long comp_by_hash6 = LZ4_compress_default(src, compressed,bytes,2*chunkSize,2);
        count_hash6 += comp_by_hash6;
        // long long comp_by_hash5 = LZ4_compress_default(src, compressed,bytes,2*chunkSize);
        // long long comp_by_hash4 = LZ4_compress_default(src, compressed,bytes,2*chunkSize);
        count_hash5 += comp_by_hash5;
        count_hash4 += comp_by_hash4;
        totalTime_by_hash5 += averageTime_hash5;
        totalTime_by_hash4 += averageTime_hash4;
        int flag = 0;
        // 0 hash4 1 hash5 2 hash6
        if(comp_by_hash5 < comp_by_hash4 && comp_by_hash5 < comp_by_hash6){
            flag = 1 ;
        }else if(comp_by_hash6 < comp_by_hash4 && comp_by_hash6 < comp_by_hash4){
            flag = 2;
        }
        if(flag==0){
            count += comp_by_hash4;
            totalTime += averageTime_hash4;
        }else if(flag == 1){
            count += comp_by_hash5;
            totalTime += averageTime_hash5;
        }else if(falg==2){
            count += comp_by_hash6;
            totalTime += averageTime_hash6;
        }
        // if(comp_by_hash5 < comp_by_hash4 ){
            
        //     count += comp_by_hash5 ;
        //     totalTime += averageTime_hash5;
        // }else{
        //     count += comp_by_hash4 ;
        //     totalTime += averageTime_hash4;
        // }
        // count += comp_by_hash5 < comp_by_hash4 ?  comp_by_hash5 : comp_by_hash4;

             
    }
    double ratio_hash4 = (double)total / count_hash4 ;
    double ratio_hash5 = (double)total / count_hash5 ;
    double ratio_hash6 = (double)total / count_hash6 ;
    double ratio= (double)total / count ;
    double ratio_frame = (double)total / count_frame ;
    double speed_hash4 = (double)total / totalTime_by_hash4;
    double speed_hash5 = (double)total / totalTime_by_hash5;
    double speed_hash6 = (double)total / totalTime_by_hash6;
    double speed= (double)total / totalTime;
    std::cout<<"Chunksize : "<<chunkSize<<std::endl;
    std::cout<<"CR_greed : "<<ratio<<" CR_hash4 : "<<ratio_hash4<<" CR_hash5 : "<<ratio_hash5<<" CR_hash6 : "<<ratio_hash6<<" F_COMP : "<< ratio_frame <<std::endl;
    std::cout<<"SP_greed : "<<speed<<" SP_hash4 : "<<speed_hash4<<" SP_hash5 : "<<speed_hash5<<" SP_hash6 : "<<speed_hash6<<std::endl;
    delete[] src;
    delete[] compressed;
    sourceFile.close();
}



int main(int argc, char* argv[]) {
    std::string sourceFilePath = argv[1];
    int sizetable[]={2048,4096,8192,1<<14,1<<15,1<<16,1<<17,1<<18,1<<19,1<<20,1<<21,1<<22,1<<23};
    int cnt=13;
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
