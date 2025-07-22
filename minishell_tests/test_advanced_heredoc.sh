#!/bin/bash

echo "=== Extended Multiple Heredoc Tests ==="

echo "Test 1: Three heredocs"
./minishell << 'SHELL_INPUT'
cat << A << B << C
content A
A
content B
B
content C
C
exit
SHELL_INPUT

echo ""
echo "Test 2: Empty heredoc"
./minishell << 'SHELL_INPUT'
cat << EMPTY << FULL
EMPTY
This is the full content
FULL
exit
SHELL_INPUT

echo ""
echo "Test 3: Heredoc with variables"
./minishell << 'SHELL_INPUT'
export TESTVAR="Hello World"
cat << FIRST << SECOND
$TESTVAR from first
FIRST
$TESTVAR from second
SECOND
exit
SHELL_INPUT

echo ""
echo "All tests completed successfully!"
