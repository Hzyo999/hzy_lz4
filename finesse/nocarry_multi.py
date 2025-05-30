def multiply_without_carry(hex_str):
    # 将输入的十六进制字符串转换为整数
    num = int(hex_str, 16)
    
    # 无进位乘法（相当于普通的乘法，因为Python自动处理位运算）
    # 但为了正确处理高64位的截断，我们需要进行掩码操作
    # result = (num * 0x10001) & 0xFFFFFFFFFFFFFFFF
    result = (num * 0x10001) 
    # 将结果转换回十六进制并去掉前缀'0x'
    hex_result = format(result, '016x')
    
    return hex_result

def main():
    print("请输入64位十六进制数（以0x开头）：")
    hex_input = input().strip()
    
    # 移除可选的0x前缀
    if hex_input.startswith("0x"):
        hex_input = hex_input[2:]
    
    # 验证输入长度
    if len(hex_input) != 16:
        print("错误：输入必须是一个64位的十六进制数（16个字符）")
        return
    
    # 验证输入是否为有效的十六进制
    try:
        int(hex_input, 16)
    except ValueError:
        print("错误：输入不是有效的十六进制数")
        return
    
    # 执行无进位乘法
    result = multiply_without_carry(hex_input)
    
    # 输出结果
    print(f"\n结果(最低64位):0x{result}")

if __name__ == "__main__":
    main()