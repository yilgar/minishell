#include "../minishell.h"

int	is_concatenable_token(t_token *token)
{
	return (token && (token->type == TOKEN_WORD
			|| token->type == TOKEN_SINGLE_QUOTE
			|| token->type == TOKEN_DOUBLE_QUOTE
			|| token->type == TOKEN_EXIT_STATUS
			|| token->type == TOKEN_ENV_VAR));
}

char	*concatenate_tokens(t_gc *gc, t_token **tokens)
{
	char	*result;
	char	*temp;
	t_token	*current;

	if (!*tokens || !is_concatenable_token(*tokens))
		return (NULL);
	if ((*tokens)->value)
		result = gc_track(gc, ft_strdup((*tokens)->value));
	else
		result = gc_track(gc, ft_strdup(""));
	current = (*tokens)->next;
	while (current && is_concatenable_token(current)
		&& !current->has_space_before)
	{
		temp = result;
		if (current->value)
			result = gc_track(gc, ft_strjoin(temp, current->value));
		else
			result = gc_track(gc, ft_strjoin(temp, ""));
		current = current->next;
	}
	*tokens = current;
	return (result);
}

t_cmd	*create_command(t_gc *gc)
{
	t_cmd	*cmd;

	cmd = gc_track(gc, malloc(sizeof(t_cmd)));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->append_mode = 0;
	cmd->heredoc_delimiter = 0;
	cmd->heredoc_delim = NULL;
	cmd->heredoc_is_quoted = 0;
	cmd->heredoc_list = NULL;
	cmd->redirection_failed = 0;
	cmd->next = NULL;
	return (cmd);
}

int	is_redirection_token(t_token *token)
{
	if (!token)
		return (0);
	return (token->type == TOKEN_REDIRECT_IN
		|| token->type == TOKEN_REDIRECT_OUT
		|| token->type == TOKEN_REDIRECT_APPEND
		|| token->type == TOKEN_REDIRECT_HEREDOC);
}
