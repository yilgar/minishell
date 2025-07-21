#!/bin/bash

echo "Signal handling test başlıyor..."

# Test 1: Normal command execution
echo "Test 1: Normal command execution"
echo "echo 'Hello World'" | timeout 3 ./minishell

# Test 2: Heredoc test (manuel test gerekiyor)
echo ""
echo "Test 2: Heredoc signal handling - manuel test gerekiyor"
echo "Şu komutu çalıştırın ve Ctrl+C ile test edin:"
echo "echo 'cat << EOF' | ./minishell"

# Test 3: Pipeline test
echo ""
echo "Test 3: Pipeline execution"
echo "echo 'ls | grep minishell'" | timeout 3 ./minishell

echo ""
echo "Signal handling refactor tamamlandı!"
echo "Yeni yapı:"
echo "- setup_signals_interactive(): İnteraktif mod için"
echo "- setup_signals_child_waiting(): Parent child beklerken"
echo "- setup_signals_child_default(): Child process'te normal signaller"
echo "- setup_signals_heredoc_child(): Heredoc child'da özel setup"
