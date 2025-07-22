#include "../minishell.h"

int	validate_input_and_tokenize(t_gc *gc, char *line, t_token **tokens)
{
	if (!line || !*line)
		return (0);
	*tokens = tokenize(gc, line);
	if (!*tokens)
		return (0);
	return (1);
}

void	process_tokens(t_gc *gc, t_env *env, t_token **tokens)
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
		return (2);  // Incomplete pipe - need continuation
	if (!pipeline)
		return (0);  // Syntax error - don't continue
	return (execute_and_cleanup(gc, env, pipeline, exit_status));
}

char	*create_combined_line(t_gc *gc, char *original_line,
		char *continuation_line)
{
	char	*temp_line;
	char	*combined_line;

	temp_line = gc_track(gc, ft_strjoin(original_line, " "));
	combined_line = gc_track(gc, ft_strjoin(temp_line, continuation_line));
	return (combined_line);
}

void	cleanup_line_resources(char **line, char *continuation_line)
{
	free(*line);
	free(continuation_line);
	*line = NULL;
}