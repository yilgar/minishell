#include "../minishell.h"

char	*find_command_path(t_gc *gc, t_env *env, char *command)
{
	char	**path_dirs;

	if (!command)
		return (NULL);
	if (ft_strchr(command, '/'))
		return (handle_absolute_path(gc, command));
	path_dirs = get_path_dirs(gc, env);
	if (!path_dirs)
		return (NULL);
	return (search_in_path_dirs(gc, path_dirs, command));
}

static char	*check_command_existence(t_gc *gc, char *command, char *path_dir)
{
	char	*full_path;

	full_path = build_full_path(gc, path_dir, command);
	if (!full_path)
		return (NULL);
	if (access(full_path, F_OK) == 0)
		return (full_path);
	return (NULL);
}

static char	*search_any_in_path_dirs(t_gc *gc, char **path_dirs, char *command)
{
	char	*full_path;
	int		i;

	i = 0;
	while (path_dirs[i])
	{
		full_path = check_command_existence(gc, command, path_dirs[i]);
		if (full_path)
			return (full_path);
		i++;
	}
	return (NULL);
}

char	*find_command_in_path_any(t_gc *gc, t_env *env, char *command)
{
	char	**path_dirs;

	if (!command)
		return (NULL);
	path_dirs = get_path_dirs(gc, env);
	if (!path_dirs)
		return (NULL);
	return (search_any_in_path_dirs(gc, path_dirs, command));
}
