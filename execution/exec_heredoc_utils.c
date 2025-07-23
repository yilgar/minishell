#include "../minishell.h"

int	check_delimiter_match(char *line, char *delimiter, int delimiter_len)
{
	return (ft_strncmp(line, delimiter, delimiter_len) == 0
		&& line[delimiter_len] == '\0');
}

void	cleanup_and_exit(t_gc *gc, t_env *env, int exit_code)
{
	gc_free_all(gc);
	env_cleanup(env);
	exit(exit_code);
}

void	process_heredoc_loop(t_heredoc_context *ctx)
{
	char	*line;
	int		delimiter_len;

	delimiter_len = ft_strlen(ctx->delimiter);
	while (1)
	{
		line = readline("heredoc> ");
		if (!line)
		{
			cleanup_and_exit(ctx->gc, ctx->env, 130);
		}
		if (check_delimiter_match(line, ctx->delimiter, delimiter_len))
		{
			free(line);
			cleanup_and_exit(ctx->gc, ctx->env, 131);
		}
		write_heredoc_line(ctx, line);
		free(line);
	}
}

void	write_heredoc_line(t_heredoc_context *ctx, char *line)
{
	char	*expanded_line;

	if (ctx->is_quoted)
		ft_putstr_fd(line, ctx->write_fd);
	else
	{
		expanded_line = expand_variables(ctx->gc, ctx->env, line, 0);
		ft_putstr_fd(expanded_line, ctx->write_fd);
	}
	ft_putstr_fd("\n", ctx->write_fd);
}
