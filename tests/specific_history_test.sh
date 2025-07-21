#!/bin/bash
cd /workspaces/test123123

echo "Specific test for history navigation with echo -n:"
echo ""

echo "Testing echo -n followed by normal commands:"
echo 'echo -n test1
echo normal1  
echo -n test2
echo normal2' | ./minishell

echo ""
echo "Manual test:"
echo "Run: ./minishell"  
echo "Commands to try:"
echo "1. echo -n hello"
echo "2. Press UP arrow - should show 'minishell> echo -n hello'"
