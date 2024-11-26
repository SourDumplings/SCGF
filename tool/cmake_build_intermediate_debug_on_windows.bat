@echo off
set project_base_dir=%~dp0\..

call "%project_base_dir%\tool\cmake_build_on_windows.bat" "Debug" "intermediate" 1 "one"

call "%project_base_dir%\tool\cmake_build_on_windows.bat" "Debug" "intermediate" 1 "two"
