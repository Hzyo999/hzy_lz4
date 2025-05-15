import numpy as np
import os
import lz4.block
from sklearn.linear_model import LinearRegression
from math import log

# 计算字节熵（保持不变）
def calculate_entropy(data):
    if not data:
        return 0
    entropy = 0
    byte_counts = [0] * 256
    for byte in data:
        byte_counts[byte] += 1
    
    for count in byte_counts:
        if count == 0:
            continue
        p = count / len(data)
        entropy -= p * log(p, 2)
    
    return entropy

# 改用LZ4的压缩率计算
def calculate_cr(data):
    try:
        compressed = lz4.block.compress(data, mode="high_compression")
        return len(compressed) / len(data) if len(data) > 0 else 0
    except Exception as e:
        print(f"LZ4压缩异常: {str(e)}")
        return 1.0  # 返回不可压缩状态

# 数据预处理（适配LZ4特性）
def prepare_dataset(dataset_path, chunk_size=4096):
    ent_array = []
    cr_array = []
    
    for filename in os.listdir(dataset_path):
        filepath = os.path.join(dataset_path, filename)
        if os.path.isfile(filepath):
            with open(filepath, 'rb') as f:
                while True:
                    chunk = f.read(chunk_size)
                    if not chunk:
                        break
                    # 填充不足块保持对齐
                    if len(chunk) < chunk_size:
                        chunk += b'\x00' * (chunk_size - len(chunk))
                    
                    entropy = calculate_entropy(chunk)
                    cr = calculate_cr(chunk)
                    
                    ent_array.append(entropy)
                    cr_array.append(cr)
    
    return np.array(ent_array), np.array(cr_array)

# 模型训练保持不变
def train_model(ent_array, cr_array):
    valid_idx = (cr_array > 0) & (cr_array <= 1)
    X = ent_array[valid_idx].reshape(-1, 1)
    y = np.log(cr_array[valid_idx])
    
    model = LinearRegression().fit(X, y)
    return model.coef_[0], model.intercept_

# 优化后的压缩决策（适配LZ4）
def lz4_compress_with_optimization(data_stream, a, b, threshold=0.9, chunk_size=4096):
    prev_compressible = False
    total_saved = 0
    compress_times = []
    
    for i in range(0, len(data_stream), chunk_size):
        chunk = data_stream[i:i+chunk_size]
        # 填充对齐
        if len(chunk) < chunk_size:
            chunk += b'\x00' * (chunk_size - len(chunk))
        
        if prev_compressible:
            start = time.time()
            compressed = lz4.block.compress(chunk)
            compress_times.append(time.time() - start)
            total_saved += len(chunk) - len(compressed)
            prev_compressible = True
            continue
            
        entropy = calculate_entropy(chunk)
        log_cr = a * entropy + b
        pred_cr = np.exp(log_cr) - np.exp(b)
        
        if pred_cr < threshold:
            start = time.time()
            compressed = lz4.block.compress(chunk)
            compress_times.append(time.time() - start)
            total_saved += len(chunk) - len(compressed)
            prev_compressible = True
        else:
            prev_compressible = False
    
    print(f"平均压缩耗时：{np.mean(compress_times)*1000:.2f}ms")
    return total_saved

# 主程序
if __name__ == "__main__":
    # 配置参数
    SILESIA_PATH = "/path/to/silesia_dataset"
    CHUNK_SIZE = 4096
    
    # 训练LZ4专用模型
    ent_array, cr_array = prepare_dataset(SILESIA_PATH, CHUNK_SIZE)
    a, b = train_model(ent_array, cr_array)
    
    # 测试数据（示例）
    test_data = b"x" * 1024 * 1024  # 1MB测试数据
    saved = lz4_compress_with_optimization(test_data, a, b)
    print(f"LZ4优化节省空间：{saved} bytes")
