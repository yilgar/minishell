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

static int	handle_process_status(int status, int i, int count, int *sigint_received)
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
			exit_status = handle_process_status(status, i, count, &sigint_received);
		}
		i++;
	}
	if (sigint_received)
		printf("\n");
	return (exit_status);
}

static void	handle_file_descriptors(int input_fd, int output_fd)
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

static void	handle_command_not_found(t_gc *gc, t_env *env, char *cmd_name)
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

static void	check_directory_and_permissions(t_gc *gc, t_env *env, t_cmd *cmd,
		char *command_path)
{
	struct stat	st;

	if (stat(command_path, &st) == 0)
	{
		if (S_ISDIR(st.st_mode))
		{
			handle_exec_error(gc, env, cmd->args[0], "Is a directory");
			exit(126);
		}
	}
	if (access(command_path, X_OK) != 0)
	{
		if (access(command_path, F_OK) == 0)
			handle_exec_error(gc, env, cmd->args[0], "Permission denied");
		else
			handle_exec_error(gc, env, cmd->args[0], "No such file or directory");
		exit(126);
	}
}

static void	execute_child_process(t_exec_context *ctx)
{
	char	*command_path;
	char	**env_array;

	signal(SIGPIPE, SIG_DFL);
	handle_file_descriptors(ctx->input_fd, ctx->output_fd);
	command_path = find_command_path(ctx->gc, ctx->env, ctx->cmd->args[0]);
	if (!command_path)
		handle_command_not_found(ctx->gc, ctx->env, ctx->cmd->args[0]);
	check_directory_and_permissions(ctx->gc, ctx->env, ctx->cmd, command_path);
	env_array = env_to_array(ctx->gc, ctx->env);
	if (!env_array)
	{
		env_cleanup_child(ctx->env);
		gc_free_all(ctx->gc);
		exit(1);
	}
	execve(command_path, ctx->cmd->args, env_array);
	perror("execve");
	env_cleanup_child(ctx->env);
	gc_free_all(ctx->gc);
	exit(1);
}

pid_t	fork_and_exec(t_exec_context *ctx)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		execute_child_process(ctx);
	return (pid);
}
