#!/usr/bin/env bash

# This script is intended to be called by make to build the project with a
# compiler that does not support modules.
# Usage:
# ./non-module-build.sh [compiler]

set -euo pipefail

compiler=${1:-${CXX:-c++}}
tmpdir=build/.tmp
output=build/rym

filter() {
  sed -E "s/import (<.*>);/#include \1/" | \
  sed -E "s/import (\".*\");/#include \1/" | \
  sed -E "s/import (.*);/#include \"\1.h\"/" | \
  sed -E "s/export module.*//g" | \
  sed -E "s/export class /class /g" | \
  sed -E "s/export namespace /namespace /g" | \
  sed -E "s/export enum /enum /g" | \
  sed -E "s/export struct /struct /g" | \
  sed -E "s/export template /template /g" | \
  sed -E "s/module;/\/\/ module;/g" | \
  sed -E "s/export / /g"
}

mkdir -p "${tmpdir}/src"

echo processing module to non-module code

for f in src/*.cppm
do
  out="${tmpdir}/${f%.cppm}.h"
  {
    echo "#pragma once"
    filter < "${f}"
  } > "${out}"
done

filter < src/main-nix.cpp > "${tmpdir}/src/main.cpp"

cp src/*.h "${tmpdir}/src"

echo building...
"${compiler}" \
  "${tmpdir}/src/main.cpp" \
  -o "${output}" \
  -std=c++20 \
  $(pkg-config --cflags --libs sdl2) \
  -lGL

echo "built ${output}"
