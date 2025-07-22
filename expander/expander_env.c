#include "../minishell.h"

static int	get_var_name_length(char *var_start)
{
	char	*var_end;

	var_end = var_start;
	while (*var_end && (ft_isalnum(*var_end) || *var_end == '_'))
		var_end++;
	return (var_end - var_start);
}

static char	*extract_var_name(t_gc *gc, char *var_start, int var_len)
{
	char	*var_name;

	var_name = gc_track(gc, malloc(var_len + 1));
	ft_strlcpy(var_name, var_start, var_len + 1);
	return (var_name);
}

static char	*build_new_result(t_gc *gc, char *result, char *var_value,
		char *remaining_str)
{
	char	*new_result;
	int		total_len;

	total_len = ft_strlen(result) + ft_strlen(var_value) + ft_strlen(remaining_str);
	new_result = gc_track(gc, malloc(total_len + 1));
	ft_strlcpy(new_result, result, ft_strlen(result) + 1);
	ft_strlcat(new_result, var_value, total_len + 1);
	ft_strlcat(new_result, remaining_str, total_len + 1);
	return (new_result);
}

static char	*process_single_variable(t_gc *gc, t_env *env, char *result, char *pos)
{
	char	*var_start;
	char	*var_name;
	char	*var_value;
	int		var_len;

	var_start = pos + 1;
	var_len = get_var_name_length(var_start);
	if (var_len == 0)
		return (NULL);
	var_name = extract_var_name(gc, var_start, var_len);
	var_value = expand_env_var(gc, env, var_name);
	if (!var_value)
		var_value = "";
	*pos = '\0';
	return (build_new_result(gc, result, var_value, var_start + var_len));
}

char	*replace_env_vars(t_gc *gc, t_env *env, char *str)
{
	char	*result;
	char	*pos;
	char	*new_result;

	result = gc_track(gc, ft_strdup(str));
	pos = ft_strchr(result, '$');
	while (pos && pos[1] && pos[1] != '?')
	{
		if (get_var_name_length(pos + 1) == 0)
		{
			pos = ft_strchr(pos + 1, '$');
			continue ;
		}
		new_result = process_single_variable(gc, env, result, pos);
		if (!new_result)
		{
			pos = ft_strchr(pos + 1, '$');
			continue ;
		}
		result = new_result;
		pos = ft_strchr(result, '$');
	}
	return (result);
}
