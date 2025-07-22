#include "../minishell.h"



static int	find_var_index(char **envp, const char *name)
{
	int	i;
	int	name_len;

	i = 0;
	name_len = ft_strlen(name);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], name, name_len) == 0
			&& envp[i][name_len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

static void	replace_existing_var(t_env *env, int existing_idx, const char *str)
{
	char	*old_str;

	old_str = env->envp[existing_idx];
	env->envp[existing_idx] = ft_strdup(str);
	if (env->envp != env->original_envp)
		free(old_str);
}

static char	**create_new_env_array(t_env *env, int len)
{
	char	**new;
	int		i;

	new = malloc(sizeof(char *) * (len + 2));
	if (!new)
		return (NULL);
	i = -1;
	while (++i < len)
		new[i] = env->envp[i];
	return (new);
}

static void	add_new_var_to_env(t_env *env, const char *str)
{
	char	**new;
	char	**old;
	int		len;

	len = env_len(env->envp);
	new = create_new_env_array(env, len);
	if (!new)
		return ;
	old = env->envp;
	new[len] = ft_strdup(str);
	new[len + 1] = NULL;
	env->envp = new;
	if (old != env->original_envp)
		free(old);
}

void	env_add_str(t_gc *gc, t_env *env, const char *str)
{
	char	*var_name;
	int		existing_idx;

	(void)gc;
	var_name = get_var_name(str);
	if (!var_name)
		return ;
	existing_idx = find_var_index(env->envp, var_name);
	free(var_name);
	if (existing_idx >= 0)
		replace_existing_var(env, existing_idx, str);
	else
		add_new_var_to_env(env, str);
}
