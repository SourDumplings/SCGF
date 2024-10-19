@echo off

set project_base_dir=%~dp0\..

if exist "%project_base_dir%\build" (
    rmdir /s /q "%project_base_dir%\build"
) else (
    echo No build directory found to delete.
)

if exist "%project_base_dir%\dist" (
    rmdir /s /q "%project_base_dir%\dist"
) else (
    echo No dist directory found to delete.
)