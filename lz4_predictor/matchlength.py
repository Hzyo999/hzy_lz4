import sys

def count_repeated_substrings(file_path):
    # 读取文件内容
    with open(file_path, 'rb') as file:
        data = file.read()

    # 用于存储各长度重复字符串的字典
    repeated_counts = {
        4: {},
        5: {},
        6: {},
        'greater_6': {}
    }

    # 获取数据长度
    data_length = len(data)

    # 遍历所有可能的起始位置和长度
    # for start in range(data_length):
    #     # 检查4字节长度的字符串
    #     if start + 4 <= data_length:
    #         substring = data[start:start+4]
    #         repeated_counts[4][substring] = repeated_counts[4].get(substring, 0) + 1

    #     # 检查5字节长度的字符串
    #     if start + 5 <= data_length:
    #         substring = data[start:start+5]
    #         repeated_counts[5][substring] = repeated_counts[5].get(substring, 0) + 1

    #     # 检查6字节长度的字符串
    #     if start + 6 <= data_length:
    #         substring = data[start:start+6]
    #         repeated_counts[6][substring] = repeated_counts[6].get(substring, 0) + 1

        # 检查大于6字节长度的字符串
        # max_length = min(start + 7, data_length)  # 从7字节开始
        # for length in range(7, max_length + 1):
        #     substring = data[start:start+length]
        #     repeated_counts['greater_6'][substring] = repeated_counts['greater_6'].get(substring, 0) + 1

    for start in range(data_length):
        flag = 0
        # 检查6字节长度的字符串
        if start + 6 <= data_length:
            substring = data[start:start+6]
            substring5 = data[start:start+5]
            substring4 = data[start:start+4]
            count = repeated_counts[6].get(substring, 0) + 1
            repeated_counts[6][substring] = repeated_counts[6].get(substring, 0) + 1
            if(count != 0) :
                repeated_counts[5][substring] = repeated_counts[5].get(substring5, 0) + 1
                repeated_counts[4][substring] = repeated_counts[4].get(substring4, 0) + 1
                flag = 1
            # 没有6字节重复
            if flag == 0 :
                count5 = repeated_counts[5].get(substring5, 0)
                repeated_counts[5][substring] = repeated_counts[5].get(substring5, 0) + 1
                if(count5 != 0) :
                    repeated_counts[4][substring] = repeated_counts[4].get(substring4, 0) + 1
                    flag = 1
                # 没有5字节重复
                if flag == 0 :
                    repeated_counts[4][substring] = repeated_counts[4].get(substring4, 0) + 1

        # 检查5字节长度的字符串
        if start + 6 <= data_length:
            substring6 = data[start:start+6]
            count = repeated_counts[6].get(substring6, 0)
            if count == 0 :
                substring5 = data[start:start+5]
                repeated_counts[5][substring] = repeated_counts[5].get(substring5, 0) + 1

    # for start in range(data_length):
    #     # 检查5字节长度的字符串
    #     if start + 6 <= data_length:
    #         substring6 = data[start:start+6]
    #         count = repeated_counts[6].get(substring6, 0)
    #         if count == 0 :
    #             substring5 = data[start:start+5]
    #             repeated_counts[5][substring] = repeated_counts[5].get(substring5, 0) + 1

    # for start in range(data_length):
    #     # 检查4字节长度的字符串
    #     if start + 5 <= data_length:
    #         substring5 = data[start:start+5]
    #         count = repeated_counts[5].get(substring5, 0)
    #         if count == 0 :
    #             substring4 = data[start:start+4]
    #             repeated_counts[4][substring] = repeated_counts[4].get(substring4, 0) + 1

    print_repeated_substrings(repeated_counts)
    # 统计重复次数大于1的字符串数量
    result = {
        '4_byte': sum(1 for count in repeated_counts[4].values() if count > 1),
        '5_byte': sum(1 for count in repeated_counts[5].values() if count > 1),
        '6_byte': sum(1 for count in repeated_counts[6].values() if count > 1),
        # 'greater_6_byte': sum(1 for count in repeated_counts['greater_6'].values() if count > 1)
    }

    return result

def print_repeated_substrings(repeated_counts):
    # 打印重复的4字节字符串及其出现的次数
    print("重复的4字节字符串:")
    for substring, count in repeated_counts[4].items():
        if count > 1:
            print(f"字节序列: {substring} , 重复次数: {count}")

    # 打印重复的5字节字符串及其出现的次数
    print("\n重复的5字节字符串:")
    for substring, count in repeated_counts[5].items():
        if count > 1:
            print(f"字节序列: {substring} , 重复次数: {count}")

    # 打印重复的6字节字符串及其出现的次数
    print("\n重复的6字节字符串:")
    for substring, count in repeated_counts[6].items():
        if count > 1:
            print(f"字节序列: {substring} , 重复次数: {count}")

    # 打印重复的长度大于6的字符串及其出现的次数
    # print("\n重复的长度大于6的字符串:")
    # for substring, count in repeated_counts['greater_6'].items():
    #     if count > 1:
    #         print(f"字节序列: {substring.hex()} , 长度: {len(substring)} , 重复次数: {count}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("用法: python script.py <文件路径>")
        sys.exit(1)

    file_path = sys.argv[1]
    result = count_repeated_substrings(file_path)
    print("重复的4字节字符串个数:", result['4_byte'])
    print("重复的5字节字符串个数:", result['5_byte'])
    print("重复的6字节字符串个数:", result['6_byte'])
    # print("重复的长度大于6的字符串个数:", result['greater_6_byte'])