#include "../minishell.h"

static int	process_export_arg(t_gc *gc, t_env *env, char *arg)
{
	char	*equals;

	if (!is_valid_identifier(arg))
		return (handle_invalid_identifier(arg));
	equals = ft_strchr(arg, '=');
	if (equals)
		handle_var_with_value(gc, env, arg);
	else
		handle_var_without_value(gc, env, arg);
	return (0);
}

int	builtin_export(t_gc *gc, t_env *env, char **args)
{
	int	i;
	int	ret;

	ret = 0;
	if (!args[1])
	{
		print_exported_vars(env);
		return (0);
	}
	i = 1;
	while (args[i])
	{
		ret |= process_export_arg(gc, env, args[i]);
		i++;
	}
	return (ret);
}
