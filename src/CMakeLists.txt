cmake_minimum_required(VERSION 3.28)

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

find_package(absl 20240116 REQUIRED)
find_package(easy_profiler 2.1.0 REQUIRED)
find_package(EnTT 3.14.0 REQUIRED)
find_package(protobuf REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${src_dir})
include_directories(${external_dir}/imgui)
include_directories(${external_dir}/SDL/include)
include_directories(${external_dir}/SDL_mixer/include)
include_directories(${external_dir}/SDL_ttf/include)
include_directories(${external_dir}/spdlog/include)
include_directories(${CMAKE_INCLUDE_PATH})
include_directories(${generated_dir})
include_directories(${Protobuf_INCLUDE_DIRS})

link_directories(${CMAKE_LIBRARY_PATH})

file(GLOB_RECURSE
    scgf_headers
    CONFIGURE_DEPENDS
    *.h
)

file(GLOB_RECURSE
    scgf_sources
    CONFIGURE_DEPENDS
    *.cpp
)

file(GLOB_RECURSE
    pb_cc
    CONFIGURE_DEPENDS
    ${generated_dir}/proto/pb/*.pb.cc
)

message(STATUS "Protobuf generated source files: ${pb_cc}")

add_executable(${project_name} ${scgf_sources} ${pb_cc})

target_link_libraries(${project_name}
    PRIVATE 
    absl::base
    absl::strings
    absl::synchronization
    crypto_algorithms_static
    easy_profiler
    gumbo
    imgui
    litehtml
    lua_shared
    protobuf::libprotobuf
    SDL3
    SDL3_mixer
    SDL3_ttf
    stb
    ZLIB::ZLIB
)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_link_libraries(${project_name} PRIVATE spdlogd)
else()
    target_link_libraries(${project_name} PRIVATE spdlog)
endif()

install(TARGETS ${project_name} DESTINATION ${dist_bin_dir})

if(MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    # Install pdb
    install(FILES "${PROJECT_BINARY_DIR}/src/Debug/${project_name}.pdb" DESTINATION ${dist_bin_dir})
endif()