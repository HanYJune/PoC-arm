#!/system/bin/sh

core="$1"
ll_size="$2"
stride="$3"

if [ -z "$core" ] || [ -z "$ll_size" ] || [ -z "$stride" ]; then
    echo "Usage: $0 CORE LL_SIZE STRIDE"
    exit 1
fi

echo "Last node flushed, predicted address cached:"
muls=0
while [ "$muls" -le 300 ]; do
    ./speculation-window "$core" "$ll_size" "$stride" "$muls" 1 0
    muls=$((muls + 10))
done

echo "Last node cached, predicted address cached:"
muls=0
while [ "$muls" -le 300 ]; do
    ./speculation-window "$core" "$ll_size" "$stride" "$muls" 0 0
    muls=$((muls + 10))
done

echo "Last node flushed, predicted address flushed:"
muls=0
while [ "$muls" -le 300 ]; do
    ./speculation-window "$core" "$ll_size" "$stride" "$muls" 1 1
    muls=$((muls + 10))
done

echo "Last node cached, predicted address flushed:"
muls=0
while [ "$muls" -le 300 ]; do
    ./speculation-window "$core" "$ll_size" "$stride" "$muls" 0 1
    muls=$((muls + 10))
done
