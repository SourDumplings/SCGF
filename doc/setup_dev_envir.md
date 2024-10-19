- [Setup environment](#setup-environment)
  - [Set up dev environment](#set-up-dev-environment)
  - [Clone with submodules](#clone-with-submodules)
  - [Update submodules](#update-submodules)
  - [Set up build environment](#set-up-build-environment)
    - [Windows](#windows)
- [Coding](#coding)
- [Build and test](#build-and-test)
  - [Windows](#windows-1)


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
SCGF uses Clang 19.1.0 compiling toolset.

- CMake 3.28+: [cmake](https://cmake.org/)
- Python 3.9+: [python](https://www.python.org/)
  - **Set up your environment variable `python3`.** For example, you can copy `python.exe` as `python3.exe` on windows after installing python3 and set up environment variable `PATH` as usual.
  - pycryptodome

### Windows
- llvm-mingw: [llvm-mingw](https://github.com/mstorsjo/llvm-mingw/releases/download/20240917/llvm-mingw-20240917-msvcrt-x86_64.zip)
  - Set `/bin` sub dir into `PATH` environment variable
  - Configure clang compiler path in `.vscode/c_cpp_properties.json`
- MinGW-w64: [MinGW-w64 - winlibs standalone build](https://github.com/brechtsanders/winlibs_mingw/releases/download/14.2.0posix-19.1.1-12.0.0-msvcrt-r2/winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64msvcrt-12.0.0-r2.7z)
  - Set `/bin` sub dir into `PATH` environment variable
- Visual Studio 2022
  - Configure vcpkg path in `CMakeLists.txt`

# Coding
- C++: C++ 20 standard for SCGF src and test
- Python: 3.9+ for SCGF tool
- Lua: 5.4.4+ for SCGF res/script

# Build and test
## Windows
Exectue task in VSCode:
- `Build xxx on Windows`: build
  - `Build intermediate xxx on Windows`: build intermediate, **You should build intermediate first, then build dist.**
  - `Build dist xxx on Windows`: build dist
- `Run test on Windows`: run `dist/SCGFTest.exe`
- `Clean on Windows`: clean build and dist dir