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
        '7_and_greater': {}
    }

    # 用于存储所有长度字符串的字典
    all_substrings = {
        4: {},
        5: {},
        6: {},
        '7_and_greater': {}
    }

    # 获取数据长度
    data_length = len(data)

    # 遍历所有可能的起始位置和长度
    for start in range(data_length):
        # 检查4字节长度的字符串
        if start + 4 <= data_length:
            substring = data[start:start+4]
            repeated_counts[4][substring] = repeated_counts[4].get(substring, 0) + 1
            all_substrings[4][substring] = all_substrings[4].get(substring, 0) + 1

        # 检查5字节长度的字符串
        if start + 5 <= data_length:
            substring = data[start:start+5]
            repeated_counts[5][substring] = repeated_counts[5].get(substring, 0) + 1
            all_substrings[5][substring] = all_substrings[5].get(substring, 0) + 1

        # 检查6字节长度的字符串
        if start + 6 <= data_length:
            substring = data[start:start+6]
            repeated_counts[6][substring] = repeated_counts[6].get(substring, 0) + 1
            all_substrings[6][substring] = all_substrings[6].get(substring, 0) + 1

        # 检查7字节及以上的字符串
        max_length = 20
        for length in range(7, max_length + 1):
            substring = data[start:start+length]
            repeated_counts['7_and_greater'][substring] = repeated_counts['7_and_greater'].get(substring, 0) + 1
            all_substrings['7_and_greater'][substring] = all_substrings['7_and_greater'].get(substring, 0) + 1

    # 筛选符合条件的字符串
    valid_repeated_4_bytes = {}  # 重复的4字节，且其5字节不重复
    valid_repeated_5_bytes = {}  # 重复的5字节，且其6字节不重复
    valid_repeated_6_bytes = {}  # 重复的6字节，且其7字节不重复
    valid_repeated_7_and_greater = {}  # 重复的7字节及以上

    # 检查4字节字符串
    for substring, count in repeated_counts.get(4, {}).items():
        if count > 1:
            # 尝试构造5字节字符串
            start_index = data.find(substring)
            if start_index + 5 <= data_length:
                five_byte_substring = data[start_index:start_index+5]
                if all_substrings.get(5, {}).get(five_byte_substring, 0) == 1:
                    valid_repeated_4_bytes[substring] = count

    # 检查5字节字符串
    for substring, count in repeated_counts.get(5, {}).items():
        if count > 1:
            start_index = data.find(substring)
            if start_index + 6 <= data_length:
                six_byte_substring = data[start_index:start_index+6]
                if all_substrings.get(6, {}).get(six_byte_substring, 0) == 1:
                    valid_repeated_5_bytes[substring] = count

    # 检查6字节字符串
    for substring, count in repeated_counts.get(6, {}).items():
        if count > 1:
            start_index = data.find(substring)
            if start_index + 7 <= data_length:
                seven_byte_substring = data[start_index:start_index+7]
                # 检查7字节字符串是否在7字节及以上的字典中且只出现一次
                if all_substrings['7_and_greater'].get(seven_byte_substring, 0) == 1:
                    valid_repeated_6_bytes[substring] = count

    # 检查7字节及以上的字符串
    for substring, count in repeated_counts['7_and_greater'].items():
        if count > 1:
            valid_repeated_7_and_greater[substring] = count

    return {
        '4_byte': valid_repeated_4_bytes,
        '5_byte': valid_repeated_5_bytes,
        '6_byte': valid_repeated_6_bytes,
        '7_and_greater': valid_repeated_7_and_greater
    }

def print_repeated_substrings(repeated_counts):
    # 打印重复的4字节字符串及其出现的次数
    if repeated_counts['4_byte']:
        print("符合条件的重复4字节字符串:")
        for substring, count in repeated_counts['4_byte'].items():
            print(f"字节序列: {substring} , 重复次数: {count}")
    else:
        print("没有符合条件的重复4字节字符串。")

    # 打印重复的5字节字符串及其出现的次数
    if repeated_counts['5_byte']:
        print("\n符合条件的重复5字节字符串:")
        for substring, count in repeated_counts['5_byte'].items():
            print(f"字节序列: {substring} , 重复次数: {count}")
    else:
        print("\n没有符合条件的重复5字节字符串。")

    # 打印重复的6字节字符串及其出现的次数
    if repeated_counts['6_byte']:
        print("\n符合条件的重复6字节字符串:")
        for substring, count in repeated_counts['6_byte'].items():
            print(f"字节序列: {substring} , 重复次数: {count}")
    else:
        print("\n没有符合条件的重复6字节字符串。")

    # 打印重复的7字节及以上字符串及其出现的次数
    if repeated_counts['7_and_greater']:
        print("\n重复的7字节及以上字符串:")
        for substring, count in repeated_counts['7_and_greater'].items():
            print(f"字节序列: {substring} , 长度: {len(substring)} , 重复次数: {count}")
    else:
        print("\n没有重复的7字节及以上字符串。")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("用法: python script.py <文件路径>")
        sys.exit(1)

    file_path = sys.argv[1]
    repeated_counts = count_repeated_substrings(file_path)
    print_repeated_substrings(repeated_counts)
    print(f"\n符合条件的重复4字节字符串个数: {len(repeated_counts.get('4_byte', {}))}")
    print(f"符合条件的重复5字节字符串个数: {len(repeated_counts.get('5_byte', {}))}")
    print(f"符合条件的重复6字节字符串个数: {len(repeated_counts.get('6_byte', {}))}")
    print(f"重复的7字节及以上字符串个数: {len(repeated_counts.get('7_and_greater', {}))}")


# import sys

# def count_repeated_substrings(file_path):
#     # 读取文件内容
#     with open(file_path, 'rb') as file:
#         data = file.read()

#     # 用于存储各长度重复字符串的字典
#     repeated_counts = {
#         4: {},
#         5: {},
#         6: {},
#         7: {}
#     }

#     # 用于存储所有长度字符串的字典
#     all_substrings = {
#         4: {},
#         5: {},
#         6: {},
#         7: {}
#     }

#     # 获取数据长度
#     data_length = len(data)

#     # 遍历所有可能的起始位置和长度
#     for start in range(data_length):
#         # 检查4字节长度的字符串
#         if start + 4 <= data_length:
#             substring = data[start:start+4]
#             repeated_counts[4][substring] = repeated_counts[4].get(substring, 0) + 1
#             all_substrings[4][substring] = all_substrings[4].get(substring, 0) + 1

#         # 检查5字节长度的字符串
#         if start + 5 <= data_length:
#             substring = data[start:start+5]
#             repeated_counts[5][substring] = repeated_counts[5].get(substring, 0) + 1
#             all_substrings[5][substring] = all_substrings[5].get(substring, 0) + 1

#         # 检查6字节长度的字符串
#         if start + 6 <= data_length:
#             substring = data[start:start+6]
#             repeated_counts[6][substring] = repeated_counts[6].get(substring, 0) + 1
#             all_substrings[6][substring] = all_substrings[6].get(substring, 0) + 1

#         # 检查7字节长度的字符串
#         if start + 7 <= data_length:
#             substring = data[start:start+7]
#             repeated_counts[7][substring] = repeated_counts[7].get(substring, 0) + 1
#             all_substrings[7][substring] = all_substrings[7].get(substring, 0) + 1

#     # 筛选符合条件的字符串
#     valid_repeated_4_bytes = {}  # 重复的4字节，且其5字节不重复
#     valid_repeated_5_bytes = {}  # 重复的5字节，且其6字节不重复
#     valid_repeated_6_bytes = {}  # 重复的6字节，且其7字节不重复
#     valid_repeated_7_bytes = {}  # 重复的7字节

#     # 检查4字节字符串
#     for substring, count in repeated_counts[4].items():
#         if count > 1:
#             # 尝试构造5字节字符串
#             start_index = data.find(substring)
#             if start_index + 5 <= data_length:
#                 five_byte_substring = data[start_index:start_index+5]
#                 if all_substrings[5].get(five_byte_substring, 0) == 1:
#                     valid_repeated_4_bytes[substring] = count

#     # 检查5字节字符串
#     for substring, count in repeated_counts[5].items():
#         if count > 1:
#             start_index = data.find(substring)
#             if start_index + 6 <= data_length:
#                 six_byte_substring = data[start_index:start_index+6]
#                 if all_substrings[6].get(six_byte_substring, 0) == 1:
#                     valid_repeated_5_bytes[substring] = count

#     # 检查6字节字符串
#     for substring, count in repeated_counts[6].items():
#         if count > 1:
#             start_index = data.find(substring)
#             if start_index + 7 <= data_length:
#                 seven_byte_substring = data[start_index:start_index+7]
#                 if all_substrings[7].get(seven_byte_substring, 0) == 1:
#                     valid_repeated_6_bytes[substring] = count

#     # 检查7字节字符串
#     for substring, count in repeated_counts[7].items():
#         if count > 1:
#             valid_repeated_7_bytes[substring] = count

#     return {
#         '4_byte': valid_repeated_4_bytes,
#         '5_byte': valid_repeated_5_bytes,
#         '6_byte': valid_repeated_6_bytes,
#         '7_byte': valid_repeated_7_bytes
#     }

# def print_repeated_substrings(repeated_counts):
#     # 打印重复的4字节字符串及其出现的次数
#     print("符合条件的重复4字节字符串:")
#     for substring, count in repeated_counts['4_byte'].items():
#         print(f"字节序列: {substring} , 重复次数: {count}")

#     # 打印重复的5字节字符串及其出现的次数
#     print("\n符合条件的重复5字节字符串:")
#     for substring, count in repeated_counts['5_byte'].items():
#         print(f"字节序列: {substring} , 重复次数: {count}")

#     # 打印重复的6字节字符串及其出现的次数
#     print("\n符合条件的重复6字节字符串:")
#     for substring, count in repeated_counts['6_byte'].items():
#         print(f"字节序列: {substring} , 重复次数: {count}")

#     # 打印重复的7字节字符串及其出现的次数
#     print("\n重复的7字节字符串:")
#     for substring, count in repeated_counts['7_byte'].items():
#         print(f"字节序列: {substring} , 重复次数: {count}")


# def get_repeated_counts(repeated_counts):
#     # 计算重复的字符串数量
#     result = {
#         '4_byte': sum(1 for count in repeated_counts[4].values() if count > 1),
#         '5_byte': sum(1 for count in repeated_counts[5].values() if count > 1),
#         '6_byte': sum(1 for count in repeated_counts[6].values() if count > 1),
#         '7_byte': sum(1 for count in repeated_counts[7].values() if count > 1)
#     }
#     return result

# if __name__ == "__main__":
#     if len(sys.argv) != 2:
#         print("用法: python script.py <文件路径>")
#         sys.exit(1)

#     file_path = sys.argv[1]
#     repeated_counts = count_repeated_substrings(file_path)
#     result = get_repeated_counts(repeated_counts)
#     # print_repeated_substrings(repeated_counts)

#     print("\n统计结果:")
#     print(f"重复的4字节字符串个数: {result['4_byte']}")
#     print(f"重复的5字节字符串个数: {result['5_byte']}")
#     print(f"重复的6字节字符串个数: {result['6_byte']}")
#     print(f"重复的长度大于6的字符串个数: {result['greater_6_byte']}")

#     # print(f"\n符合条件的重复4字节字符串个数: {len(repeated_counts['4_byte'])}")
#     # print(f"符合条件的重复5字节字符串个数: {len(repeated_counts['5_byte'])}")
#     # print(f"符合条件的重复6字节字符串个数: {len(repeated_counts['6_byte'])}")
#     # print(f"重复的7字节字符串个数: {len(repeated_counts['7_byte'])}")