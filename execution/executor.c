#include "../minishell.h"

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
	setup_signals_child_waiting();
	exit_status = wait_for_processes(pids, cmd_count);
	setup_signals_interactive();
	pipeline->exit_status = exit_status;
	return (exit_status);
}
