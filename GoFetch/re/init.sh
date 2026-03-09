#!/bin/sh

# increase stack size to avoid segfault
ulimit -s 65520

# create data directory
cd ..
mkdir -p data
cd re

# build experiments
cd src
if [ -n "${ANDROID_NDK_HOME}" ] && [ -z "${CC}" ]; then
    CC="${ANDROID_NDK_HOME}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android33-clang"
fi

if [ -n "${CC}" ]; then
    make TARGET_OS=android CC="${CC}"
else
    make TARGET_OS=android
fi

# run cache experiments
sudo ./evset_gen.out
