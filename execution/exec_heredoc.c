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

void	process_heredoc_input(t_heredoc_context *ctx)
{
	if (!ctx->delimiter)
	{
		cleanup_and_exit(ctx->gc, ctx->env, 0);
	}
	process_heredoc_loop(ctx);
}
