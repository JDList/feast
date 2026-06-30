#!/usr/bin/env bash

set -euo pipefail

OUTPUT="CMakeLists.txt"

cat > "$OUTPUT" <<'EOF'
# Auto-generated CMakeLists.txt

EOF

for file in *.cpp; do
    [ -f "$file" ] || continue

    target="${file%.cpp}"

    cat >> "$OUTPUT" <<EOF
add_executable(${target} ${file})
target_link_libraries(${target} PRIVATE feast)
add_test(NAME ${target} COMMAND ${target})

EOF

done

echo "Generated ${OUTPUT}"
