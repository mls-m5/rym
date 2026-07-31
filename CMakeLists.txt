cmake_minimum_required(VERSION 3.28)

project(rym LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
   CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
    message(FATAL_ERROR
        "CMake's C++ module scanning requires GCC 14 or newer. "
        "Configure with Clang 16+ instead, for example "
        "-DCMAKE_CXX_COMPILER=clang++.")
endif()

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(rym src/main-nix.cpp)
target_compile_features(rym PRIVATE cxx_std_20)
set_target_properties(rym PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_SCAN_FOR_MODULES ON
)

target_sources(rym
    PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src"
        FILES
            src/broadphase.cppm
            src/comet.cppm
            src/common.cppm
            src/draw.cppm
            src/explosion.cppm
            src/eye.cppm
            src/glapi.cppm
            src/graf.cppm
            src/hant.cppm
            src/linesmoke.cppm
            src/matgl.cppm
            src/obj.cppm
            src/particle.cppm
            src/projectile.cppm
            src/roamingbroadphase.cppm
            src/shaderprogram.cppm
            src/ship.cppm
            src/star.cppm
            src/unit.cppm
            src/vec.cppm
)

target_include_directories(rym PRIVATE src)
target_compile_definitions(rym PRIVATE GL_GLEXT_PROTOTYPES=1 GL3_PROTOTYPES=1)
target_link_libraries(rym PRIVATE SDL2::SDL2 OpenGL::GL)
