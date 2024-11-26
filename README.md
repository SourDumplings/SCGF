- [SCGF](#scgf)
- [Setup environment](#setup-environment)
  - [Set up dev environment](#set-up-dev-environment)
  - [Clone with submodules](#clone-with-submodules)
  - [Update submodules](#update-submodules)
  - [Set up build environment](#set-up-build-environment)
    - [Windows](#windows)
- [Coding](#coding)
- [Build and test](#build-and-test)
  - [Windows](#windows-1)
    - [Exectue task in VSCode](#exectue-task-in-vscode)
    - [Test in Visual Studio](#test-in-visual-studio)
  - [TODO](#todo)

# SCGF
Smart Cat Game Framework(SCGF)

A simple cross-platform game framework

**ALL OF SCGF IS BUILT FROM SOURCE. NO BINARY EXTERNAL LIB!**

See `.gitmodules`.

# Setup environment
## Set up dev environment
- Git: [git](https://git-scm.com/)
- VSCode: [VSCode](https://code.visualstudio.com/)

## Clone with submodules
```sh
git clone --recursive git@github.com:SourDumplings/SCGF.git
```

## Update submodules
```sh
git submodule update --remote --recursive
```

## Set up build environment
- CMake 3.28+: [cmake](https://cmake.org/)
- Python 3.9+: [python](https://www.python.org/)
  - **Set up your environment variable `python3`.** For example, you can copy `python.exe` as `python3.exe` on windows after installing python3 and set up environment variable `PATH` as usual.
  - pycryptodome

### Windows
SCGF uses MSVC compiling toolset.
- Visual Studio 2022
  - Configure vcpkg path in `CMakeLists.txt`

# Coding
- C++: C++ 20 standard for SCGF src and test
- Python: 3.9+ for SCGF tool
- Lua: 5.4.7+ for SCGF res/script

# Build and test
## Windows
### Exectue task in VSCode
- `Build xxx on Windows`: build, xxx means `CMAKE_BUILD_TYPE`. **Do `Clean all on Windows` task first if you want to switch to another `CMAKE_BUILD_TYPE`, i.e. Debug to Release.**
  - `Build intermediate xxx on Windows`: build intermediate, **You should build intermediate first, then build dist.**
  - `Build dist xxx on Windows`: build dist
- `Run test on Windows`: run `dist/SCGFTest.exe`
- `Clean all on Windows`: clean build、intermediate and dist dir. 
- `Clean build dist on Windows`: clean build and dist dir. 

### Test in Visual Studio
After finishing `Build intermediate xxx on Windows` task in VSCode, you can run test in Visual Studio. Just open SCGF with Visual Studio and select startup item as `SCGF.exe(Install)`.

## TODO
- 待编译器对模块支持完善后全部模块化，目前支持的不是很好
- 优化测试方式，使用单元测试框架，尽量去掉 test 目录
