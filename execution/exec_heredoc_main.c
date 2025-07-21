#include "../minishell.h"

static int	handle_heredoc_parent_process(int read_fd, int write_fd, pid_t pid)
{
	int	status;

	close(write_fd);
	waitpid(pid, &status, 0);
	if (g_exit_status == -1)
		g_exit_status = 0;
	if (WIFSIGNALED(status))
	{
		close(read_fd);
		g_exit_status = 130;
		return (-1);
	}
	return (read_fd);
}

int	handle_heredoc(t_gc *gc, t_env *env, char *delimiter, int is_quoted)
{
	int		pipe_fds[2];
	pid_t	pid;

	if (pipe(pipe_fds) == -1)
	{
		perror("pipe");
		return (-1);
	}
	g_exit_status = -1;
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return (-1);
	}
	if (pid == 0)
	{
		setup_heredoc_child_process(gc, env, pipe_fds[0]);
		process_heredoc_input(gc, env, delimiter, pipe_fds[1], is_quoted);
	}
	return (handle_heredoc_parent_process(pipe_fds[0], pipe_fds[1], pid));
}
