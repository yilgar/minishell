#!/bin/bash

echo "Testing PATH expansion without quotes:"
printf 'export PATH=$PATH:/home/codespaces\necho $PATH\nexit\n' | ./minishell > output1.txt 2>&1

echo "Testing PATH expansion with quotes:"
printf 'export PATH="$PATH:/home/codespaces"\necho $PATH\nexit\n' | ./minishell > output2.txt 2>&1

echo "Testing variable expansion without quotes:"
printf 'echo /desktop$HOME/desktop\nexit\n' | ./minishell > output3.txt 2>&1

echo "Testing variable expansion with quotes:"
printf 'echo "/desktop/$HOME/desktop"\nexit\n' | ./minishell > output4.txt 2>&1

echo "=== Results ==="
echo "PATH without quotes:"
cat output1.txt
echo "PATH with quotes:"
cat output2.txt
echo "Variable without quotes:"
cat output3.txt
echo "Variable with quotes:"
cat output4.txt

rm -f output1.txt output2.txt output3.txt output4.txt
