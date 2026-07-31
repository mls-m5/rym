cmake_minimum_required(VERSION 3.20)

project(rym LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# The original sources use C++20 modules, including imports of standard-library
# headers.  Support for those header units still varies significantly between
# compilers.  Generate the project's established non-module representation so
# a normal C++20 compiler can build it consistently.
set(RYM_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${RYM_GENERATED_DIR}")

function(rym_demodule INPUT OUTPUT)
    file(READ "${INPUT}" CONTENT)
    string(REGEX REPLACE "import (<[^;\n]+>);" "#include \\1" CONTENT "${CONTENT}")
    string(REGEX REPLACE "import (\"[^;\n]+\");" "#include \\1" CONTENT "${CONTENT}")
    string(REGEX REPLACE "import ([A-Za-z_][A-Za-z0-9_]*);" "#include \"\\1.h\"" CONTENT "${CONTENT}")
    string(REGEX REPLACE "export module[^;\n]*;" "" CONTENT "${CONTENT}")
    string(REGEX REPLACE "(^|\n)module;" "\\1// module;" CONTENT "${CONTENT}")
    string(REGEX REPLACE "export[ \t]+" "" CONTENT "${CONTENT}")
    if(OUTPUT MATCHES "\\.h$")
        file(WRITE "${OUTPUT}" "#pragma once\n${CONTENT}")
    else()
        file(WRITE "${OUTPUT}" "${CONTENT}")
    endif()
endfunction()

file(GLOB RYM_MODULES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cppm")
foreach(MODULE_SOURCE IN LISTS RYM_MODULES)
    get_filename_component(MODULE_NAME "${MODULE_SOURCE}" NAME_WE)
    rym_demodule("${MODULE_SOURCE}" "${RYM_GENERATED_DIR}/${MODULE_NAME}.h")
endforeach()

rym_demodule(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main-nix.cpp"
    "${RYM_GENERATED_DIR}/main.cpp"
)
configure_file(src/glfunctions.h "${RYM_GENERATED_DIR}/glfunctions.h" COPYONLY)
configure_file(src/vektorer.h "${RYM_GENERATED_DIR}/vektorer.h" COPYONLY)
configure_file(src/rym_private.h "${RYM_GENERATED_DIR}/rym_private.h" COPYONLY)

add_executable(rym "${RYM_GENERATED_DIR}/main.cpp")
target_include_directories(rym PRIVATE "${RYM_GENERATED_DIR}")
target_compile_definitions(rym PRIVATE GL_GLEXT_PROTOTYPES=1 GL3_PROTOTYPES=1)
target_link_libraries(rym PRIVATE SDL2::SDL2 OpenGL::GL)
