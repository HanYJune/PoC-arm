#!/system/bin/sh
set -eu

# Run pacman with each ORR-count argument 100 times and measure the transient
# access latency reported by the binary.

cd "$(dirname "$0")"
BIN="./pacman"
RUNS=${RUNS:-100}
TRANSIENT_THRESHOLD=${TRANSIENT_THRESHOLD:-150}

if [ ! -x "$BIN" ]; then
  echo "Binary not found at $BIN"
  echo "Compile pacman first (see pacman.c header comment)."
  exit 1
fi

if [ "$#" -eq 0 ]; then
  set -- $(seq 0 100)
fi

for arg in "$@"; do
  echo "==== ORR count: $arg (transient threshold: $TRANSIENT_THRESHOLD ns) ===="
  hits=0
  total=0
  transient_sum=0

  i=0
  while [ "$i" -lt "$RUNS" ]; do
    if ! run_output=$("$BIN" "$arg"); then
      echo "run $i failed, skipping" >&2
      i=$((i + 1))
      continue
    fi

    transient_val=$(printf "%s\n" "$run_output" | awk '/transient data access:/ {print $4}' | head -n 1)
    if [ -z "$transient_val" ]; then
      echo "run $i: could not parse transient latency, skipping" >&2
      i=$((i + 1))
      continue
    fi

    total=$((total + 1))
    transient_sum=$((transient_sum + transient_val))
    if [ "$transient_val" -lt "$TRANSIENT_THRESHOLD" ]; then
      hits=$((hits + 1))
    fi
    i=$((i + 1))
  done

  if [ "$total" -eq 0 ]; then
    echo "No successful samples collected."
    continue
  fi

  hit_rate=$(awk -v h="$hits" -v t="$total" 'BEGIN { printf "%.2f", (t==0 ? 0 : (h*100.0)/t) }')
  avg_transient=$(awk -v s="$transient_sum" -v t="$total" 'BEGIN { printf "%.2f", (t==0 ? 0 : s/t) }')

  echo "samples: $total/$RUNS"
  echo "transient rate (<${TRANSIENT_THRESHOLD} ns): $hit_rate% ($hits/$total)"
  echo "avg transient latency: ${avg_transient} ns"
done
