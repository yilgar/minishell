#!/bin/bash
cd /workspaces/test123123

echo "Testing minishell readline improvements:"
echo ""

# Test prompt behavior
echo "=== Testing echo -n ==="
echo 'echo -n -nnnn enes' | ./minishell

echo ""
echo "=== Testing normal commands ==="  
echo 'echo hello world
ls > /dev/null
echo done' | ./minishell
