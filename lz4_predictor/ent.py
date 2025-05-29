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

    # 计算没出现 也就是probability为0数量
    for pro in probabilities:
        cnt=0
        if pro < 0.01 :
            cnt+=1
    

    # 计算熵值
    entropy = -np.sum(probabilities * np.log2(probabilities + 1e-12))
    return entropy

def process_dataset(dataset_path):
    """
    处理 Silesia 数据集，计算每个文件的熵值和压缩比。
    
    参数:
        dataset_path (str): 数据集路径。
        
    返回:
        tuple: 三个列表，分别包含文件名、每个文件的熵值和压缩比。
    """
    filenames = []
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
        
        # 使用 LZ4 进行压缩
        compressed = lz4.block.compress(data)
        cr = len(compressed) / len(data)
        
        filenames.append(filename)
        ent_array.append(entropy)
        cr_array.append(cr)
                
    return filenames, np.array(ent_array), np.array(cr_array)

# 示例使用
if __name__ == "__main__":
    dataset_path = "/home/huzhiyang/dataset/silesia"  # 替换为你的 Silesia 数据集路径
    
    filenames, ent_array, cr_array = process_dataset(dataset_path)
    
    # 打印结果
    for i in range(len(filenames)):
        print(f"File: {filenames[i]}, Entropy: {ent_array[i]:.4f}, Compression Ratio: {cr_array[i]:.4f}")
    
    # 如果需要，可以将结果保存到文件
    np.save("filenames.npy", filenames)
    np.save("entropy_values.npy", ent_array)
    np.save("compression_ratios.npy", cr_array)