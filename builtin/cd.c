#include "../minishell.h"

int	builtin_cd(t_gc *gc, t_env *env, char **args)
{
	char	*path;
	char	*old_pwd;

	if (args[1] && args[2])
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	old_pwd = getcwd(NULL, 0);
	if (old_pwd)
		gc_track(gc, old_pwd);
	path = resolve_path(env, args, old_pwd);
	if (!path)
		return (1);
	if (chdir(path) == -1)
	{
		ft_putstr_fd("cd: ", STDERR_FILENO);
		ft_putstr_fd(path, STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		return (1);
	}
	return (update_pwd_vars(gc, env, old_pwd));
}
