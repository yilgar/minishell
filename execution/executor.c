#include "../minishell.h"

static void	close_redirect_fds(int input_fd, int output_fd)
{
	if (input_fd != STDIN_FILENO)
		close(input_fd);
	if (output_fd != STDOUT_FILENO)
		close(output_fd);
}

static int	handle_empty_command(t_exec_context *ctx)
{
	int	redirect_input_fd;
	int	redirect_output_fd;

	if (setup_redirections(ctx->gc, ctx->env, ctx->cmd, &redirect_input_fd,
			&redirect_output_fd) == -1)
		return (1);
	close_redirect_fds(redirect_input_fd, redirect_output_fd);
	return (0);
}

static void	setup_builtin_redirections(int input_fd, int output_fd)
{
	if (input_fd != STDIN_FILENO)
		dup2(input_fd, STDIN_FILENO);
	if (output_fd != STDOUT_FILENO)
		dup2(output_fd, STDOUT_FILENO);
}

static void	restore_builtin_fds(int saved_stdin, int saved_stdout)
{
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
}

static int	execute_builtin_with_redirections(t_exec_context *ctx)
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

static int	handle_external_process_status(int status)
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

static int	execute_external_command(t_exec_context *ctx)
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
	setup_signals_child_running();
	waitpid(pid, &status, 0);
	restore_signals();
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

static int	count_commands(t_pipeline *pipeline)
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

static void	setup_child_redirections(t_exec_context *ctx, int redirect_input_fd,
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

static void	handle_child_failure(t_exec_context *ctx)
{
	if (ctx->input_fd != STDIN_FILENO)
		close(ctx->input_fd);
	if (ctx->output_fd != STDOUT_FILENO)
		close(ctx->output_fd);
	env_cleanup_child(ctx->env);
	gc_free_all(ctx->gc);
	exit(1);
}

static pid_t	create_builtin_child_process(t_exec_context *ctx)
{
	int		redirect_input_fd;
	int		redirect_output_fd;
	pid_t	pid;
	int		builtin_exit_code;

	pid = fork();
	if (pid == 0)
	{
		signal(SIGPIPE, SIG_DFL);
		if (setup_redirections(ctx->gc, ctx->env, ctx->cmd, &redirect_input_fd,
				&redirect_output_fd) == 0)
			setup_child_redirections(ctx, redirect_input_fd, redirect_output_fd);
		else
			handle_child_failure(ctx);
		builtin_exit_code = execute_builtin(ctx->gc, ctx->env, ctx->cmd->args);
		env_cleanup_child(ctx->env);
		gc_free_all(ctx->gc);
		exit(builtin_exit_code);
	}
	return (pid);
}

static int	handle_builtin_in_pipeline(t_exec_context *ctx)
{
	return (create_builtin_child_process(ctx));
}

static pid_t	create_failed_external_child(t_exec_context *ctx)
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

static int	handle_external_in_pipeline(t_exec_context *ctx)
{
	int				redirect_input_fd;
	int				redirect_output_fd;
	pid_t			pid;
	t_exec_context	exec_ctx;

	if (setup_redirections(ctx->gc, ctx->env, ctx->cmd, &redirect_input_fd,
			&redirect_output_fd) == -1)
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

static pid_t	create_empty_child_process(t_exec_context *ctx)
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

static int	handle_empty_in_pipeline(t_exec_context *ctx)
{
	int		redirect_input_fd;
	int		redirect_output_fd;

	if (setup_redirections(ctx->gc, ctx->env, ctx->cmd, &redirect_input_fd,
			&redirect_output_fd) == -1)
		return (create_failed_external_child(ctx));
	close_redirect_fds(redirect_input_fd, redirect_output_fd);
	return (create_empty_child_process(ctx));
}

static void	setup_pipe_context(t_exec_context *ctx, t_cmd *current,
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

static pid_t	execute_single_command_in_pipeline(t_exec_context *ctx)
{
	if (ctx->cmd->args && ctx->cmd->args[0] && is_builtin(ctx->cmd->args[0]))
		return (handle_builtin_in_pipeline(ctx));
	else if (ctx->cmd->args && ctx->cmd->args[0]
		&& ctx->cmd->args[0][0] != '\0')
		return (handle_external_in_pipeline(ctx));
	else
		return (handle_empty_in_pipeline(ctx));
}

static void	cleanup_pipe_fds(t_exec_context *ctx)
{
	if (ctx->input_fd != STDIN_FILENO)
		close(ctx->input_fd);
	if (ctx->output_fd != STDOUT_FILENO)
		close(ctx->output_fd);
}

static int	execute_pipeline_commands(t_gc *gc, t_env *env,
		t_pipeline *pipeline, pid_t *pids)
{
	t_cmd			*current;
	int				pipe_fds[2];
	int				prev_fd;
	int				i;
	t_exec_context	ctx;

	prev_fd = STDIN_FILENO;
	current = pipeline->commands;
	i = 0;
	ctx.gc = gc;
	ctx.env = env;
	while (current)
	{
		setup_pipe_context(&ctx, current, prev_fd, pipe_fds);
		pids[i] = execute_single_command_in_pipeline(&ctx);
		cleanup_pipe_fds(&ctx);
		if (current->next)
			prev_fd = pipe_fds[0];
		current = current->next;
		i++;
	}
	return (0);
}

static int	execute_single_command_pipeline(t_gc *gc, t_env *env,
		t_pipeline *pipeline)
{
	t_exec_context	ctx;
	int				exit_status;

	ctx.gc = gc;
	ctx.env = env;
	ctx.cmd = pipeline->commands;
	ctx.input_fd = STDIN_FILENO;
	ctx.output_fd = STDOUT_FILENO;
	exit_status = execute_command(&ctx);
	pipeline->exit_status = exit_status;
	return (exit_status);
}

int	execute_pipeline(t_gc *gc, t_env *env, t_pipeline *pipeline)
{
	pid_t	*pids;
	int		cmd_count;
	int		exit_status;

	if (!pipeline || !pipeline->commands)
		return (1);
	cmd_count = count_commands(pipeline);
	pids = gc_track(gc, malloc(sizeof(pid_t) * cmd_count));
	if (!pids)
		return (1);
	if (cmd_count == 1)
		return (execute_single_command_pipeline(gc, env, pipeline));
	if (execute_pipeline_commands(gc, env, pipeline, pids) == 1)
		return (1);
	setup_signals_child_running();
	exit_status = wait_for_processes(pids, cmd_count);
	restore_signals();
	pipeline->exit_status = exit_status;
	return (exit_status);
}
