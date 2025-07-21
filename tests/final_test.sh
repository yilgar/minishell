#!/bin/bash
cd /workspaces/test123123

echo "Final test for readline improvements:"
echo ""

echo "=== Testing echo -n (should show prompt on new line) ==="
echo 'echo -n test123' | ./minishell

echo ""
echo "=== Testing normal commands ==="
echo 'echo hello
pwd
echo bye' | ./minishell
