#include "../minishell.h"

pid_t	create_empty_child_process(t_exec_context *ctx)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		signal(SIGPIPE, SIG_DFL);
		if (ctx->input_fd != STDIN_FILENO)
		{
			dup2(ctx->input_fd, STDIN_FILENO);
			close(ctx->input_fd);
		}
		if (ctx->output_fd != STDOUT_FILENO)
		{
			dup2(ctx->output_fd, STDOUT_FILENO);
			close(ctx->output_fd);
		}
		env_cleanup_child(ctx->env);
		gc_free_all(ctx->gc);
		exit(0);
	}
	return (pid);
}

int	handle_empty_in_pipeline(t_exec_context *ctx)
{
	int		redirect_input_fd;
	int		redirect_output_fd;

	if (setup_redirections(ctx->gc, ctx->env, ctx->cmd, &redirect_input_fd,
			&redirect_output_fd) == -1)
		return (create_failed_external_child(ctx));
	close_redirect_fds(redirect_input_fd, redirect_output_fd);
	return (create_empty_child_process(ctx));
}

void	setup_pipe_context(t_exec_context *ctx, t_cmd *current,
		int prev_fd, int *pipe_fds)
{
	ctx->cmd = current;
	ctx->input_fd = prev_fd;
	ctx->output_fd = STDOUT_FILENO;
	if (current->next)
	{
		if (pipe(pipe_fds) == -1)
		{
			perror("pipe");
			return ;
		}
		ctx->output_fd = pipe_fds[1];
	}
}

pid_t	execute_single_command_in_pipeline(t_exec_context *ctx)
{
	if (ctx->cmd->args && ctx->cmd->args[0] && is_builtin(ctx->cmd->args[0]))
		return (handle_builtin_in_pipeline(ctx));
	else if (ctx->cmd->args && ctx->cmd->args[0]
		&& ctx->cmd->args[0][0] != '\0')
		return (handle_external_in_pipeline(ctx));
	else
		return (handle_empty_in_pipeline(ctx));
}

void	cleanup_pipe_fds(t_exec_context *ctx)
{
	if (ctx->input_fd != STDIN_FILENO)
		close(ctx->input_fd);
	if (ctx->output_fd != STDOUT_FILENO)
		close(ctx->output_fd);
}