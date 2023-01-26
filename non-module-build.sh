#!/usr/bin/bash

# This script supposed to be called by make to build the project with a
# compiler that does not support modules.
# Usage:
# ./non-module-build.sh g++

CXX=$1

function header {
  echo "#pragma once"
}

function filter {
  sed -E "s/import (<.*>);/#include \1/" | \
  sed -E "s/import (\".*\");/#include \1/" | \
  sed -E "s/import (.*);/#include \"\1.h\"/" | \
  sed -E "s/export module.*//g" | \
  sed -E "s/export class /class /g" | \
  sed -E "s/export namespace /namespace /g" | \
  sed -E "s/export enum /enum /g" | \
  sed -E "s/export struct /struct /g" | \
  sed -E "s/export template /template /g" | \
  sed -E "s/module;/\/\/module/g" |
  sed -E "s/export / /g" 
#  sed -E "s/export /inline /g" | \
}

tmpdir=build/.tmp

mkdir -p ${tmpdir}/src

echo processing module to non-module code

for f in src/*.cppm
do
  in=$f
  out=$tmpdir/${f%%.*}.h
  (header && cat $in) | filter > $out
done

cat src/main-nix.cpp | filter > ${tmpdir}/src/main.cpp

cp src/*.h ${tmpdir}/src

echo building...
${CXX} ${tmpdir}/src/main.cpp -o build/rym -lSDL2 -std=c++17 -stdlib=libc++

# Why it is not possible to compile without libc++ is beyond my understanding
