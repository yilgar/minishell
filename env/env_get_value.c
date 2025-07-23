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

char	*get_var_name(const char *str)
{
	char	*eq_pos;
	int		len;
	char	*name;

	eq_pos = ft_strchr(str, '=');
	if (!eq_pos)
		return (ft_strdup(str));
	len = eq_pos - str;
	name = malloc(len + 1);
	if (!name)
		return (NULL);
	ft_strlcpy(name, str, len + 1);
	return (name);
}
