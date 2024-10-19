@echo off
set project_base_dir=%~dp0\..

call "%project_base_dir%\tool\cmake_build_on_windows.bat" "Debug" "intermediate" 0

@REM echo Cleaning the build directory...
@REM rmdir /s /q "%project_base_dir%\build"
