#!/bin/bash

echo "=== Complex heredoc test ==="

# Test with mixed quoted/unquoted delimiters
printf 'export TEST="value"\ncat << "Q1" << U1 << '"'"'Q2'"'"'\nQuoted 1: $TEST\nQ1\nUnquoted 1: $TEST\nU1\nQuoted 2: $TEST\nQ2\nexit\n' | ./minishell

echo ""
echo "=== Comparison with bash ==="
export TEST="value"
cat << "Q1" << U1 << 'Q2'
Quoted 1: $TEST
Q1
Unquoted 1: $TEST
U1
Quoted 2: $TEST
Q2
