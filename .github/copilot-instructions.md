# Minishell Copilot Instructions

## 42 School Norm Compliance

This project follows the strict **42 School Norm** coding standards:

### Critical Naming Rules
- Structures: `s_` prefix (e.g., `s_token`, `s_cmd`)
- Typedefs: `t_` prefix (e.g., `t_token`, `t_pipeline`)
- Enums: `e_` prefix (e.g., `e_token_type`)
- Globals: `g_` prefix (e.g., `g_exit_status`)
- Snake_case only - no camelCase, all lowercase with underscores

### Function Constraints
- **Max 25 lines** per function (excluding braces)
- **Max 4 parameters** per function
- **Max 5 variables** per function
- **Max 5 functions** per .c file
- Return values must be in parentheses: `return (value);`

### Formatting Rules
- **80 columns max** per line
- **4-char tabulations** (not spaces)
- Braces on separate lines except for struct/enum/union declarations
- One declaration per line, no declaration+initialization except globals/static/const
- Empty line between declarations and function body
- Asterisks stick to variable names: `char *str` not `char* str`

### Forbidden Constructs
- `for` loops (use `while` instead)
- `do...while`, `switch`, `case`, `goto`
- Ternary operators (`? :`)
- Variable Length Arrays (VLAs)

## Architecture Overview

This is a POSIX shell implementation with a **structured pipeline architecture**: Lexer → Parser → Expander → Executor. Each stage is isolated in its own directory with minimal interdependencies.

### Core Data Flow
1. **Lexer** (`lexer/`) - Tokenizes input into `t_token` linked list with types (WORD, PIPE, redirections, quotes)
2. **Parser** (`parser/`) - Builds `t_pipeline` containing `t_cmd` structures from tokens
3. **Expander** (`expander/`) - Handles variable expansion (`$VAR`, `$?`) respecting quote contexts
4. **Executor** (`execution/`) - Executes commands with process management and I/O redirection

### Key Structural Patterns

**Memory Management**: Everything uses a **garbage collector** (`gc/gc.c`). Always wrap allocations with `gc_track(gc, malloc(...))`. The GC automatically frees all tracked memory on shell exit or error.

**Environment Handling**: The `t_env` structure maintains both original and modified environment arrays. Use `env_get_value()`, `env_add_str()`, `env_remove_key()` rather than direct `char **envp` manipulation.

**Token Processing**: Tokens have metadata flags (`has_space_before`, `from_env_expansion`, `is_quoted_delimiter`) that affect parsing behavior. The `concatenate_tokens()` function respects these flags for proper word assembly.

**Process Architecture**: Single commands run in the main process (for builtins) or fork. Pipelines create child processes with pipe setup. Heredocs always fork for signal isolation.

**Norm-Compliant Patterns**: 
- Use `while` loops instead of `for`: `i = -1; while (++i < len)`
- Split long functions into helper functions to stay under 25 lines
- Limit function parameters to 4 max - use context structures like `t_exec_context`
- All error returns use parentheses: `return (-1);`, `return (NULL);`

## Critical Implementation Details

### Heredoc Processing
Heredocs (`<<`) **always fork** a child process for proper signal handling (Ctrl+C should exit heredoc, not shell). The parent waits and manages the read file descriptor. Use `handle_heredoc()` in `execution/exec_heredoc_main.c`.

### Signal Handling
- **Interactive mode**: SIGINT displays new prompt, SIGQUIT ignored
- **Command execution**: Child inherits default signal handlers
- **Heredoc**: Special signal setup in child process
- Global `g_exit_status` tracks signal-based exits (130 for SIGINT)

### Redirection Chain
Commands can have multiple redirections processed left-to-right. File redirections override previous ones; heredocs stack in a linked list (`t_heredoc`). Always validate syntax before execution.

### Quote Expansion Rules
- Single quotes: No expansion
- Double quotes: Expand `$VAR` and `$?`, preserve literal `$` for invalid vars
- Unquoted: Full expansion + word splitting
- Heredoc delimiters: Quoted delimiters disable expansion in heredoc content

## Build & Debug Workflow

```bash
make                # Build with debug symbols (-g)
make clean          # Clean objects only
make fclean         # Full clean including libft
```

**Memory Debugging**: Use `valgrind --suppressions=valgrind.supp ./minishell` to ignore readline library leaks.

**Testing**: Scripts in `tests/` directory cover specific scenarios:
- `test_heredoc.sh` - Heredoc edge cases
- `test_bash_comparison.sh` - Output comparison with bash
- `final_test.sh` - Comprehensive functionality test

## Directory Responsibilities

- `builtin/` - Shell built-in commands (cd, echo, env, export, pwd, unset, exit)
- `env/` - Environment variable management functions
- `lexer/` - Tokenization and character classification
- `parser/` - Syntax analysis and AST construction
- `expander/` - Variable expansion logic
- `execution/` - Process management, I/O redirection, command execution
- `signals/` - Signal handler setup for different execution contexts
- `utils/` - Character type checking utilities
- `libft/` - Standard C library functions (linked as static library)

## Common Gotchas

**Norm Compliance**: 
- Variables must be declared at function start with empty line before logic
- No inline declarations: `int i = 0;` forbidden (except globals/static/const)
- Use tabs not spaces for indentation
- Function prototypes need explicit parameter names: `int func(char *str, int len);`

**Fork Safety**: Always `gc_free_all()` and `env_cleanup()` before `exit()` in child processes to prevent memory leaks.

**File Descriptor Management**: Track all opened FDs. Use helper functions like `close_pipes()` and always close redirected FDs after use.

**Token Concatenation**: Adjacent tokens without spaces (like `echo$HOME`) must be concatenated into single arguments. Check `has_space_before` flag.

**Exit Status**: Distinguish between command exit codes (0-255) and signal termination (128+signal). Use `$?` expansion for the last command's status.

**Readline Integration**: Commands using readline (heredocs) need special terminal attribute handling to prevent echo control character display.
