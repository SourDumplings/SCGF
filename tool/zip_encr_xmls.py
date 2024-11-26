import os
import zlib
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
import xml.etree.ElementTree as ET


def encrypt_and_compress_xml_string(xml_str, key):
    # 首先使用zlib压缩XML字符串
    compressed_xml = zlib.compress(xml_str)

    # 使用AES加密压缩后的数据
    cipher = AES.new(key, AES.MODE_CBC)
    encrypted_xml = cipher.encrypt(pad(compressed_xml, AES.block_size))

    return encrypted_xml, cipher.iv


def process_directory(directory, output_directory, key):
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.xml'):
                input_path = os.path.join(root, file)
                # 序列化XML文件
                tree = ET.parse(input_path)
                xml_str = ET.tostring(tree.getroot(), encoding='utf8',
                                      method='xml')

                # 加密并压缩序列化后的XML字符串
                encrypted_compressed_xml, iv = encrypt_and_compress_xml_string(
                    xml_str, key)

                # 构建输出文件路径
                relative_path = os.path.relpath(root, directory)
                output_file_name = file[:-4] + ".scgf"
                output_path = os.path.join(output_directory, relative_path,
                                           output_file_name)
                os.makedirs(os.path.dirname(output_path), exist_ok=True)

                # 保存加密的数据和IV
                with open(output_path, 'wb') as f:
                    f.write(iv + encrypted_compressed_xml)
                print(f"Encrypted and compressed {input_path} to {output_path}")


def main():
    project_base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    key_conf_path = os.path.join(project_base_dir, 'conf/key.xml')
    key_conf_tree = ET.parse(key_conf_path)
    key_root = key_conf_tree.getroot()
    key = key_root.find('AES').text # AES密钥，确保是16/24/32字节长度
    print("AES key is: " + key)
    key = key.encode('utf-8')

    input_directory = os.path.join(project_base_dir, "conf/design")
    output_directory = os.path.join(project_base_dir, "res/dist/scgf/design")
    process_directory(input_directory, output_directory, key)


if __name__ == "__main__":
    main()
