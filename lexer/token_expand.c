#include "../minishell.h"

void	expand_token_variables(t_gc *gc, t_env *env, t_token *current)
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

void	process_token_expansion(t_gc *gc, t_env *env, t_token *tokens)
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

void	split_expanded_tokens(t_gc *gc, t_token **tokens)
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

t_pipeline	*create_pipeline_from_tokens(t_gc *gc, t_token *tokens,
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