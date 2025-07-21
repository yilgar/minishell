#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

# ifndef ECHOCTL
#  define ECHOCTL 0001000
# endif

typedef struct s_gc		t_gc;

typedef struct s_gc_node
{
	void				*ptr;
	struct s_gc_node	*next;
}						t_gc_node;

struct					s_gc
{
	t_gc_node			*head;
};

typedef struct s_env
{
	char				**envp;
	char				**original_envp;
}						t_env;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_APPEND,
	TOKEN_REDIRECT_HEREDOC,
	TOKEN_SINGLE_QUOTE,
	TOKEN_DOUBLE_QUOTE,
	TOKEN_ENV_VAR,
	TOKEN_EXIT_STATUS,
	TOKEN_EOF,
	TOKEN_ERROR
}						t_token_type;

typedef struct s_token
{
	t_token_type		type;
	char				*value;
	int					has_space_before;
	int					from_env_expansion;
	int					is_heredoc_delimiter;
	int					is_quoted_delimiter;
	struct s_token		*next;
}						t_token;

typedef struct s_heredoc
{
	char				*delimiter;
	int					is_quoted;
	struct s_heredoc	*next;
}						t_heredoc;

typedef struct s_cmd
{
	char				**args;
	char				*input_file;
	char				*output_file;
	int					append_mode;
	int					heredoc_delimiter;
	char				*heredoc_delim;
	int					heredoc_is_quoted;
	t_heredoc			*heredoc_list;
	int					redirection_failed;
	struct s_cmd		*next;
}						t_cmd;

typedef struct s_pipeline
{
	t_cmd				*commands;
	int					exit_status;
}						t_pipeline;

typedef struct s_exec_context
{
	t_gc				*gc;
	t_env				*env;
	t_cmd				*cmd;
	int					input_fd;
	int					output_fd;
}						t_exec_context;

void					gc_init(t_gc *gc);
void					*gc_track(t_gc *gc, void *ptr);
void					**gc_track_array(t_gc *gc, void **array);
void					gc_free_all(t_gc *gc);

t_env					*env_init_copy(t_gc *gc, char **envp);
void					env_cleanup(t_env *env);
void					env_cleanup_child(t_env *env);
void					env_add_str(t_gc *gc, t_env *env, const char *str);
void					env_remove_key(t_gc *gc, t_env *env, const char *key);
int						env_len(char **envp);
char					*env_get_value(t_env *env, const char *key);

// Builtin functions
int						builtin_pwd(t_gc *gc, char **args);
int						builtin_echo(t_gc *gc, char **args);
int						builtin_env(t_gc *gc, t_env *env, char **args);
int						builtin_cd(t_gc *gc, t_env *env, char **args);
int						builtin_export(t_gc *gc, t_env *env, char **args);
int						builtin_unset(t_gc *gc, t_env *env, char **args);
int						builtin_exit(t_gc *gc, t_env *env, char **args);
int						is_builtin(char *cmd);
int						execute_builtin(t_gc *gc, t_env *env, char **args);

// Lexer/Tokenizer functions
t_token					*tokenize(t_gc *gc, char *input);
t_token					*create_token(t_gc *gc, t_token_type type, char *value);
void					add_token(t_token **head, t_token *new_token);
void					print_tokens(t_token *tokens);
t_token					*handle_special_char(t_gc *gc, char *input, int *i);
t_token					*handle_pipe_token(t_gc *gc, int *i);
t_token					*handle_redirect_token(t_gc *gc, char *input, int *i);
t_token					*handle_variable_token(t_gc *gc, char *input, int *i);
void					handle_exec_error(t_gc *gc, t_env *env, char *cmd, char *err_msg);
t_token_type			get_redirect_type(char *input, int *i);
char					*extract_word(t_gc *gc, char *input, int *i);

// Parser functions
t_pipeline				*parse_pipeline(t_gc *gc, t_token *tokens,
							int *incomplete_pipe);
t_cmd					*parse_command(t_gc *gc, t_token **tokens);
char					**parse_args(t_gc *gc, t_token **tokens);
int						is_concatenable_token(t_token *token);
char					*concatenate_tokens(t_gc *gc, t_token **tokens);
t_cmd					*create_command(t_gc *gc);
int						is_redirection_token(t_token *token);
int						validate_syntax(t_token *tokens);
int						handle_input_redirection(t_cmd *cmd, char *filename);
int						handle_output_redirection(t_cmd *cmd, char *filename, int flags);
int						handle_heredoc_redirection(t_gc *gc, t_cmd *cmd, t_token *filename_token);
int						handle_redirection(t_gc *gc, t_cmd *cmd, t_token **tokens);
int						check_redirection_syntax(t_token *current);

// Expander functions
char					*expand_variables(t_gc *gc, t_env *env, char *str,
							int exit_status);
char					*expand_variables_in_double_quotes(t_gc *gc, t_env *env,
							char *str, int exit_status);
char					*expand_env_var(t_gc *gc, t_env *env, char *var_name);

// Utils
int						is_special_char(char c);
int						is_quote(char c);
int						is_whitespace(char c);
int						skip_whitespace(char *str, int *i);

// Heredoc functions
t_heredoc				*add_heredoc(t_gc *gc, t_heredoc **list,
							char *delimiter, int is_quoted);
int						handle_multiple_heredocs(t_gc *gc, t_env *env,
							t_heredoc *heredoc_list);
void					setup_heredoc_child_process(t_gc *gc, t_env *env, int write_fd);
void					process_heredoc_input(t_gc *gc, t_env *env,
							char *delimiter, int write_fd, int is_quoted);

// Redirection functions
int						validate_redirection_token(t_token **tokens);
int						process_file_redirection(t_cmd *cmd, t_token_type type,
							char *filename);

// Execution Engine functions
int						execute_pipeline(t_gc *gc, t_env *env,
							t_pipeline *pipeline);
int						execute_command(t_exec_context *ctx);
int						setup_redirections(t_gc *gc, t_env *env, t_cmd *cmd,
							int *input_fd, int *output_fd);
int						handle_heredoc(t_gc *gc, t_env *env, char *delimiter, int is_quoted);
pid_t					fork_and_exec(t_exec_context *ctx);
char					*find_command_path(t_gc *gc, t_env *env, char *command);
char					*find_command_in_path_any(t_gc *gc, t_env *env,
							char *command);
char					**env_to_array(t_gc *gc, t_env *env);
void					close_pipes(int *pipe_fds, int count);
int						wait_for_processes(pid_t *pids, int count);

// Signal handling
void					setup_signals_interactive(void);
void					setup_signals_child_waiting(void);
void					setup_signals_child_default(void);
void					setup_signals_heredoc_child(t_gc *gc, t_env *env);

// Interactive shell
void					interactive_shell(t_gc *gc, t_env *env);
char					*read_command_line(void);
int						process_command_line(t_gc *gc, t_env *env, char *line,
							int *exit_status);

// Global variable for exit status
extern int				g_exit_status;

#endif