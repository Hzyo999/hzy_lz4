//暴力 重组数据
#include <iostream>
#include <vector>
#include <set>
#include <bitset>
#include <map>
#include <cmath>
#include <algorithm>
#include "compress.h"

//#include "../xdelta3/xdelta3.h"
#define INF 987654321
using namespace std;

int main(int argc, char* argv[]) {
	// if (argc != 5) {
	// 	cerr << "usage: ./lsh_inf [input_file] [window_size] [SF_NUM] [FEATURE_NUM]\n";
	// 	exit(0);
	// }
    //FILE* file = fopen("../dataset/calgary/obj1", "rb");
    FILE* file = fopen(argv[1], "rb");
    // 移动到文件末尾
    fseek(file, 0, SEEK_END);
    // 获取文件大小
    long size = ftell(file);
    size%=BLOCK_SIZE;
    int end_size = size;
    cout<<end_size<<endl;
    fclose(file);
	DATA_IO f(argv[1]);
    //DATA_IO f("../dataset/obj1");
    f.time_check_start();
	f.read_file();
    // f.write_file(f.trace[0], BLOCK_SIZE);
    // f.write_file(f.trace[12], BLOCK_SIZE);
    // f.write_file(f.trace[1], BLOCK_SIZE);
    // f.write_file(f.trace[2], BLOCK_SIZE);
    // f.write_file(f.trace[3], BLOCK_SIZE);
    // f.write_file(f.trace[4], BLOCK_SIZE);
    // f.write_file(f.trace[7], BLOCK_SIZE);
    // f.write_file(f.trace[5], BLOCK_SIZE);
    // f.write_file(f.trace[6], BLOCK_SIZE);
    // f.write_file(f.trace[13], end_size);
    // f.write_file(f.trace[8], BLOCK_SIZE);
    // f.write_file(f.trace[11], BLOCK_SIZE);
    // f.write_file(f.trace[9], BLOCK_SIZE);
    // f.write_file(f.trace[10], BLOCK_SIZE);


    // f.write_file(f.trace[0], BLOCK_SIZE);
    // f.write_file(f.trace[1], BLOCK_SIZE);
    // f.write_file(f.trace[2], BLOCK_SIZE);
    // f.write_file(f.trace[3], BLOCK_SIZE);
    // f.write_file(f.trace[4], BLOCK_SIZE); 
    // f.write_file(f.trace[5], BLOCK_SIZE);
    // f.write_file(f.trace[6], BLOCK_SIZE);
    // f.write_file(f.trace[7], BLOCK_SIZE);
    // f.write_file(f.trace[8], BLOCK_SIZE);
    // f.write_file(f.trace[9], BLOCK_SIZE);
    // f.write_file(f.trace[10], BLOCK_SIZE);
    // f.write_file(f.trace[11], BLOCK_SIZE);
    // f.write_file(f.trace[12], BLOCK_SIZE);
    // f.write_file(f.trace[13], end_size);
    cout<<"ok"<<endl;
    
    f.write_file(f.trace[0], BLOCK_SIZE);
    f.write_file(f.trace[1], BLOCK_SIZE);
    
    f.write_file(f.trace[2], BLOCK_SIZE);
    
    f.write_file(f.trace[3], BLOCK_SIZE);
    
    f.write_file(f.trace[4], BLOCK_SIZE); 
    f.write_file(f.trace[5], BLOCK_SIZE);
    f.write_file(f.trace[7], BLOCK_SIZE);
    f.write_file(f.trace[6], BLOCK_SIZE);
    
    f.write_file(f.trace[8], BLOCK_SIZE);
    f.write_file(f.trace[9], end_size);
    cout<<"ok"<<endl;
	//f.recipe_write();
	cout << "Total time: " << f.time_check_end() << "us\n";

}