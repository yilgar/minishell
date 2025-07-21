#!/bin/bash

echo "=== Testing quoted heredoc (should NOT expand variables) ==="
printf 'export TESTVAR="Hello World"\ncat << "EOF"\n$TESTVAR\nEOF\nexit\n' | ./minishell

echo ""
echo "=== Testing unquoted heredoc (should expand variables) ==="
printf 'export TESTVAR="Hello World"\ncat << EOF\n$TESTVAR\nEOF\nexit\n' | ./minishell

echo ""
echo "=== Testing single quoted heredoc (should NOT expand variables) ==="
printf "export TESTVAR=\"Hello World\"\ncat << 'EOF'\n\$TESTVAR\nEOF\nexit\n" | ./minishell
