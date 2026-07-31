#!/usr/bin/env bash

set -euo pipefail

repo_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
output_dir="${repo_dir}/build/container-modules"
image=rym-modules-build

if command -v podman >/dev/null 2>&1 && podman info >/dev/null 2>&1; then
    engine=podman
elif command -v docker >/dev/null 2>&1 && docker info >/dev/null 2>&1; then
    engine=docker
else
    echo "Podman or Docker is required." >&2
    exit 1
fi

"${engine}" build --file "${repo_dir}/Dockerfile.modules" --tag "${image}" "${repo_dir}"
container=$("${engine}" create "${image}")
trap '"${engine}" rm -f "${container}" >/dev/null 2>&1 || true' EXIT

mkdir -p "${output_dir}"
"${engine}" cp "${container}:/artifacts/rym" "${output_dir}/rym"

echo "built ${output_dir}/rym"
