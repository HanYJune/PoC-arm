#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_FILE="${SCRIPT_DIR}/kenable-pmu.c"
OUT_KO="${SCRIPT_DIR}/kenable-pmu.ko"

KBUILD_DIR="${KBUILD_DIR:-/home/hyj/android-kernel/out/kbuild}"
ARCH="${ARCH:-arm64}"
LLVM="${LLVM:-1}"

if [[ ! -f "${SRC_FILE}" ]]; then
  echo "error: source not found: ${SRC_FILE}" >&2
  exit 1
fi

if [[ ! -d "${KBUILD_DIR}" ]]; then
  echo "error: kbuild dir not found: ${KBUILD_DIR}" >&2
  exit 1
fi

tmpdir="$(mktemp -d /tmp/kenable-pmu-build.XXXXXX)"
cleanup() {
  rm -rf "${tmpdir}"
}
trap cleanup EXIT

cp "${SRC_FILE}" "${tmpdir}/kenable-pmu.c"
printf 'obj-m += kenable-pmu.o\n' > "${tmpdir}/Makefile"

make -C "${KBUILD_DIR}" M="${tmpdir}" ARCH="${ARCH}" LLVM="${LLVM}" modules
cp "${tmpdir}/kenable-pmu.ko" "${OUT_KO}"

echo "built: ${OUT_KO}"
modinfo "${OUT_KO}" | sed -n '1,80p'
