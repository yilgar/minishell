#include "../minishell.h"

static int	handle_heredoc_parent_process(int read_fd, int write_fd, pid_t pid)
{
	int	status;

	close(write_fd);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		close(read_fd);
		if (WTERMSIG(status) == SIGINT)
			g_exit_status = 130;
		return (-1);
	}
	return (read_fd);
}

static int	create_heredoc_pipe(int pipe_fds[2])
{
	if (pipe(pipe_fds) == -1)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}

static pid_t	fork_heredoc_process(int pipe_fds[2])
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return (-1);
	}
	return (pid);
}

static void	setup_heredoc_child(t_heredoc_context *ctx, int pipe_fds[2])
{
	setup_heredoc_child_process(ctx->gc, ctx->env, pipe_fds[0]);
	ctx->write_fd = pipe_fds[1];
	process_heredoc_input(ctx);
}

int	handle_heredoc(t_gc *gc, t_env *env, char *delimiter, int is_quoted)
{
	int					pipe_fds[2];
	pid_t				pid;
	t_heredoc_context	ctx;

	if (create_heredoc_pipe(pipe_fds) == -1)
		return (-1);
	pid = fork_heredoc_process(pipe_fds);
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		ctx.gc = gc;
		ctx.env = env;
		ctx.delimiter = delimiter;
		ctx.is_quoted = is_quoted;
		setup_heredoc_child(&ctx, pipe_fds);
	}
	return (handle_heredoc_parent_process(pipe_fds[0], pipe_fds[1], pid));
}
