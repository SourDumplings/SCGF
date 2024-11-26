import xml.etree.ElementTree as ET
import os
import sys

def parse_xml(file_path):
    tree = ET.parse(file_path)
    root = tree.getroot()
    key_node = root.find('AES')
    if key_node is not None:
        return key_node.text
    else:
        raise ValueError("No <AES> element found under <Key>")

if __name__ == "__main__":
    project_base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    key_conf_path = os.path.join(project_base_dir, 'conf/key.xml')
    try:
        key_value = parse_xml(key_conf_path)
        print(key_value)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)