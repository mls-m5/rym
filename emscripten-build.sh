#!/usr/bin/env bash

set -euo pipefail

compiler=${CXX:-em++}
generated_dir=build/emscripten/generated
output_dir=build/emscripten

filter() {
    sed -E "s/import (<.*>);/#include \\1/" |
        sed -E "s/import (\".*\");/#include \\1/" |
        sed -E "s/import (.*);/#include \"\\1.h\"/" |
        sed -E "s/export module.*//g" |
        sed -E "s/export class /class /g" |
        sed -E "s/export namespace /namespace /g" |
        sed -E "s/export enum /enum /g" |
        sed -E "s/export struct /struct /g" |
        sed -E "s/export template /template /g" |
        sed -E "s/module;/\\/\\/ module;/g" |
        sed -E "s/export / /g"
}

mkdir -p "${generated_dir}"

for source in src/*.cppm; do
    output="${generated_dir}/$(basename "${source%.cppm}").h"
    {
        echo "#pragma once"
        filter < "${source}"
    } > "${output}"
done

filter < src/main-nix.cpp > "${generated_dir}/main.cpp"
cp src/*.h "${generated_dir}/"

"${compiler}" \
    "${generated_dir}/main.cpp" \
    -I"${generated_dir}" \
    -std=c++20 \
    -fexceptions \
    -O3 \
    -sUSE_SDL=2 \
    -sMAX_WEBGL_VERSION=2 \
    -sASSERTIONS=0 \
    -sNO_DISABLE_EXCEPTION_CATCHING \
    -sALLOW_MEMORY_GROWTH=1 \
    -sEXIT_RUNTIME=0 \
    -o "${output_dir}/rym.html"

echo "built ${output_dir}/rym.html"
