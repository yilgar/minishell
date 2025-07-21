#!/bin/bash
cd /workspaces/test123123

echo "Manual test for history navigation:"
echo "1. Run minishell: ./minishell"
echo "2. Execute: echo -n test"
echo "3. Press UP arrow to see previous command"
echo "4. Check if 'minishell> ' is visible"
echo ""
echo "Expected: History should show 'echo -n test' with visible prompt"
echo ""
echo "Starting minishell now..."
./minishell
