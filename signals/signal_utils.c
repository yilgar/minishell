#include "../minishell.h"

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

void	setup_signals_heredoc_child(t_gc *gc, t_env *env)
{
	heredoc_gc_store(gc);
	heredoc_env_store(env);
	signal(SIGINT, heredoc_sigint_handler);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}
