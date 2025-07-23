#include "minishell.h"

static void	setup_readline(void)
{
	rl_clear_history();
}

char	*read_command_line(void)
{
	char	*line;

	line = readline("minishell> ");
	if (line && *line)
		add_history(line);
	return (line);
}

static int	handle_command_result(char **line, int result, t_gc *gc, t_env *env)
{
	int	exit_status;

	if (result == 1)
		return (1);
	else if (result == 2)
		return (handle_incomplete_pipe(gc, env, line, &exit_status));
	return (0);
}

void	interactive_shell(t_gc *gc, t_env *env)
{
	char	*line;
	int		exit_status;
	int		should_exit;
	int		result;

	exit_status = 0;
	should_exit = 0;
	setup_readline();
	setup_signals_interactive();
	while (!should_exit)
	{
		line = read_command_line();
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		result = process_command_line(gc, env, line, &exit_status);
		write(STDOUT_FILENO, "", 0);
		// rl_on_new_line();
		should_exit = handle_command_result(&line, result, gc, env);
		free(line);
	}
	clear_history();
}

int	main(int argc, char **argv, char **envp)
{
	t_gc	gc;
	t_env	*env;

	(void)argc;
	(void)argv;
	gc_init(&gc);
	env = env_init_copy(&gc, envp);
	if (!env)
	{
		printf("Error: Failed to initialize environment\n");
		return (1);
	}
	interactive_shell(&gc, env);
	env_cleanup(env);
	gc_free_all(&gc);
	return (g_exit_status);
}



//heredoc contrl c 0 ile çıkıyor 1 çıkması lazım

//heredoc control d dogru cıkıyor