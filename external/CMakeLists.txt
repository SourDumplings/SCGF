cmake_minimum_required(VERSION 3.28)

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libs")

if(${BUILD_STEP} MATCHES "one")
    add_subdirectory(zlib)

    if(WIN32 AND ${CMAKE_BUILD_TYPE} MATCHES "Debug")
        # Install pdb
        set(external_bin_dir ${PROJECT_BINARY_DIR}/external)
        install(FILES "${external_bin_dir}/zlib/Debug/zlibd.pdb" DESTINATION ${intermediate_bin_dir})
    endif()
else()
    set(EASY_PROFILER_NO_GUI ON CACHE BOOL "Build easy_profiler without the GUI application (required Qt)")
    set(BUILD_CRYPTO_ALGORITHMS_TEST OFF CACHE BOOL "Don't need build test CRYPTO_ALGORITHMS")
    set(protobuf_BUILD_TESTS OFF CACHE BOOL "Don't need build protobuf test")

    add_subdirectory(crypto-algorithms)
    add_subdirectory(easy_profiler)
    add_subdirectory(entt)
    add_subdirectory(imgui)
    add_subdirectory(litehtml)
    add_subdirectory(Lua)
    add_subdirectory(protobuf)
    add_subdirectory(rapidxml)
    add_subdirectory(SDL)
    add_subdirectory(SDL_mixer)
    add_subdirectory(SDL_ttf)
    add_subdirectory(spdlog)
    add_subdirectory(stb-cmake)

    install(TARGETS SDL3-shared
        RUNTIME DESTINATION ${intermediate_bin_dir}    # .dll
        ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}      # .lib
        LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}      # .so or .dylib
    )
    install(TARGETS SDL3_mixer-shared
        RUNTIME DESTINATION ${intermediate_bin_dir}    # .dll
        ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}      # .lib
        LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}      # .so or .dylib
    )
    install(TARGETS SDL3_ttf-shared
        RUNTIME DESTINATION ${intermediate_bin_dir}    # .dll
        ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}      # .lib
        LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}      # .so or .dylib
    )
    install(TARGETS spdlog
        RUNTIME DESTINATION ${intermediate_bin_dir}    # .dll
        ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}      # .lib
        LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}      # .so or .dylib
    )
    if(WIN32 AND ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    #     # Install pdb
        set(external_bin_dir ${PROJECT_BINARY_DIR}/external)
        install(FILES "${PROJECT_BINARY_DIR}/Debug/gumbo.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${external_bin_dir}/Lua/lua-5.4.7/Debug/lua_shared.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${external_bin_dir}/litehtml/Debug/litehtml.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${external_bin_dir}/SDL/Debug/SDL3.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${external_bin_dir}/SDL_mixer/Debug/SDL3_mixer.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${external_bin_dir}/SDL_ttf/Debug/SDL3_ttf.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${external_bin_dir}/spdlog/Debug/spdlogd.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/abseil_dll.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/libprotobufd.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/libprotobuf-lited.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/libprotocd.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/protoc.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/protoc-gen-upb.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/protoc-gen-upb_minitable.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/protoc-gen-upbdefs.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/utf8_range.pdb" DESTINATION ${intermediate_bin_dir})
        install(FILES "${PROJECT_BINARY_DIR}/bin/Debug/utf8_validity.pdb" DESTINATION ${intermediate_bin_dir})
    endif()
endif()