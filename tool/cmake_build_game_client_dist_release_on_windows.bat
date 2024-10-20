@echo off
set project_base_dir=%~dp0\..

if exist "%project_base_dir%\dist" (
    rmdir /s /q "%project_base_dir%\dist"
)

python3 %project_base_dir%\tool\generate_proto.py

for /f "delims=" %%i in ('python3 "%project_base_dir%\tool\aes_key_loader.py"') do (
    set "AES_KEY=%%i"
)

if %errorlevel% neq 0 (
    echo Python script failed with exit code %errorlevel%.
    exit /b %errorlevel%
)

echo AES_KEY: %AES_KEY%

set SCGF_AES_KEY=%AES_KEY%
call "%project_base_dir%\tool\cmake_build_on_windows.bat" "Release" "dist" 0
