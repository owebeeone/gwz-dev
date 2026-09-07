#!/bin/sh
set -eu
if [ "$#" -ne 3 ]; then
    echo "usage: $0 UID GID OUTPUT_BINARY (Linux, C++20)" >&2
    exit 2
fi
for helper_id in "$1" "$2"; do
    case "$helper_id" in ''|*[!0-9]*) echo 'UID and GID must be numeric' >&2; exit 2;; esac
done
if [ "$1" -eq 0 ] || [ "$2" -eq 0 ]; then echo 'UID/GID must be non-root' >&2; exit 2; fi
helper_source_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
exec c++ -std=c++20 -O2 -Wall -Wextra -Werror -Wconversion -Wsign-conversion \
    -fstack-protector-strong -D_FORTIFY_SOURCE=3 -fPIE -pie \
    -Wl,-z,relro,-z,now -DBENCH_UID="$1" -DBENCH_GID="$2" \
    "$helper_source_dir/gwz-fs-bench.cpp" -o "$3"
