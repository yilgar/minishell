#include "../minishell.h"

static int	setup_input_redirection(t_cmd *cmd, int *input_fd)
{
	if (cmd->input_file)
	{
		*input_fd = open(cmd->input_file, O_RDONLY);
		if (*input_fd == -1)
		{
			perror(cmd->input_file);
			return (-1);
		}
	}
	return (0);
}

static int	setup_output_redirection(t_cmd *cmd, int *output_fd)
{
	int	flags;

	if (cmd->output_file)
	{
		flags = O_WRONLY | O_CREAT;
		if (cmd->append_mode)
			flags |= O_APPEND;
		else
			flags |= O_TRUNC;
		*output_fd = open(cmd->output_file, flags, 0644);
		if (*output_fd == -1)
		{
			perror(cmd->output_file);
			return (-1);
		}
	}
	return (0);
}

static int	setup_heredoc_redirection(t_exec_context *ctx, int *input_fd)
{
	if (ctx->cmd->heredoc_list)
		*input_fd = handle_multiple_heredocs(ctx->gc, ctx->env,
				ctx->cmd->heredoc_list);
	else
		*input_fd = handle_heredoc(ctx->gc, ctx->env, ctx->cmd->heredoc_delim,
				ctx->cmd->heredoc_is_quoted);
	if (*input_fd == -1)
		return (-1);
	return (0);
}

int	setup_redirections(t_exec_context *ctx, int *input_fd, int *output_fd)
{
	*input_fd = STDIN_FILENO;
	*output_fd = STDOUT_FILENO;
	if (ctx->cmd->heredoc_delimiter)
	{
		if (setup_heredoc_redirection(ctx, input_fd) == -1)
			return (-1);
	}
	else if (setup_input_redirection(ctx->cmd, input_fd) == -1)
		return (-1);
	if (setup_output_redirection(ctx->cmd, output_fd) == -1)
	{
		if (*input_fd != STDIN_FILENO)
			close(*input_fd);
		return (-1);
	}
	return (0);
}
