#include "../minishell.h"


void	setup_heredoc_child_process(t_gc *gc, t_env *env, int write_fd)
{
	struct termios	term;

	close(write_fd);
	if (tcgetattr(STDIN_FILENO, &term) == 0)
	{
		term.c_lflag &= ~ECHOCTL;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
	}
	setup_signals_heredoc_child(gc, env);
}


static void	write_heredoc_line(t_gc *gc, t_env *env, char *line,
		int write_fd, int is_quoted)
{
	char	*expanded_line;

	if (is_quoted)
		ft_putstr_fd(line, write_fd);
	else
	{
		expanded_line = expand_variables(gc, env, line, 0);
		ft_putstr_fd(expanded_line, write_fd);
	}
	ft_putstr_fd("\n", write_fd);
}

void	process_heredoc_input(t_gc *gc, t_env *env, char *delimiter,
		int write_fd, int is_quoted)
{
	char	*line;
	int		delimiter_len;

	if (!delimiter)
	{	
		gc_free_all(gc);
		env_cleanup(env);
		exit(0);
	}
	delimiter_len = ft_strlen(delimiter);
	while (1)
	{
		line = readline("heredoc> ");
		if (!line)
		{
			gc_free_all(gc);
			env_cleanup(env);
			exit(130);
		}
		if (ft_strncmp(line, delimiter, delimiter_len) == 0
			&& line[delimiter_len] == '\0')
		{
			free(line);
			gc_free_all(gc);
			env_cleanup(env);
			exit(131);
		}
		write_heredoc_line(gc, env, line, write_fd, is_quoted);
		free(line);
	}
}
