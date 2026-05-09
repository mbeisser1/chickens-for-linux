#!/usr/bin/env bash
# Run clang-format on game sources (repo-root .clang-format). Skips build/ and Allegro.
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"
find src -maxdepth 1 \( -name '*.cpp' -o -name '*.h' \) -print0 | xargs -0 clang-format -i
echo "clang-format: done ($(find src -maxdepth 1 \( -name '*.cpp' -o -name '*.h' \) | wc -l) files under src/)"
