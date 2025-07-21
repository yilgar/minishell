#include "minishell.h"

static void	setup_readline(void)
{
	rl_clear_history();
}

char	*read_command_line(void)
{
	char	*line;

	line = readline("minishell> ");
	if (line && *line)
		add_history(line);
	return (line);
}

static t_token	*find_token_position(t_token **tokens, t_token *token_to_split,
		t_token **prev_token)
{
	t_token	*current;

	*prev_token = NULL;
	current = *tokens;
	while (current && current != token_to_split)
	{
		*prev_token = current;
		current = current->next;
	}
	return (current);
}

static void	link_token_to_list(t_token **tokens, t_token **current,
		t_token *prev_token, t_token *new_token)
{
	if (!*current)
	{
		if (prev_token)
			prev_token->next = new_token;
		else
			*tokens = new_token;
		*current = new_token;
	}
	else
	{
		(*current)->next = new_token;
		*current = new_token;
	}
}

static t_token	*create_and_configure_token(t_gc *gc, char *word, int has_space)
{
	t_token	*new_token;

	new_token = create_token(gc, TOKEN_WORD, gc_track(gc, ft_strdup(word)));
	if (new_token)
	{
		new_token->has_space_before = has_space;
		new_token->is_heredoc_delimiter = 0;
	}
	return (new_token);
}

static void	create_tokens_from_words(t_gc *gc, t_token **tokens, char **words,
		t_token *prev_token)
{
	t_token	*current;
	t_token	*new_token;
	int		i;

	i = 0;
	current = NULL;
	while (words[i])
	{
		if (words[i][0] != '\0')
		{
			new_token = create_and_configure_token(gc, words[i], (i > 0));
			if (new_token)
			{
				link_token_to_list(tokens, &current, prev_token, new_token);
				prev_token = NULL;
			}
		}
		i++;
	}
}

static void	link_tokens_to_original(t_token **tokens, t_token *token_to_split)
{
	t_token	*current;

	current = *tokens;
	while (current && current->next != token_to_split->next)
		current = current->next;
	if (current)
		current->next = token_to_split->next;
}

static void	split_token_by_whitespace(t_gc *gc, t_token **tokens,
		t_token *token_to_split)
{
	char	**words;
	t_token	*prev_token;

	if (!token_to_split || !token_to_split->value
		|| !ft_strchr(token_to_split->value, ' '))
		return ;
	find_token_position(tokens, token_to_split, &prev_token);
	words = ft_split(token_to_split->value, ' ');
	if (!words)
		return ;
	gc_track_array(gc, (void **)words);
	gc_track(gc, words);
	create_tokens_from_words(gc, tokens, words, prev_token);
	link_tokens_to_original(tokens, token_to_split);
}

static void	mark_heredoc_delimiters(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current && current->type != TOKEN_EOF)
	{
		if (current->type == TOKEN_REDIRECT_HEREDOC && current->next)
		// {
			current->next->is_heredoc_delimiter = 1;
		// }
		current = current->next;
	}
}

static void	set_token_properties(t_token *token, char *value, t_token_type type)
{
	token->value = value;
	token->type = type;
}

static void	expand_env_var_token(t_gc *gc, t_env *env, t_token *current)
{
	char	*var_name;
	char	*expanded;
	char	*new_value;

	var_name = current->value + 1;
	if (!var_name || var_name[0] == '\0')
		new_value = gc_track(gc, ft_strdup("$"));
	else
	{
		expanded = env_get_value(env, var_name);
		if (expanded)
			new_value = gc_track(gc, ft_strdup(expanded));
		else
			new_value = gc_track(gc, ft_strdup(""));
	}
	set_token_properties(current, new_value, TOKEN_WORD);
	current->from_env_expansion = 1;
}

static void	expand_exit_status_token(t_gc *gc, t_token *current)
{
	char	*new_value;

	new_value = gc_track(gc, ft_itoa(g_exit_status));
	set_token_properties(current, new_value, TOKEN_WORD);
}

static void	expand_token_variables(t_gc *gc, t_env *env, t_token *current)
{
	if (current->type == TOKEN_DOUBLE_QUOTE)
	// {
		current->value = expand_variables_in_double_quotes(gc, env,
				current->value, g_exit_status);
	// }
	else if (current->type == TOKEN_WORD)
	// {
		current->value = expand_variables(gc, env, current->value,
				g_exit_status);
	// }
	else if (current->type == TOKEN_ENV_VAR)
	// {
		expand_env_var_token(gc, env, current);
	// }
	else if (current->type == TOKEN_EXIT_STATUS)
	// {
		expand_exit_status_token(gc, current);
	// }
}

static void	process_token_expansion(t_gc *gc, t_env *env, t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current && current->type != TOKEN_EOF)
	{
		if (current->is_heredoc_delimiter)
		{
			current = current->next;
			continue ;
		}
		expand_token_variables(gc, env, current);
		current = current->next;
	}
}

static void	split_expanded_tokens(t_gc *gc, t_token **tokens)
{
	t_token	*current;
	t_token	*next;

	current = *tokens;
	while (current && current->type != TOKEN_EOF)
	{
		next = current->next;
		if (current->type == TOKEN_WORD && current->from_env_expansion
			&& current->value && ft_strchr(current->value, ' '))
		// {
			split_token_by_whitespace(gc, tokens, current);
		// }
		current = next;
	}
}

static t_pipeline	*create_pipeline_from_tokens(t_gc *gc, t_token *tokens,
		int *exit_status, int *is_incomplete_pipe)
{
	t_pipeline	*pipeline;
	int			incomplete_pipe;

	incomplete_pipe = 0;
	*is_incomplete_pipe = 0;
	pipeline = parse_pipeline(gc, tokens, &incomplete_pipe);
	if (incomplete_pipe)
	{
		*is_incomplete_pipe = 1;
		return (NULL);
	}
	if (!pipeline)
	{
		*exit_status = 2;
		g_exit_status = 2;
		return (NULL);
	}
	return (pipeline);
}

static int	execute_and_cleanup(t_gc *gc, t_env *env, t_pipeline *pipeline,
		int *exit_status)
{
	if (!pipeline->commands)
		return (0);
	*exit_status = execute_pipeline(gc, env, pipeline);
	g_exit_status = *exit_status;
	gc_free_all(gc);
	return (0);
}

static int	validate_input_and_tokenize(t_gc *gc, char *line, t_token **tokens)
{
	if (!line || !*line)
		return (0);
	*tokens = tokenize(gc, line);
	if (!*tokens)
		return (0);
	return (1);
}

static void	process_tokens(t_gc *gc, t_env *env, t_token **tokens)
{
	mark_heredoc_delimiters(*tokens);
	process_token_expansion(gc, env, *tokens);
	split_expanded_tokens(gc, tokens);
}

int	process_command_line(t_gc *gc, t_env *env, char *line, int *exit_status)
{
	t_token		*tokens;
	t_pipeline	*pipeline;
	int			is_incomplete_pipe;

	if (!validate_input_and_tokenize(gc, line, &tokens))
		return (0);
	process_tokens(gc, env, &tokens);
	pipeline = create_pipeline_from_tokens(gc, tokens, exit_status, &is_incomplete_pipe);
	if (is_incomplete_pipe)
	{
		return (2);  // Incomplete pipe - need continuation
	}
	if (!pipeline)
	{
		return (0);  // Syntax error - don't continue
	}
	return (execute_and_cleanup(gc, env, pipeline, exit_status));
}

static char	*create_combined_line(t_gc *gc, char *original_line,
		char *continuation_line)
{
	char	*temp_line;
	char	*combined_line;

	temp_line = gc_track(gc, ft_strjoin(original_line, " "));
	combined_line = gc_track(gc, ft_strjoin(temp_line, continuation_line));
	return (combined_line);
}

static void	cleanup_line_resources(char **line, char *continuation_line)
{
	free(*line);
	free(continuation_line);
	*line = NULL;
}

static int	handle_incomplete_pipe(t_gc *gc, t_env *env, char **line,
		int *exit_status)
{
	char	*continuation_line;
	char	*combined_line;

	continuation_line = readline("> ");
	if (!continuation_line)
		return (0);
	combined_line = create_combined_line(gc, *line, continuation_line);
	cleanup_line_resources(line, continuation_line);
	return (process_command_line(gc, env, combined_line, exit_status));
}

static int	handle_command_result(char **line, int result, t_gc *gc, t_env *env)
{
	int	exit_status;

	if (result == 1)
		return (1);
	else if (result == 2)
		return (handle_incomplete_pipe(gc, env, line, &exit_status));
	return (0);
}

void	interactive_shell(t_gc *gc, t_env *env)
{
	char	*line;
	int		exit_status;
	int		should_exit;
	int		result;

	exit_status = 0;
	should_exit = 0;
	setup_readline();
	setup_signals();
	while (!should_exit)
	{
		line = read_command_line();
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		result = process_command_line(gc, env, line, &exit_status);
		// fflush(stdout);  // Tüm output stream'leri flush eder
		write(STDOUT_FILENO, "", 0);  // Boş write ile flush tetiklenir fflush(stdout); sildim
		rl_on_new_line();
		should_exit = handle_command_result(&line, result, gc, env);
		free(line);
	}
	clear_history();
}

int	main(int argc, char **argv, char **envp)
{
	t_gc	gc;
	t_env	*env;

	(void)argc;
	(void)argv;
	gc_init(&gc);
	env = env_init_copy(&gc, envp);
	if (!env)
	{
		printf("Error: Failed to initialize environment\n");
		return (1);
	}
	interactive_shell(&gc, env);
	env_cleanup(env);
	gc_free_all(&gc);
	return (g_exit_status);
}
