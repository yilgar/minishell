#include "../minishell.h"

void	print_exported_vars(t_env *env)
{
	int		i;
	char	*equals;
	char	*key;
	char	*value;

	i = 0;
	while (env->envp[i])
	{
		equals = ft_strchr(env->envp[i], '=');
		if (equals)
		{
			key = ft_substr(env->envp[i], 0, equals - env->envp[i]);
			value = equals + 1;
			printf("declare -x %s=\"%s\"\n", key, value);
			free(key);
		}
		else
			printf("declare -x %s\n", env->envp[i]);
		i++;
	}
}

int	is_valid_identifier(char *str)
{
	int	i;

	if (!str || str[0] == '\0')
		return (0);
	if (str[0] == '=' || ft_isdigit(str[0]))
		return (0);
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	
	return (1);
}

int	handle_invalid_identifier(char *arg)
{
	ft_putstr_fd("export: `", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
	return (1);
}

void	handle_var_with_value(t_gc *gc, t_env *env, char *arg)
{
	env_add_str(gc, env, arg);
}

void	handle_var_without_value(t_gc *gc, t_env *env, char *arg)
{
	char	*var_with_equals;

	if (!env_get_value(env, arg))
	{
		var_with_equals = gc_track(gc, ft_strjoin(arg, "="));
		env_add_str(gc, env, var_with_equals);
	}
}
