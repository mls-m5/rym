
set_languages("c++20")
target("rym")
    set_kind("binary")
    add_files("src/*.cpp")
    add_files("src/*.cppm")
    add_cxxflags("clang::-stdlib=libc++")
    add_links("SDL2")
