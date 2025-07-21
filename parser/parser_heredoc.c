#include "../minishell.h"

int	handle_heredoc_redirection(t_gc *gc, t_cmd *cmd, t_token *filename_token)
{
	int	is_quoted;

	cmd->heredoc_delimiter = 1;
	cmd->heredoc_delim = filename_token->value;
	is_quoted = (filename_token->type == TOKEN_SINGLE_QUOTE
			|| filename_token->type == TOKEN_DOUBLE_QUOTE);
	cmd->heredoc_is_quoted = is_quoted;
	add_heredoc(gc, &cmd->heredoc_list, filename_token->value, is_quoted);
	filename_token->is_heredoc_delimiter = 1;
	filename_token->is_quoted_delimiter = is_quoted;
	return (0);
}

t_heredoc	*add_heredoc(t_gc *gc, t_heredoc **list, char *delimiter,
		int is_quoted)
{
	t_heredoc	*new_heredoc;
	t_heredoc	*current;

	if (!delimiter)
		return (NULL);
	new_heredoc = gc_track(gc, malloc(sizeof(t_heredoc)));
	if (!new_heredoc)
		return (NULL);
	new_heredoc->delimiter = delimiter;
	new_heredoc->is_quoted = is_quoted;
	new_heredoc->next = NULL;
	if (!*list)
		*list = new_heredoc;
	else
	{
		current = *list;
		while (current->next)
			current = current->next;
		current->next = new_heredoc;
	}
	return (new_heredoc);
}

int	handle_multiple_heredocs(t_gc *gc, t_env *env, t_heredoc *heredoc_list)
{
	t_heredoc	*current;
	int			final_fd;
	int			temp_fd;

	if (!heredoc_list)
		return (-1);
	current = heredoc_list;
	final_fd = -1;
	while (current)
	{
		temp_fd = handle_heredoc(gc, env, current->delimiter,
				current->is_quoted);
		if (temp_fd == -1)
		{
			if (final_fd != -1)
				close(final_fd);
			return (-1);
		}
		if (final_fd != -1)
			close(final_fd);
		final_fd = temp_fd;
		current = current->next;
	}
	return (final_fd);
}
