#include "../minishell.h"

char	*env_get_value(t_env *env, const char *key)
{
	size_t	len;
	int		i;

	len = ft_strlen(key);
	i = -1;
	while (env->envp[++i])
	{
		if (!ft_strncmp(env->envp[i], key, len) && env->envp[i][len] == '=')
			return (env->envp[i] + len + 1);
	}
	return (NULL);
}
