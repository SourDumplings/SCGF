import os
import hashlib
import platform
import subprocess

def calculate_md5(file_path):
    """Calculate the MD5 checksum of a specified file"""
    hash_md5 = hashlib.md5()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

def check_file_modification(file_to_monitor, md5_file_dir):
    """Check if the file has been modified"""
    file_modified = False
    md5_file_path = os.path.join(md5_file_dir, os.path.basename(file_to_monitor) + ".md5")
    
    current_md5 = calculate_md5(file_to_monitor)
    
    if os.path.exists(md5_file_path):
        with open(md5_file_path, 'r') as f:
            saved_md5 = f.read().strip()
        
        if current_md5 == saved_md5:
            print(f'"{file_to_monitor}" has not been modified.')
        else:
            print(f'"{file_to_monitor}" has been modified.')
            file_modified = True
            with open(md5_file_path, 'w') as f:
                f.write(current_md5)
    else:
        print(f'Initial generation of MD5 for "{file_to_monitor}".')
        with open(md5_file_path, 'w') as f:
            f.write(current_md5)
        file_modified = True
    
    return file_modified

def generate_pb(protoc_path, pb_path, pb_cpp_out_dir, target_pb_path):
    command = "%s -I=%s --cpp_out=%s %s" % (
            protoc_path,
            pb_path,
            pb_cpp_out_dir,
            target_pb_path,
        )
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode == 0:
        print(result.stdout)
    else:
        print(result.stderr)

def main():
    project_base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    pb_cpp_out_dir = os.path.join(project_base_dir, "intermediate", "generated", "proto", "pb")

    if not os.path.exists(pb_cpp_out_dir):
        os.makedirs(pb_cpp_out_dir)
    
    pb_path = os.path.join(project_base_dir, "proto", "pb")

    scgf_cs_proto = os.path.join(pb_path, "SCGF_CS.proto")
    print(f'Checking {scgf_cs_proto}...')

    if check_file_modification(scgf_cs_proto, pb_cpp_out_dir):
        print("Generating proto...")

        protoc_path = os.path.join(project_base_dir, "intermediate", "bin")
        if platform.system() == "Windows":
            protoc_path = os.path.join(protoc_path, "protoc.exe")
        else:
            print("We dont't support this platform %s yet!" % platform.system())
            exit(-1)

        generate_pb(protoc_path, pb_path, pb_cpp_out_dir, scgf_cs_proto)

        print("Proto generation complete.")
    else:
        print("We don't need to generate proto.")

if __name__ == "__main__":
    main()