import os
import numpy as np
import lz4.block
from collections import Counter
from math import log2

# 常量定义
BLOCK_SIZE = 4096  # 4KB数据块
COMPRESS_LEVEL = 1  # LZ4压缩级别
SILESIA_PATH = "/home/huzhiyang/dataset/silesia"  # Silesia数据集路径

def calculate_entropy_old(data):
    """计算数据块的熵值（与之前保持一致）"""
    if len(data) == 0:
        return 0.0
    
    counts = Counter(data)
    probs = [count / len(data) for count in counts.values()]
    
    entropy = -sum(p * log2(p) for p in probs)
    return entropy

def process_dataset_by_block(dataset_path):
    """处理Silesia数据集 修改压缩算法部分"""
    ent_array = []
    cr_array = []
    
    for filename in os.listdir(dataset_path):
        filepath = os.path.join(dataset_path, filename)
        if not os.path.isfile(filepath):
            continue
            
        with open(filepath, "rb") as f:
            while True:
                block = f.read(BLOCK_SIZE)
                if not block:
                    break
                
                # 计算熵值（保持10%采样）
                # sample_size = max(1, len(block)//10)
                # sampled_data = block[:sample_size]
                # entropy = calculate_entropy(sampled_data)

                # 计算熵值（保持100%采样）
                sample_size = len(block)
                sampled_data = block[:sample_size]
                entropy = calculate_entropy(sampled_data)
                
                # 使用LZ4进行压缩
                compressed = lz4.block.compress(block)
                cr = len(compressed) / len(block)
                
                ent_array.append(entropy)
                cr_array.append(cr)
                
    return np.array(ent_array), np.array(cr_array)

def calculate_entropy(data):
    # 将字节串转换为 NumPy 数组，并视为无符号 8 位整数
    data_array = np.frombuffer(data, dtype=np.uint8)
    # 计算数据的熵值
    if data_array.size == 0:
        return 0.0
    byte_counts = np.bincount(data_array)
    probabilities = byte_counts / len(data_array)
    entropy = -np.sum(probabilities * np.log2(probabilities + 1e-12))
    return entropy

def process_dataset(dataset_path):
    """处理Silesia数据集,按文件粒度计算文件数据熵和文件压缩比"""
    ent_array = []
    cr_array = []
    
    for filename in os.listdir(dataset_path):
        filepath = os.path.join(dataset_path, filename)
        if not os.path.isfile(filepath):
            continue
        
        # 读取整个文件内容
        with open(filepath, "rb") as f:
            data = f.read()
        
        # 计算文件的熵值
        entropy = calculate_entropy(data)
        
        # 使用LZ4进行压缩
        compressed = lz4.block.compress(data)
        cr = len(compressed) / len(data)
        
        ent_array.append(entropy)
        cr_array.append(cr)
                
    return np.array(ent_array), np.array(cr_array)

def train_model(ent_array, cr_array):
    """训练模型（保持相同逻辑）"""
    valid_idx = (cr_array > 0) & (cr_array <= 1)
    ent_array = ent_array[valid_idx]
    cr_array = cr_array[valid_idx]
    
    coeffs = np.polyfit(ent_array, np.log(cr_array), 1)
    a, b = coeffs
    return a, b

class LZ4CompressionPredictor:
    """LZ4专用压缩预测器"""
    def __init__(self, a, b, threshold=0.9):
        self.a = a
        self.b = b
        self.threshold = threshold
        
    def predict_cr(self, entropy):
        """预测压缩率"""
        log_cr = self.a * entropy + self.b
        cr = np.exp(log_cr) - np.exp(self.b)
        return cr
    
    def should_compress(self, entropy, prev_compressible=False):
        """优化决策逻辑"""
        if prev_compressible:
            return True
            
        if entropy < 2.0:  # 针对LZ4优化低熵阈值
            return True
            
        predicted_cr = self.predict_cr(entropy)
        return predicted_cr < self.threshold

# 示例使用
if __name__ == "__main__":
    # 处理数据集（使用LZ4）
    ent_array, cr_array = process_dataset(SILESIA_PATH)

    #输出对应的压缩熵跟压缩比

    for ent , cr in zip(ent_array, cr_array):
        print(f"{ent} - {cr}")
    # 训练模型
    a, b = train_model(ent_array, cr_array)
    print(f"LZ4模型参数: a={a:.4f}, b={b:.4f}")
    
    # 初始化预测器
    predictor = LZ4CompressionPredictor(a, b)
    
    # 测试预测
    test_entropies = [1.5, 3.0, 4.5]
    for ent in test_entropies:
        cr = predictor.predict_cr(ent)
        decision = predictor.should_compress(ent)
        print(f"熵值 {ent:.1f} → 预测CR: {cr:.2f} → 压缩建议: {decision}")

    # 性能验证
    sample_block = os.urandom(BLOCK_SIZE)  # 生成测试数据
    compressed = lz4.block.compress(sample_block)
    print(f"\n示例压缩性能:原始大小: {BLOCK_SIZE} → 压缩后: {len(compressed)}")