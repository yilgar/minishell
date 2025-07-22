#!/bin/bash

echo "Debug: Testing /desktop\$HOME/desktop expansion"

printf 'echo "/desktop/$HOME/desktop"\nexit\n' | ./minishell > debug1.txt 2>&1
printf 'echo /desktop$HOME/desktop\nexit\n' | ./minishell > debug2.txt 2>&1

echo "=== With quotes ==="
cat debug1.txt

echo "=== Without quotes ==="
cat debug2.txt

rm -f debug1.txt debug2.txt
