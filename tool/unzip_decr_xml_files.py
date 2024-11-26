# -*- coding: utf-8 -*-
# @Time    : 2024/6/25 22:12
# @Author  : SourDumplings
# @Email   : changzheng300@foxmail.com
# @File    : unzip_decr_xml_files.py

import os
import zlib
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad
import xml.etree.ElementTree as ET


def decrypt_and_decompress_file(input_path, output_path, key):
    # 读取加密的文件
    with open(input_path, 'rb') as f:
        file_content = f.read()

    # 分离初始化向量和加密的数据
    iv = file_content[:AES.block_size]
    encrypted_data = file_content[AES.block_size:]

    # 解密数据
    cipher = AES.new(key, AES.MODE_CBC, iv)
    decrypted_data = unpad(cipher.decrypt(encrypted_data), AES.block_size)

    # 解压缩数据
    decompressed_data = zlib.decompress(decrypted_data)

    # 保存解压缩后的数据到输出路径
    with open(output_path, 'wb') as f:
        f.write(decompressed_data)


def process_encrypted_directory(input_directory, output_directory, key):
    for root, dirs, files in os.walk(input_directory):
        for file in files:
            if file.endswith('.scgf'):
                input_path = os.path.join(root, file)
                relative_path = os.path.relpath(root, input_directory)
                output_path = os.path.join(output_directory, relative_path,
                                           file.replace('.scgf', '.xml'))
                os.makedirs(os.path.dirname(output_path), exist_ok=True)

                # 解密和解压缩文件
                decrypt_and_decompress_file(input_path, output_path, key)
                print(
                    f"Decrypted and decompressed {input_path} to {output_path}")


def main():
    project_base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    key_conf_path = os.path.join(project_base_dir, 'conf/key.xml')
    key_conf_tree = ET.parse(key_conf_path)
    key_root = key_conf_tree.getroot()
    key = key_root.find('AES').text # AES密钥，确保是16/24/32字节长度
    print("AES key is: " + key)
    key = key.encode('utf-8')

    input_directory = os.path.join(project_base_dir, "res/dist/scgf/design")
    output_directory = os.path.join(project_base_dir, "conf/design")
    process_encrypted_directory(input_directory, output_directory, key)


if __name__ == "__main__":
    main()