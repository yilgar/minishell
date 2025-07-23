#include "../minishell.h"

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
			handle_exec_error(gc, env, cmd->args[0],
				"No such file or directory");
		exit(126);
	}
}

static void	execute_child_process(t_exec_context *ctx)
{
	char	*command_path;
	char	**env_array;

	setup_signals_child_default();
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
