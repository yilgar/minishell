#!/bin/bash
cd /workspaces/test123123

echo "Testing multiple variables in heredoc:"
echo ""

# Test with multiple variables
echo "=== Minishell test with multiple variables ==="
echo 'cat << eof
Home directory: $HOME
User: $USER
Path: $PATH
eof' | ./minishell

echo ""
echo "=== Test with exit status ==="
echo 'false
cat << eof
Exit status: $?
eof' | ./minishell
