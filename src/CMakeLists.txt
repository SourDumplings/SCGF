cmake_minimum_required(VERSION 3.28)

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

set(exe_name ${project_name})
if(${BUILD_TARGET} MATCHES "dist")
    if(${DEPLOY_TARGET} MATCHES "Client")
        message(STATUS "deploy target: Game Client")
        set(exe_name ${exe_name}_client)
    elseif(${DEPLOY_TARGET} MATCHES "Server")
        message(STATUS "deploy target: Game Server")
        set(exe_name ${exe_name}_server)
    else()
        message(FATAL_ERROR "deploy target: Unknown")
    endif()
endif()

add_executable(${exe_name} ${scgf_sources} ${pb_cc})

target_link_libraries(${exe_name}
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
    target_link_libraries(${exe_name} PRIVATE spdlogd)
else()
    target_link_libraries(${exe_name} PRIVATE spdlog)
endif()

install(TARGETS ${exe_name} DESTINATION ${dist_bin_dir})

if(MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    # Install pdb
    install(FILES "${PROJECT_BINARY_DIR}/src/Debug/${exe_name}.pdb" DESTINATION ${dist_bin_dir})
endif()