#include "../minishell.h"

void	handle_child_failure(t_exec_context *ctx)
{
	if (ctx->input_fd != STDIN_FILENO)
		close(ctx->input_fd);
	if (ctx->output_fd != STDOUT_FILENO)
		close(ctx->output_fd);
	env_cleanup_child(ctx->env);
	gc_free_all(ctx->gc);
	exit(1);
}

pid_t	create_builtin_child_process(t_exec_context *ctx)
{
	int		redirect_input_fd;
	int		redirect_output_fd;
	pid_t	pid;
	int		builtin_exit_code;

	pid = fork();
	if (pid == 0)
	{
		signal(SIGPIPE, SIG_DFL);
		if (setup_redirections(ctx, &redirect_input_fd,
				&redirect_output_fd) == 0)
			setup_child_redirections(ctx, redirect_input_fd,
				redirect_output_fd);
		else
			handle_child_failure(ctx);
		builtin_exit_code = execute_builtin(ctx->gc, ctx->env, ctx->cmd->args);
		env_cleanup_child(ctx->env);
		gc_free_all(ctx->gc);
		exit(builtin_exit_code);
	}
	return (pid);
}

int	handle_builtin_in_pipeline(t_exec_context *ctx)
{
	return (create_builtin_child_process(ctx));
}

pid_t	create_failed_external_child(t_exec_context *ctx)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		signal(SIGPIPE, SIG_DFL);
		handle_child_failure(ctx);
	}
	return (pid);
}

int	handle_external_in_pipeline(t_exec_context *ctx)
{
	int				redirect_input_fd;
	int				redirect_output_fd;
	pid_t			pid;
	t_exec_context	exec_ctx;

	if (setup_redirections(ctx, &redirect_input_fd, &redirect_output_fd) == -1)
		return (create_failed_external_child(ctx));
	exec_ctx = *ctx;
	if (redirect_input_fd != STDIN_FILENO)
		exec_ctx.input_fd = redirect_input_fd;
	if (redirect_output_fd != STDOUT_FILENO)
		exec_ctx.output_fd = redirect_output_fd;
	pid = fork_and_exec(&exec_ctx);
	close_redirect_fds(redirect_input_fd, redirect_output_fd);
	return (pid);
}
