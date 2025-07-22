#include "../minishell.h"

int	g_exit_status = 0;

static void	handle_sigint_interactive(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	g_exit_status = 130;
}

// Ignored SIGINT handler
static void	handle_sigint_ignore(int sig)
{
	(void)sig;
}

// Set interactive mode signals (main shell)
void	setup_signals_interactive(void)
{
	signal(SIGINT, handle_sigint_interactive);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}

// Set signals for child while waiting (parent ignores)
void	setup_signals_child_waiting(void)
{
	signal(SIGINT, handle_sigint_ignore);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}

// Set default signals for executed child process
void	setup_signals_child_default(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGPIPE, SIG_DFL);
}

// Set heredoc child signal handlers
