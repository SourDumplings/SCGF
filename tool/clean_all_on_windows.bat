@echo off

set project_base_dir=%~dp0\..

if exist "%project_base_dir%\build" (
    rmdir /s /q "%project_base_dir%\build"
) else (
    echo No build directory found to delete.
)

if exist "%project_base_dir%\intermediate" (
    rmdir /s /q "%project_base_dir%\intermediate"
) else (
    echo No intermediate directory found to delete.
)

if exist "%project_base_dir%\dist" (
    rmdir /s /q "%project_base_dir%\dist"
) else (
    echo No dist directory found to delete.
)

if exist "%project_base_dir%\conf\key.xml" (
    del /s /q "%project_base_dir%\conf\key.xml"
) else (
    echo No conf\key.xml directory found to delete.
)

if exist "%project_base_dir%\res\dist\scgf" (
    rmdir /s /q "%project_base_dir%\res\dist\scgf"
) else (
    echo No res\dist\scgf directory found to delete.
)