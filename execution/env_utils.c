#include "../minishell.h"

char	**env_to_array(t_gc *gc, t_env *env)
{
	char	**env_array;
	int		len;
	int		i;

	if (!env || !env->envp)
		return (NULL);
	len = env_len(env->envp);
	env_array = gc_track(gc, malloc(sizeof(char *) * (len + 1)));
	if (!env_array)
		return (NULL);
	i = 0;
	while (i < len)
	{
		env_array[i] = env->envp[i];
		i++;
	}
	env_array[len] = NULL;
	return (env_array);
}
