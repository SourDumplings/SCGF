cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32) # This cannot be MSVC, must be WIN32
    set(CMAKE_C_COMPILER "D:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/cl.exe")
    set(CMAKE_CXX_COMPILER "D:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/cl.exe")

    set(PKG_CONFIG_EXECUTABLE "D:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/vcpkg.exe")

    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi /DEBUG /fsanitize=address /W4 /WX- /utf-8 /bigobj /EHsc")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG /PDB: /INCREMENTAL:NO")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /W4 /WX- /utf-8 /bigobj /EHsc")
    endif()
endif()

file(READ "${CMAKE_SOURCE_DIR}/conf/SCGF.xml" xml_content)
string(REGEX MATCH "<project_name>([^<]+)</project_name>" _match_project_name "${xml_content}")
set(project_name "${CMAKE_MATCH_1}")
string(REGEX MATCH "<version>([^<]+)</version>" _match_version "${xml_content}")
set(scgf_version "${CMAKE_MATCH_1}")

project(${project_name} VERSION ${scgf_version})

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "System Name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "System Version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "System Processor: ${CMAKE_SYSTEM_PROCESSOR}")

message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "CMake generator: ${CMAKE_GENERATOR}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C Compiler ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C Compiler Version: ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "CXX Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

if(WIN32)
    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        message(FATAL_ERROR "SCGF doesn't support to be built on Windows by compiler other than MSVC!")
    endif()
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)

message(STATUS "C++ Compiler Directory: ${COMPILER_DIR}")

set(dist_dir ${PROJECT_SOURCE_DIR}/dist)
set(dist_bin_dir ${PROJECT_SOURCE_DIR}/dist/bin)
set(external_dir ${PROJECT_SOURCE_DIR}/external)
set(src_dir ${PROJECT_SOURCE_DIR}/src)
set(intermediate_dir ${PROJECT_SOURCE_DIR}/intermediate)
set(generated_dir ${intermediate_dir}/generated)
set(intermediate_bin_dir ${intermediate_dir}/bin)

set(CMAKE_INSTALL_PREFIX ${intermediate_dir})
set(CMAKE_PREFIX_PATH ${intermediate_dir})
set(CMAKE_INCLUDE_PATH ${intermediate_dir}/include)
set(CMAKE_LIBRARY_PATH ${intermediate_dir}/lib)

message(STATUS "project name: " ${project_name})
message(STATUS "version: " ${scgf_version})
message(STATUS "build type: " ${CMAKE_BUILD_TYPE})
message(STATUS "build step: " ${BUILD_STEP})

message(STATUS "project dir: " ${PROJECT_SOURCE_DIR})
message(STATUS "source dir: " ${src_dir})
message(STATUS "binary dir: " ${PROJECT_BINARY_DIR})
message(STATUS "external dir: " ${external_dir})
message(STATUS "intermediate dir: " ${intermediate_dir})
message(STATUS "generated dir: " ${generated_dir})
message(STATUS "dist dir: " ${dist_dir})

set(DEPLOY_TARGET_ENUM 0)
if(${BUILD_TARGET} MATCHES "dist")
    if(${DEPLOY_TARGET} MATCHES "Client")
        message(STATUS "deploy target: Game Client")
        set(DEPLOY_TARGET_ENUM 1)
    elseif(${DEPLOY_TARGET} MATCHES "Server")
        message(STATUS "deploy target: Game Server")
        set(DEPLOY_TARGET_ENUM 2)
    else()
        message(FATAL_ERROR "deploy target: Unknown")
    endif()
endif()

add_compile_definitions(PROJECT_BASE_DIR="${PROJECT_SOURCE_DIR}")
add_compile_definitions(CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}")
add_compile_definitions(PROJECT_NAME="${project_name}")
add_compile_definitions(SCGF_VERSION="${scgf_version}")
add_compile_definitions(DEPLOY_TARGET="${DEPLOY_TARGET}")
add_compile_definitions(DEPLOY_TARGET_ENUM=${DEPLOY_TARGET_ENUM})

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_compile_definitions(DEBUG)
endif()
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

if(${BUILD_TARGET} MATCHES "intermediate")
    add_subdirectory(${external_dir})
else()
    if(NOT DEFINED ENV{SCGF_AES_KEY})
        message("ENV SCGF_AES_KEY is not defined, load from ${CMAKE_SOURCE_DIR}/conf/key.xml")
        file(READ "${CMAKE_SOURCE_DIR}/conf/key.xml" xml_content)
        string(REGEX MATCH "<AES>([^<]+)</AES>" _match_aes_key "${xml_content}")
        set(SCGF_AES_KEY "${CMAKE_MATCH_1}")
        add_compile_definitions(SCGF_AES_KEY="${SCGF_AES_KEY}")
    else()
        add_compile_definitions(SCGF_AES_KEY="$ENV{SCGF_AES_KEY}")
    endif()

    execute_process(
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tool/generate_proto.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE result
    )

    if(result)
        message(FATAL_ERROR "Failed to execute generate_proto.py")
    endif()

    add_subdirectory(src)
    add_subdirectory(test)
endif()

if(MSVC)
    if(${BUILD_TARGET} MATCHES "intermediate" AND ${CMAKE_BUILD_TYPE} MATCHES "Debug")
        file(GLOB ASAN_DLLS ${COMPILER_DIR}/clang_rt.asan_*.dll)
        file(GLOB ASAN_PDBS ${COMPILER_DIR}/clang_rt.asan_*.pdb)

        install(FILES ${ASAN_DLLS} DESTINATION  ${intermediate_bin_dir})
        install(FILES ${ASAN_PDBS} DESTINATION  ${intermediate_bin_dir})
    else()
        file(GLOB DIST_DLLS ${intermediate_bin_dir}/*.dll)
        install(FILES ${DIST_DLLS} DESTINATION  ${dist_bin_dir})

        if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
            file(GLOB DIST_PDBS ${intermediate_bin_dir}/*.pdb)
            install(FILES ${DIST_PDBS} DESTINATION  ${dist_bin_dir})
        endif()
    endif()
endif()

if(${BUILD_TARGET} MATCHES "dist")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/conf/dist DESTINATION  ${dist_dir}/conf)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/dist  DESTINATION  ${dist_dir}/doc)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/res/dist  DESTINATION  ${dist_dir}/res)
endif()