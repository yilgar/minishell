#!/bin/bash

echo "Testing multiple heredoc..."

# Test with multiple heredocs - this should fail in current implementation
echo "Running: cat << EOF1 << EOF2"
echo "First heredoc" | ./minishell -c "cat << EOF1 << EOF2
First line
EOF1
Second line  
EOF2" 2>&1

echo ""
echo "Expected output should show only 'Second line' (the last heredoc content)"
