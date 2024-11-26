import os
import zlib
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
import xml.etree.ElementTree as ET

def encrypt_and_compress_file(input_path, output_path, key):
    # 读取文件内容
    with open(input_path, 'rb') as f:
        file_content = f.read()

    # 压缩文件内容
    compressed_content = zlib.compress(file_content)

    # 生成随机的IV
    iv = os.urandom(16)

    # 使用AES加密压缩后的数据
    cipher = AES.new(key, AES.MODE_CBC, iv)
    encrypted_content = cipher.encrypt(pad(compressed_content, AES.block_size))

    # 保存加密的数据和IV
    with open(output_path, 'wb') as f:
        f.write(iv + encrypted_content)

    print(f"Encrypted and compressed {input_path} to {output_path}")

def process_directory(directory, output_directory, key):
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.lua'):
                input_path = os.path.join(root, file)
                relative_path = os.path.relpath(root, directory)
                output_file_name = file.replace(".lua", ".scgf")
                output_path = os.path.join(output_directory, relative_path, output_file_name)
                os.makedirs(os.path.dirname(output_path), exist_ok=True)
                encrypt_and_compress_file(input_path, output_path, key)

def main():
    project_base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    key_conf_path = os.path.join(project_base_dir, 'conf/key.xml')
    key_conf_tree = ET.parse(key_conf_path)
    key_root = key_conf_tree.getroot()
    key = key_root.find('AES').text # AES密钥，确保是16/24/32字节长度
    print("AES key is: " + key)
    key = key.encode('utf-8')

    input_directory = os.path.join(project_base_dir, "res/script")
    output_directory = os.path.join(project_base_dir, "res/dist/scgf/script")
    process_directory(input_directory, output_directory, key)

if __name__ == "__main__":
    main()