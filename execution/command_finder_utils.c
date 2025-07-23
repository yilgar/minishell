#include "../minishell.h"

char	**get_path_dirs(t_gc *gc, t_env *env)
{
	char	*path;
	char	**path_dirs;

	path = env_get_value(env, "PATH");
	if (!path)
		return (NULL);
	path_dirs = ft_split(path, ':');
	if (!path_dirs)
		return (NULL);
	gc_track_array(gc, (void **)path_dirs);
	gc_track(gc, path_dirs);
	return (path_dirs);
}

char	*build_full_path(t_gc *gc, char *path_dir, char *command)
{
	char	*temp;
	char	*full_path;

	temp = gc_track(gc, ft_strjoin(path_dir, "/"));
	if (!temp)
		return (NULL);
	full_path = gc_track(gc, ft_strjoin(temp, command));
	return (full_path);
}

char	*check_command_in_path(t_gc *gc, char *command, char *path_dir)
{
	char	*full_path;

	full_path = build_full_path(gc, path_dir, command);
	if (!full_path)
		return (NULL);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	return (NULL);
}

char	*handle_absolute_path(t_gc *gc, char *command)
{
	if (access(command, F_OK) == 0)
		return (gc_track(gc, ft_strdup(command)));
	return (NULL);
}

char	*search_in_path_dirs(t_gc *gc, char **path_dirs, char *command)
{
	char	*full_path;
	int		i;

	i = 0;
	while (path_dirs[i])
	{
		full_path = check_command_in_path(gc, command, path_dirs[i]);
		if (full_path)
			return (full_path);
		i++;
	}
	return (NULL);
}
