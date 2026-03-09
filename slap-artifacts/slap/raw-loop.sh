#!/system/bin/sh

core="$1"
if [ -z "$core" ]; then
    echo "Usage: $0 CORE"
    exit 1
fi

i=10
while [ "$i" -le 1000 ]; do
    ./raw-loop "$core" "$i"
    i=$((i + 10))
done
