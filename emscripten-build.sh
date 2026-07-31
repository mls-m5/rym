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
    --shell-file web/shell.html \
    -o "${output_dir}/rym.html"

cp "${output_dir}/rym.html" "${output_dir}/index.html"
cp web/manifest.webmanifest "${output_dir}/"
cp web/service-worker.js "${output_dir}/service-worker.js"
cp rym-logo.png "${output_dir}/rym-logo.png"

# Changing the cache name whenever the Wasm changes ensures an installed PWA
# updates after a new GitHub Pages deployment.
wasm_hash=$(sha256sum "${output_dir}/rym.wasm" | cut -c1-12)
sed -i "s/rym-v1/rym-${wasm_hash}/" "${output_dir}/service-worker.js"

echo "built ${output_dir}/rym.html"
