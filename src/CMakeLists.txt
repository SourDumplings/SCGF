cmake_minimum_required(VERSION 3.28)

if(NOT DEFINED ENV{SCGF_AES_KEY})
    message(FATAL_ERROR "SCGF_AES_KEY is not defined")
endif()

add_compile_definitions(SCGF_AES_KEY="$ENV{SCGF_AES_KEY}")

include_directories(${src_dir})
include_directories(${external_dir}/imgui)
include_directories(${external_dir}/SDL/include)
include_directories(${external_dir}/SDL_mixer/include)
include_directories(${external_dir}/SDL_ttf/include)
include_directories(${external_dir}/spdlog/include)
include_directories(${CMAKE_INCLUDE_PATH})
include_directories(${generated_dir})

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

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    find_package(easy_profiler REQUIRED)
endif()

find_package(EnTT REQUIRED)

add_executable(scgf ${scgf_sources})

target_link_libraries(scgf
    PRIVATE crypto_algorithms_static
    easy_profiler
    imgui
    litehtml
    lua_shared
    SDL3
    SDL3_mixer
    SDL3_ttf
    stb
)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_link_libraries(scgf PRIVATE spdlogd)
    target_link_libraries(scgf PRIVATE zlibd)
else()
    target_link_libraries(scgf PRIVATE spdlog)
    target_link_libraries(scgf PRIVATE zlib)
endif()

install(TARGETS scgf DESTINATION ${dist_dir})

if(WIN32 AND ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    # Install pdb
    install(FILES "${PROJECT_BINARY_DIR}/src/Debug/scgf.pdb" DESTINATION ${dist_dir})
endif()

