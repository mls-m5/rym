

cmake_minimum_required(VERSION 3.24)

project(rym)

# Ack use of experimental features
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "3c375311-a3c9-4396-a187-3227ef642046")

# Experimental Module support
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)



file(GLOB RYM_SRCS src/*.cppm)

add_executable(
    rym
    "src/main-nix.cpp"
    )

target_compile_features(rym PUBLIC cxx_std_20)

target_sources(
    rym
      PUBLIC
        FILE_SET CXX_MODULES FILES ${RYM_SRC})

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}  -stdlib=libc++")

set(CMAKE_EXPERIMENTAL_CXX_SCANDEP_SOURCE "clang++-16")

set_property(TARGET rym PROPERTY CXX_STANDARD 20)
