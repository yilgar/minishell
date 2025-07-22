#include "../minishell.h"

int	process_redirections(t_gc *gc, t_cmd *cmd, t_token **current)
{
	int	redirection_failed;

	redirection_failed = 0;
	while (*current && (*current)->type != TOKEN_PIPE
		&& (*current)->type != TOKEN_EOF)
	{
		if (is_redirection_token(*current))
		{
			if (check_redirection_syntax(*current) == -1)
				return (-1);
			if (handle_redirection(gc, cmd, current) == -1
				&& !redirection_failed)
				redirection_failed = 1;
		}
		else
			*current = (*current)->next;
	}
	if (redirection_failed)
		cmd->redirection_failed = 1;
	return (0);
}

void	setup_default_args_for_heredoc(t_gc *gc, t_cmd *cmd)
{
	if (cmd->heredoc_delimiter && (!cmd->args || !cmd->args[0]))
	{
		cmd->args = gc_track(gc, malloc(sizeof(char *) * 2));
		if (cmd->args)
		{
			cmd->args[0] = gc_track(gc, ft_strdup("cat"));
			cmd->args[1] = NULL;
		}
	}
}

t_cmd	*parse_command(t_gc *gc, t_token **tokens)
{
	t_cmd	*cmd;
	t_token	*current;
	t_token	*start;

	cmd = create_command(gc);
	if (!cmd)
		return (NULL);
	start = *tokens;
	current = *tokens;
	if (process_redirections(gc, cmd, &current) == -1)
		return (NULL);
	*tokens = start;
	cmd->args = parse_args(gc, tokens);
	setup_default_args_for_heredoc(gc, cmd);
	*tokens = current;
	return (cmd);
}
