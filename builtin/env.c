#include "../minishell.h"

int	builtin_env(t_gc *gc, t_env *env, char **args)
{
	int	i;

	(void)gc;
	(void)args;
	if (!env || !env->envp)
		return (1);
	i = 0;
	while (env->envp[i])
	{
		printf("%s\n", env->envp[i]);
		i++;
	}
	return (0);
}
