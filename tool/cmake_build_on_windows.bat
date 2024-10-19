@echo off
set project_base_dir=%~dp0\..

echo CMAKE_BUILD_TYPE: %1
echo BUILD_TARGET: %2
echo DEPLOY_TARGET: %3

if not exist "%project_base_dir%\build" (
    mkdir "%project_base_dir%\build"
)

cd "%project_base_dir%\build"

cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=%1 -DBUILD_TARGET=%2 -DDEPLOY_TARGET=%3
cmake --build . --config %1 --target install -j8 -v

cd ..