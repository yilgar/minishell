#include "../minishell.h"

static char	*get_home_path(t_env *env)
{
	return (env_get_value(env, "HOME"));
}

static int	update_pwd_vars(t_gc *gc, t_env *env, char *old_pwd)
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

int	builtin_cd(t_gc *gc, t_env *env, char **args)
{
	char	*path;
	char	*old_pwd;

	// Check for too many arguments
	if (args[1] && args[2])
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	
	old_pwd = getcwd(NULL, 0);
	if (old_pwd)
		gc_track(gc, old_pwd);
	if (!args[1] || ft_strncmp(args[1], "~", 1) == 0)
	{
		path = get_home_path(env);
		if (!path)
		{
			ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
			return (1);
		}
	}
	else if (ft_strncmp(args[1], "-", 1) == 0 && args[1][1] == '\0')
	{
		path = env_get_value(env, "OLDPWD");
		if (!path)
		{
			ft_putstr_fd("cd: OLDPWD not set\n", STDERR_FILENO);
			return (1);
		}
		printf("%s\n", path);
	}
	else
	{
		path = args[1];
		if (ft_strncmp(path, "$PWD", 4) == 0 && path[4] == '\0')
		{
			path = env_get_value(env, "PWD");
			if (!path)
				path = old_pwd; // fallback to current directory
		}
	}
	if (chdir(path) == -1)
	{
		ft_putstr_fd("cd: ", STDERR_FILENO);
		ft_putstr_fd(path, STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		return (1);
	}
	return (update_pwd_vars(gc, env, old_pwd));
}
