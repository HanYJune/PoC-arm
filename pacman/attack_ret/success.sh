#!/system/bin/sh

set -eu

# Always operate relative to this script so the repo can be moved freely.
cd "$(dirname "$0")"
BIN="./backpac_ret_aut_success"

if [ ! -x "$BIN" ]; then
  echo "Binary not found at $BIN"
  echo "Compile backpac_ret first (see pacman.c header comment)."
  exit 1
fi

orr=0
while [ "$orr" -le 200 ]; do
  echo "==== ORR count: $orr ===="
  min_access=""

  run=0
  while [ "$run" -lt 1 ]; do
    run_output=$("$BIN" "$orr")
    access_val=$(printf "%s\n" "$run_output" | awk '/transient data access:/ {print $4}' | head -n 1)

    if [ -n "$access_val" ]; then
      if [ -z "$min_access" ] || [ "$access_val" -lt "$min_access" ]; then
        min_access=$access_val
      fi
    fi

    run=$((run + 1))
  done

  if [ -n "$min_access" ]; then
    echo "min transient access: $min_access"
  else
    echo "min transient access: (not found)"
  fi

  orr=$((orr + 1))
done
