#!/bin/bash

echo "=== BASH behavior for comparison ==="

echo "--- Quoted heredoc in bash ---"
export TESTVAR="Hello World"
cat << "EOF"
$TESTVAR
EOF

echo ""
echo "--- Unquoted heredoc in bash ---"
cat << EOF
$TESTVAR
EOF

echo ""
echo "--- Single quoted heredoc in bash ---"
cat << 'EOF'
$TESTVAR
EOF
