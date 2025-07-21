#!/bin/bash
cd /workspaces/test123123

echo "Testing history navigation:"
echo ""

echo "=== Adding commands to history ==="
echo 'echo command1
echo command2  
echo command3
echo -n lastcommand' | ./minishell

echo ""
echo "Now test manually:"
echo "1. Run ./minishell"
echo "2. Use UP/DOWN arrows to navigate history"
echo "3. Check if 'minishell> ' prompt stays visible"
