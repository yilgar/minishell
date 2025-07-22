#!/bin/bash

echo "=== Testing Bash behavior ==="
echo "Testing bash heredoc with Ctrl+C:"
timeout 5s bash -c '
echo "cat << eof" > /tmp/test_input
echo "" >> /tmp/test_input
echo -e "\x03" >> /tmp/test_input  # Ctrl+C
bash < /tmp/test_input
'

echo ""
echo "=== Testing Minishell behavior ==="
echo "Testing minishell heredoc with Ctrl+C:"
timeout 5s bash -c '
echo "cat << eof" > /tmp/test_input_mini
echo "" >> /tmp/test_input_mini
echo -e "\x03" >> /tmp/test_input_mini  # Ctrl+C
./minishell < /tmp/test_input_mini
'

# Cleanup
rm -f /tmp/test_input /tmp/test_input_mini
