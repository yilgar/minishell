#include "../minishell.h"

int		g_exit_status = 0;

void	handle_sigint(int sig)
{
	(void)sig;
	printf("\n");
	rl_replace_line("", 0);
	rl_on_new_line();
	if (g_exit_status != -1)
		rl_redisplay();
	g_exit_status = 130;
}

void	handle_sigint_child_running(int sig)
{
	(void)sig;
}

void	handle_sigint_heredoc_parent(int sig)
{
	(void)sig;
}

void	setup_signals(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}

void	setup_signals_child_running(void)
{
	signal(SIGINT, handle_sigint_child_running);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}

void	restore_signals(void)
{
	setup_signals();
}
