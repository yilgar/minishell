#!/bin/bash
cd /workspaces/test123123

echo "Testing history prompt issue:"
echo ""

echo "Commands to add to history:"
echo 'echo -n first
echo -n second
echo normal' | ./minishell

echo ""
echo "Now manually test:"
echo "1. Run: ./minishell"
echo "2. Type: echo -n test"
echo "3. Press UP arrow to navigate history"
echo "4. Check if 'minishell> ' appears correctly"
