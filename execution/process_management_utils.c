#include "../minishell.h"

void	close_pipes(int *pipe_fds, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		if (pipe_fds[i] != -1)
			close(pipe_fds[i]);
		i++;
	}
}

int	handle_process_status(int status, int i, int count, int *sigint_received)
{
	int	exit_status;

	exit_status = 0;
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		*sigint_received = 1;
	if (i == count - 1)
	{
		if (WIFEXITED(status))
			exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_status = 128 + WTERMSIG(status);
	}
	return (exit_status);
}

int	wait_for_processes(pid_t *pids, int count)
{
	int	i;
	int	status;
	int	exit_status;
	int	sigint_received;

	exit_status = 0;
	sigint_received = 0;
	i = 0;
	while (i < count)
	{
		if (pids[i] != -1)
		{
			waitpid(pids[i], &status, 0);
			exit_status = handle_process_status(status, i, count,
					&sigint_received);
		}
		i++;
	}
	if (sigint_received)
		printf("\n");
	return (exit_status);
}

void	handle_file_descriptors(int input_fd, int output_fd)
{
	if (input_fd != STDIN_FILENO)
	{
		dup2(input_fd, STDIN_FILENO);
		close(input_fd);
	}
	if (output_fd != STDOUT_FILENO)
	{
		dup2(output_fd, STDOUT_FILENO);
		close(output_fd);
	}
}

void	handle_command_not_found(t_gc *gc, t_env *env, char *cmd_name)
{
	char	*path_file;

	if (!ft_strchr(cmd_name, '/'))
	{
		path_file = find_command_in_path_any(gc, env, cmd_name);
		if (path_file)
		{
			handle_exec_error(gc, env, cmd_name, "Permission denied");
			exit(126);
		}
	}
	handle_exec_error(gc, env, cmd_name, "command not found");
	exit(127);
}
