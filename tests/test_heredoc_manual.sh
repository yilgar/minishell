#!/bin/bash
cd /workspaces/test123123

echo "Testing heredoc with variable expansion in minishell:"
echo ""

# Test with minishell
echo "=== Minishell test ==="
echo 'cat << eof
$HOME
eof' | ./minishell

echo ""
echo "=== Bash test ==="
cat << eof
$HOME
eof
