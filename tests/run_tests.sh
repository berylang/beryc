#!/usr/bin/env bash
set -uo pipefail

BERY_BIN="${BERY_BIN:-./build/bery}"
TESTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

PASS=0
FAIL=0

for dir in "$TESTS_DIR"/*/; do
    name="$(basename "$dir")"
    testFile="$dir/test.bry"
    expectedFile="$dir/expected.txt"

    [ -f "$testFile" ] || continue
    [ -f "$expectedFile" ] || continue

    actual="$("$BERY_BIN" run "$testFile" 2>&1)"
    expected="$(cat "$expectedFile")"

    if [ "$actual" == "$expected" ]; then
        echo "PASS: $name"
        PASS=$((PASS+1))
    else
        echo "FAIL: $name"
        echo "  expected: $expected"
        echo "  actual:   $actual"
        FAIL=$((FAIL+1))
    fi
done

echo ""
echo "$PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]