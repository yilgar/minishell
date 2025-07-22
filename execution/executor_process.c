#include "../minishell.h"

int	handle_external_process_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			printf("\n");
		return (128 + WTERMSIG(status));
	}
	return (1);
}

int	execute_external_command(t_exec_context *ctx)
{
	int				redirect_input_fd;
	int				redirect_output_fd;
	pid_t			pid;
	int				status;
	t_exec_context	exec_ctx;

	if (setup_redirections(ctx->gc, ctx->env, ctx->cmd, &redirect_input_fd,
			&redirect_output_fd) == -1)
		return (1);
	exec_ctx = *ctx;
	if (redirect_input_fd != STDIN_FILENO)
		exec_ctx.input_fd = redirect_input_fd;
	if (redirect_output_fd != STDOUT_FILENO)
		exec_ctx.output_fd = redirect_output_fd;
	pid = fork_and_exec(&exec_ctx);
	if (pid == -1)
		return (1);
	setup_signals_child_waiting();
	waitpid(pid, &status, 0);
	setup_signals_interactive();
	close_redirect_fds(redirect_input_fd, redirect_output_fd);
	return (handle_external_process_status(status));
}

int	execute_command(t_exec_context *ctx)
{
	if (!ctx->cmd)
		return (0);
	if (ctx->cmd->redirection_failed && ctx->input_fd == STDIN_FILENO
		&& ctx->output_fd == STDOUT_FILENO)
		return (1);
	if (!ctx->cmd->args || !ctx->cmd->args[0] || ctx->cmd->args[0][0] == '\0')
		return (handle_empty_command(ctx));
	if (is_builtin(ctx->cmd->args[0]))
		return (execute_builtin_with_redirections(ctx));
	return (execute_external_command(ctx));
}

int	count_commands(t_pipeline *pipeline)
{
	t_cmd	*current;
	int		count;

	count = 0;
	current = pipeline->commands;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

void	setup_child_redirections(t_exec_context *ctx, int redirect_input_fd,
		int redirect_output_fd)
{
	if (redirect_input_fd != STDIN_FILENO)
	{
		dup2(redirect_input_fd, STDIN_FILENO);
		close(redirect_input_fd);
	}
	else if (ctx->input_fd != STDIN_FILENO)
	{
		dup2(ctx->input_fd, STDIN_FILENO);
		close(ctx->input_fd);
	}
	if (redirect_output_fd != STDOUT_FILENO)
	{
		dup2(redirect_output_fd, STDOUT_FILENO);
		close(redirect_output_fd);
	}
	else if (ctx->output_fd != STDOUT_FILENO)
	{
		dup2(ctx->output_fd, STDOUT_FILENO);
		close(ctx->output_fd);
	}
}