import os
import numpy as np
import lz4.block

def calculate_entropy(data):
    """
    计算给定数据的熵值。
    
    参数:
        data (bytes): 输入数据（字节串）。
        
    返回:
        float: 数据的熵值。
    """
    # 将字节串转换为 NumPy 数组，并视为无符号 8 位整数
    data_array = np.frombuffer(data, dtype=np.uint8)
    if data_array.size == 0:
        return 0.0
    
    # 计算每个字节的出现次数
    byte_counts = np.bincount(data_array)
    # 计算每个字节的概率分布
    probabilities = byte_counts / len(data_array)
    # 计算熵值
    entropy = -np.sum(probabilities * np.log2(probabilities + 1e-12))
    return entropy
def calculate_threshold_bytes(probabilities, thresholds=[0.8, 0.9]):
    """
    计算需要多少个字节才能使累积概率超过指定的阈值。
    
    参数:
        probabilities (np.ndarray): 概率分布数组。
        thresholds (list): 概率阈值列表（默认为 [0.8, 0.9]）。
        
    返回:
        dict: 每个阈值对应的所需字节数。
    """
    sorted_probabilities = np.sort(probabilities)[::-1]
    cumulative_prob = 0.0
    threshold_bytes = {threshold: 0 for threshold in thresholds}
    
    for i, p in enumerate(sorted_probabilities):
        cumulative_prob += p
        for threshold in thresholds:
            if threshold_bytes[threshold] == 0 and cumulative_prob >= threshold:
                threshold_bytes[threshold] = i + 1  # +1 因为字节从1开始计数
    
    return threshold_bytes

def analyze_probabilities(data):
    """
    分析给定数据的概率分布，返回排名前十的字节及其概率，以及概率小于0.001的字节数。
    
    参数:
        data (bytes): 输入数据（字节串）。
        
    返回:
        tuple: 排名前十的字节及其概率，概率小于0.001的字节数。
    """
    # 将字节串转换为 NumPy 数组，并视为无符号 8 位整数
    data_array = np.frombuffer(data, dtype=np.uint8)
    if data_array.size == 0:
        return [], 0.0, 0
    
    # 计算每个字节的出现次数
    byte_counts = np.bincount(data_array)
    # 计算每个字节的概率分布
    probabilities = byte_counts / len(data_array)
    
    # 获取排名前十的字节及其概率
    top_indices = np.argsort(probabilities)[::-1][:10]
    top_bytes = [(index, probabilities[index]) for index in top_indices]
    # 计算排名前十的字节的总概率
    total_top_prob = np.sum(probabilities[top_indices])
    # 统计概率小于0.001的字节数
    low_prob_count = np.sum(probabilities < 0.001)
    # 统计概率为0的字节数
    zero_prob_count = np.sum(probabilities == 0.0)
    # 计算累积概率阈值所需的字节数
    thresholds = calculate_threshold_bytes(probabilities, thresholds=[0.8, 0.9])
    
    return top_bytes, total_top_prob, low_prob_count, zero_prob_count, thresholds



def process_dataset(dataset_path):
    """
    处理 Silesia 数据集，计算每个文件的熵值、压缩比、排名前十的字节及其概率，以及概率小于0.001的字节数。
    
    参数:
        dataset_path (str): 数据集路径。
        
    返回:
        tuple: 文件名、熵值、压缩比、排名前十的字节及其概率、概率小于0.001的字节数。
    """
    filenames = []
    ent_array = []
    cr_array = []
    top_bytes_array = []
    total_top_prob_array = []
    low_prob_count_array = []
    zero_prob_count_array = []
    threshold_bytes_80 = []
    threshold_bytes_90 = []
    
    # 获取文件夹下所有文件的列表，并取前两个文件
    files = [f for f in os.listdir(dataset_path) if os.path.isfile(os.path.join(dataset_path, f))]
    files_to_process = files[:2]  # 只处理前两个文件
    
    for filename in files:
        filepath = os.path.join(dataset_path, filename)
        
        # 读取整个文件内容
        with open(filepath, "rb") as f:
            data = f.read()
        
        # 计算文件的熵值
        entropy = calculate_entropy(data)
        
        # 分析概率分布
        top_bytes, total_top_prob, low_prob_count, zero_prob_count, thresholds= analyze_probabilities(data)
        
        # 使用 LZ4 进行压缩
        compressed = lz4.block.compress(data)
        cr = len(compressed) / len(data)
        
        filenames.append(filename)
        ent_array.append(entropy)
        cr_array.append(cr)
        top_bytes_array.append(top_bytes)
        total_top_prob_array.append(total_top_prob)
        low_prob_count_array.append(low_prob_count)
        zero_prob_count_array.append(zero_prob_count)
        threshold_bytes_80.append(thresholds[0.8])
        threshold_bytes_90.append(thresholds[0.9])
                
    return (filenames, 
            np.array(ent_array), 
            np.array(cr_array),
            top_bytes_array,
            np.array(total_top_prob_array),
            np.array(low_prob_count_array),
            np.array(zero_prob_count_array),
            np.array(threshold_bytes_80),
            np.array(threshold_bytes_90)
            )

# 示例使用
if __name__ == "__main__":
    dataset_path = "/home/huzhiyang/dataset/silesia"  # 替换为你的 Silesia 数据集路径
    
    filenames, ent_array, cr_array, top_bytes_array, total_top_prob_array, low_prob_count_array, zero_prob_count_array, \
    threshold_bytes_80, threshold_bytes_90= process_dataset(dataset_path)
    
    # 打印结果
    for i in range(len(filenames)):
        top_bytes_str = ", ".join([f"{byte[0]}: {byte[1]:.4f}" for byte in top_bytes_array[i]])
        print(f"File: {filenames[i]}")
        print(f"  Entropy: {ent_array[i]:.4f}")
        print(f"  Compression Ratio: {cr_array[i]:.4f}")
        print(f"  Top 10 Bytes: {top_bytes_str}")
        print(f"  Total Top Probability: {total_top_prob_array[i]:.4f}")
        print(f"  Low Probability Bytes Count: {low_prob_count_array[i]}")
        print(f"  Zero Probability Bytes Count: {zero_prob_count_array[i]}")
        print(f"  Bytes needed for 80% probability: {threshold_bytes_80[i]}")
        print(f"  Bytes needed for 90% probability: {threshold_bytes_90[i]}")
        print()