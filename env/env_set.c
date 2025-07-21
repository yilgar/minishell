#include "../minishell.h"

int	env_set(t_gc *gc, t_env *env, const char *key, const char *value)
{
	size_t	key_len;
	char	*new_entry;
	int		i;

	if (!env || !key || !value)
		return (1);
	key_len = ft_strlen(key);
    new_entry = gc_track(gc, ft_strjoin(key, "="));
    new_entry = gc_track(gc, ft_strjoin(new_entry, value));
	if (!new_entry)
		return (1);
	i = 0;
	while (env->envp[i])
	{
		if (!ft_strncmp(env->envp[i], key, key_len)
			&& env->envp[i][key_len] == '=')
		{
			env->envp[i] = new_entry;
			return (0);
		}
		i++;
	}
	env_add_str(gc, env, new_entry);
	return (0);
}
