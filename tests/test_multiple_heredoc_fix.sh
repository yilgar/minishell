#!/bin/bash

echo "=== Multiple Heredoc Test ==="

# Test 1: Multiple heredocs with different delimiters
echo "Test 1: cat << EOF1 << EOF2"
echo "Expected: Only 'Second content' should be displayed"

./minishell << 'SHELL_INPUT'
cat << EOF1 << EOF2
First content
EOF1
Second content
EOF2
exit
SHELL_INPUT

echo ""
echo "=== Comparison with bash ==="
echo "Bash output:"
bash << 'SHELL_INPUT'
cat << EOF1 << EOF2
First content
EOF1
Second content
EOF2
SHELL_INPUT

echo ""
echo "Test completed."
