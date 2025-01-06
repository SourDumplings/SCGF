@echo off

set project_base_dir=%~dp0\..

if exist "%project_base_dir%\res\dist\scgf" (
    rmdir /s /q "%project_base_dir%\res\dist\scgf"
) else (
    echo No res\dist\scgf directory found to delete.
)

echo "Generating scgf files..."

python3 %project_base_dir%/tool/zip_encr_xmls.py

python3 %project_base_dir%/tool/zip_encr_scripts.py
