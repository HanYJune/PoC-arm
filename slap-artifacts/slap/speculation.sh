#!/system/bin/sh

core="$1"
if [ -z "$core" ]; then
    echo "Usage: $0 CORE"
    exit 1
fi

length=0
while [ "$length" -le 1000 ]; do
    stride=-320
    while [ "$stride" -le 320 ]; do
        ./speculation "$core" "$length" "$stride"
        stride=$((stride + 8))
    done
    length=$((length + 20))
done
