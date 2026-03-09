#!/bin/bash

if command -v conda >/dev/null 2>&1; then
	eval "$(conda shell.bash hook)"
	conda activate augury
fi

BIN=./bin/augury
# BIN=./bin/imp
CORE="${CORE:-0}"
REPETITIONS="${REPETITIONS:-32}"

quit_fail() {
	echo "${RED}[RUN FAIL] $1${NOCOLOR}"
	exit 1
}

if [ ! -f $BIN ]; then
	quit_fail "$BIN not found"
fi

RED="\033[1;31m"
GREEN="\033[1;32m"
NOCOLOR="\033[0m"
