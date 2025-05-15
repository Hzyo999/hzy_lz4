import numpy as np
from sklearn.metrics import confusion_matrix

def evaluate_fp_fn(test_data, a, b, threshold=0.9, chunk_size=4096):
    y_true = []
    y_pred = []
    
    for i in range(0, len(test_data), chunk_size):
        chunk = test_data[i:i+chunk_size]
        if len(chunk) < chunk_size:
            chunk += b'\x00' * (chunk_size - len(chunk))
        
        # 真实标签
        actual_cr = calculate_cr(chunk)  # 使用之前定义的LZ4压缩率计算
        y_true.append(1 if actual_cr < threshold else 0)
        
        # 模型预测
        entropy = calculate_entropy(chunk)
        log_cr = a * entropy + b
        pred_cr = np.exp(log_cr) - np.exp(b)
        y_pred.append(1 if pred_cr < threshold else 0)
    
    # 生成混淆矩阵
    tn, fp, fn, tp = confusion_matrix(y_true, y_pred).ravel()
    
    # 关键指标计算
    metrics = {
        "FP_count": fp,
        "FN_count": fn,
        "FP_rate": fp / (fp + tn),  # 假阳性率
        "FN_rate": fn / (fn + tp),  # 假阴性率
        "Precision": tp / (tp + fp),
        "Recall": tp / (tp + fn)
    }
    return metrics
