#!/bin/bash
cd /workspaces/test123123

echo "Comparing bash vs minishell echo -n behavior:"
echo ""

echo "=== Bash behavior ==="
echo -n "bash_prompt$ "
echo -n "test123"
echo -n "bash_prompt$ "
echo ""

echo ""
echo "=== Minishell behavior ==="
echo 'echo -n test123' | ./minishell
