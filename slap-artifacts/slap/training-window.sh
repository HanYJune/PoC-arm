#!/system/bin/sh

core="$1"
if [ -z "$core" ]; then
    echo "Usage: $0 CORE"
    exit 1
fi

muls=0
while [ "$muls" -le 10 ]; do
    ./training-window "$core" "$muls"
    muls=$((muls + 1))
done
