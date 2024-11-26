import os
import xml.etree.ElementTree as ET
import zipfile

def zip_excluding_directories(root_dir, exclude_subdirs, output_filename):
    files_to_zip = []
    for root, _, files in os.walk(root_dir):
        # Determine if the current directory should be excluded
        exclude = False
        for subdir in exclude_subdirs:
            subdir_path = os.path.join(root_dir, subdir)
            if os.path.commonpath([root, subdir_path]) == subdir_path:
                exclude = True
                break
        
        if exclude:
            continue

        for file in files:
            file_path = os.path.join(root, file)
            arcname = os.path.relpath(file_path, start=root_dir)
            if arcname in exclude_subdirs:
                continue

            files_to_zip.append((file_path, arcname))

    total_files = len(files_to_zip)
    with zipfile.ZipFile(output_filename, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for i, (file_path, arcname) in enumerate(files_to_zip, start=1):
            zipf.write(file_path, arcname)
            progress = (i / total_files) * 100
            print(f'Progress: {progress:.2f}%', end='\r')
    print()  # Ensure the next print starts on a new line

def main():
    project_base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    project_base_conf_path = os.path.join(project_base_dir, 'conf/SCGF.xml')

    # spliter is \\
    exclude_subdirs = [
        '.git',
        '.vscode\\settings.json',
        'build',
        'dist',
        'intermediate',
        '.gitignore',
        '.gitmodules'
    ]

    tree = ET.parse(project_base_conf_path)
    root = tree.getroot()
    project_name = root.find('project_name').text
    version = root.find('version').text

    zip_filename = f'{project_name}_{version}.zip'
    zip_filepath = os.path.join(project_base_dir, zip_filename)
    zip_excluding_directories(project_base_dir, exclude_subdirs, zip_filepath)
    print(f'Project {project_name} has been zipped into {zip_filepath}, excluding {exclude_subdirs}')

if __name__ == '__main__':
    main()