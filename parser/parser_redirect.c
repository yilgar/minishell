#include "../minishell.h"

int	handle_input_redirection(t_cmd *cmd, char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		if (!cmd->redirection_failed)
		{
			perror(filename);
			cmd->redirection_failed = 1;
		}
		return (-1);
	}
	close(fd);
	cmd->input_file = filename;
	return (0);
}

int	handle_output_redirection(t_cmd *cmd, char *filename, int flags)
{
	int	fd;

	fd = open(filename, flags, 0644);
	if (fd != -1)
		close(fd);
	else
	{
		if (!cmd->redirection_failed)
		{
			perror(filename);
			cmd->redirection_failed = 1;
		}
		return (-1);
	}
	cmd->output_file = filename;
	return (0);
}

int	process_file_redirection(t_cmd *cmd, t_token_type type,
		char *filename)
{
	int	result;

	result = 0;
	if (type == TOKEN_REDIRECT_IN)
		result = handle_input_redirection(cmd, filename);
	else if (type == TOKEN_REDIRECT_OUT)
		result = handle_output_redirection(cmd, filename,
				O_WRONLY | O_CREAT | O_TRUNC);
	else if (type == TOKEN_REDIRECT_APPEND)
		result = handle_output_redirection(cmd, filename,
				O_WRONLY | O_CREAT | O_APPEND);
	return (result);
}

int	validate_redirection_token(t_token **tokens)
{
	*tokens = (*tokens)->next;
	if (!*tokens || ((*tokens)->type != TOKEN_WORD
			&& (*tokens)->type != TOKEN_SINGLE_QUOTE
			&& (*tokens)->type != TOKEN_DOUBLE_QUOTE
			&& (*tokens)->type != TOKEN_ENV_VAR))
		return (0);
	return (1);
}

int	check_redirection_syntax(t_token *current)
{
	if (!current->next)
		return (0);
	if (current->next->type == TOKEN_PIPE || current->next->type == TOKEN_EOF)
		return (0);
	if (current->next->type == TOKEN_REDIRECT_IN
		|| current->next->type == TOKEN_REDIRECT_OUT
		|| current->next->type == TOKEN_REDIRECT_APPEND
		|| current->next->type == TOKEN_REDIRECT_HEREDOC)
		return (0);
	return (1);
}
