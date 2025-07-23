#include "../minishell.h"

char	*get_home_path(t_env *env)
{
	return (env_get_value(env, "HOME"));
}

char	*handle_oldpwd_path(t_env *env)
{
	char	*path;

	path = env_get_value(env, "OLDPWD");
	if (!path)
	{
		ft_putstr_fd("cd: OLDPWD not set\n", STDERR_FILENO);
		return (NULL);
	}
	printf("%s\n", path);
	return (path);
}

char	*handle_home_path(t_env *env)
{
	char	*path;

	path = get_home_path(env);
	if (!path)
	{
		ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
		return (NULL);
	}
	return (path);
}

char	*resolve_path(t_env *env, char **args, char *old_pwd)
{
	char	*path;

	if (!args[1] || ft_strncmp(args[1], "~", 1) == 0)
		return (handle_home_path(env));
	else if (ft_strncmp(args[1], "-", 1) == 0 && args[1][1] == '\0')
		return (handle_oldpwd_path(env));
	else
	{
		path = args[1];
		if (ft_strncmp(path, "$PWD", 4) == 0 && path[4] == '\0')
		{
			path = env_get_value(env, "PWD");
			if (!path)
				path = old_pwd;
		}
		return (path);
	}
}

int	update_pwd_vars(t_gc *gc, t_env *env, char *old_pwd)
{
	char	*new_pwd;
	char	*temp_str;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		perror("cd: getcwd");
		return (1);
	}
	gc_track(gc, new_pwd);
	if (old_pwd)
	{
		env_remove_key(gc, env, "OLDPWD");
		temp_str = gc_track(gc, ft_strjoin("OLDPWD=", old_pwd));
		env_add_str(gc, env, temp_str);
	}
	env_remove_key(gc, env, "PWD");
	temp_str = gc_track(gc, ft_strjoin("PWD=", new_pwd));
	env_add_str(gc, env, temp_str);
	return (0);
}
