#include "../minishell.h"

int	g_exit_status = 0;

t_gc	*heredoc_gc_store(t_gc *set)
{
	static t_gc	*gc_ptr = NULL;

	if (set)
		gc_ptr = set;
	return gc_ptr;
}

t_env	*heredoc_env_store(t_env *set)
{
	static t_env	*env_ptr = NULL;

	if (set)
		env_ptr = set;
	return env_ptr;
}

void	heredoc_sigint_handler(int sig)
{
	t_gc	*gc = heredoc_gc_store(NULL);
	t_env	*env = heredoc_env_store(NULL);

	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	if (gc)
		gc_free_all(gc);
	if (env)
		env_cleanup(env);
	exit(130);
}

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
void	setup_signals_heredoc_child(t_gc *gc, t_env *env)
{
	heredoc_gc_store(gc);
	heredoc_env_store(env);
	signal(SIGINT, heredoc_sigint_handler);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}
