#include "../minishell.h"

void	close_redirect_fds(int input_fd, int output_fd)
{
	if (input_fd != STDIN_FILENO)
		close(input_fd);
	if (output_fd != STDOUT_FILENO)
		close(output_fd);
}

int	handle_empty_command(t_exec_context *ctx)
{
	int	redirect_input_fd;
	int	redirect_output_fd;

	if (setup_redirections(ctx->gc, ctx->env, ctx->cmd, &redirect_input_fd,
			&redirect_output_fd) == -1)
		return (1);
	close_redirect_fds(redirect_input_fd, redirect_output_fd);
	return (0);
}

void	setup_builtin_redirections(int input_fd, int output_fd)
{
	if (input_fd != STDIN_FILENO)
		dup2(input_fd, STDIN_FILENO);
	if (output_fd != STDOUT_FILENO)
		dup2(output_fd, STDOUT_FILENO);
}

void	restore_builtin_fds(int saved_stdin, int saved_stdout)
{
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
}

int	execute_builtin_with_redirections(t_exec_context *ctx)
{
	int	redirect_input_fd;
	int	redirect_output_fd;
	int	saved_stdin;
	int	saved_stdout;
	int	exit_code;

	if (setup_redirections(ctx->gc, ctx->env, ctx->cmd, &redirect_input_fd,
			&redirect_output_fd) == -1)
		return (1);
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	setup_builtin_redirections(redirect_input_fd, redirect_output_fd);
	exit_code = execute_builtin(ctx->gc, ctx->env, ctx->cmd->args);
	restore_builtin_fds(saved_stdin, saved_stdout);
	close_redirect_fds(redirect_input_fd, redirect_output_fd);
	return (exit_code);
}