#!/bin/bash
cd /workspaces/test123123

echo "=== Testing echo -n behavior ==="
echo 'echo -n test123' | ./minishell

echo ""
echo "=== Testing normal echo ==="
echo 'echo hello world' | ./minishell

echo ""
echo "=== Testing multiple commands for history ==="
echo 'echo cmd1
echo cmd2
echo -n cmd3' | ./minishell
