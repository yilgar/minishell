#!/bin/bash
cd /workspaces/test123123

echo "Testing readline behavior:"
echo ""

echo "=== Test 1: echo -n behavior (should stay on same line like bash) ==="
echo 'echo -n test123' | ./minishell

echo ""
echo "=== Test 2: Normal echo ==="
echo 'echo hello world' | ./minishell

echo ""
echo "=== Test 3: Multiple commands ==="
echo 'echo first
echo second  
echo third' | ./minishell
