#include "../minishell.h"

int	execute_and_cleanup(t_gc *gc, t_env *env, t_pipeline *pipeline,
		int *exit_status)
{
	if (!pipeline->commands)
		return (0);
	*exit_status = execute_pipeline(gc, env, pipeline);
	g_exit_status = *exit_status;
	gc_free_all(gc);
	return (0);
}



int	handle_incomplete_pipe(t_gc *gc, t_env *env, char **line,
		int *exit_status)
{
	char	*continuation_line;
	char	*combined_line;

	continuation_line = readline("> ");
	if (!continuation_line)
		return (0);
	combined_line = create_combined_line(gc, *line, continuation_line);
	cleanup_line_resources(line, continuation_line);
	return (process_command_line(gc, env, combined_line, exit_status));
}