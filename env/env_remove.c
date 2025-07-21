#include "../minishell.h"

static int	should_remove_key(const char *env_var, const char *key, int keylen)
{
	return (ft_strncmp(env_var, key, keylen) == 0 && env_var[keylen] == '=');
}

static void	copy_env_vars(char **new, char **envp, const char *key)
{
	int	keylen;
	int	i;
	int	j;

	keylen = ft_strlen(key);
	i = 0;
	j = 0;
	while (envp[i])
	{
		if (!should_remove_key(envp[i], key, keylen))
			new[j++] = envp[i];
		i++;
	}
	new[j] = NULL;
}

static void	free_removed_vars(char **envp, char **original_envp,
		const char *key)
{
	int	keylen;
	int	i;

	if (envp == original_envp)
		return ;
	keylen = ft_strlen(key);
	i = 0;
	while (envp[i])
	{
		if (should_remove_key(envp[i], key, keylen))
			free(envp[i]);
		i++;
	}
}

void	env_remove_key(t_gc *gc, t_env *env, const char *key)
{
	int		len;
	char	**new;
	char	**old;

	if (!env || !env->envp || !key)
		return ;
	(void)gc;
	len = env_len(env->envp);
	new = malloc(sizeof(char *) * (len + 1));
	if (!new)
		return ;
	old = env->envp;
	copy_env_vars(new, env->envp, key);
	free_removed_vars(env->envp, env->original_envp, key);
	env->envp = new;
	if (old != env->original_envp)
		free(old);
}
