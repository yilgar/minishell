#include "../minishell.h"

char	*expand_env_var(t_gc *gc, t_env *env, char *var_name)
{
	char	*value;

	if (!var_name)
		return (NULL);
	value = env_get_value(env, var_name);
	if (value)
		return (gc_track(gc, ft_strdup(value)));
	return (gc_track(gc, ft_strdup("")));
}

static char	*replace_exit_status(t_gc *gc, char *str, int exit_status)
{
	char	*result;
	char	*exit_str;
	char	*pos;
	int		before_len;
	int		after_len;
	int		total_len;

	pos = ft_strnstr(str, "$?", ft_strlen(str));
	if (!pos)
		return (gc_track(gc, ft_strdup(str)));
	exit_str = gc_track(gc, ft_itoa(exit_status));
	before_len = pos - str;
	after_len = ft_strlen(pos + 2);
	total_len = before_len + ft_strlen(exit_str) + after_len;
	result = gc_track(gc, malloc(total_len + 1));
	if (!result)
		return (NULL);
	ft_strlcpy(result, str, before_len + 1);
	ft_strlcat(result, exit_str, total_len + 1);
	ft_strlcat(result, pos + 2, total_len + 1);
	return (result);
}

static char	*replace_env_vars(t_gc *gc, t_env *env, char *str)
{
	char	*result;
	char	*pos;
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	int		var_len;
	int		before_len;
	int		after_len;
	int		total_len;
	char	*new_result;

	result = gc_track(gc, ft_strdup(str));
	pos = ft_strchr(result, '$');
	while (pos && pos[1] && pos[1] != '?')
	{
		var_start = pos + 1;
		var_end = var_start;
		while (*var_end && (ft_isalnum(*var_end) || *var_end == '_'))
			var_end++;
		var_len = var_end - var_start;
		if (var_len == 0)
		{
			pos = ft_strchr(pos + 1, '$');
			continue ;
		}
		var_name = gc_track(gc, malloc(var_len + 1));
		ft_strlcpy(var_name, var_start, var_len + 1);
		var_value = expand_env_var(gc, env, var_name);
		if (!var_value)
			var_value = "";
		before_len = pos - result;
		after_len = ft_strlen(var_end);
		total_len = before_len + ft_strlen(var_value) + after_len;
		new_result = gc_track(gc, malloc(total_len + 1));
		ft_strlcpy(new_result, result, before_len + 1);
		ft_strlcat(new_result, var_value, total_len + 1);
		ft_strlcat(new_result, var_end, total_len + 1);
		result = new_result;
		pos = ft_strchr(result + before_len + ft_strlen(var_value), '$');
	}
	return (result);
}

static char	*process_single_quote_segment(t_gc *gc, char *str, int *i)
{
	int		start;
	char	*segment;

	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != '\'')
		(*i)++;
	if (str[*i] == '\'')
	{
		segment = gc_track(gc, malloc(*i - start + 1));
		ft_strlcpy(segment, str + start, *i - start + 1);
		(*i)++;
		return (segment);
	}
	return (gc_track(gc, ft_strdup("")));
}

static char	*process_double_quote_segment(t_gc *gc, t_env *env, char *str,
		int *i, int exit_status)
{
	int		start;
	char	*segment;
	char	*expanded_segment;

	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != '"')
		(*i)++;
	if (str[*i] == '"')
	{
		segment = gc_track(gc, malloc(*i - start + 1));
		ft_strlcpy(segment, str + start, *i - start + 1);
		expanded_segment = replace_exit_status(gc, segment, exit_status);
		expanded_segment = replace_env_vars(gc, env, expanded_segment);
		(*i)++;
		return (expanded_segment);
	}
	return (gc_track(gc, ft_strdup("")));
}

static char	*process_unquoted_segment(t_gc *gc, t_env *env, char *str,
		int *i, int exit_status)
{
	int		start;
	char	*segment;
	char	*expanded_segment;

	start = *i;
	while (str[*i] && str[*i] != '\'' && str[*i] != '"')
		(*i)++;
	segment = gc_track(gc, malloc(*i - start + 1));
	ft_strlcpy(segment, str + start, *i - start + 1);
	expanded_segment = replace_exit_status(gc, segment, exit_status);
	expanded_segment = replace_env_vars(gc, env, expanded_segment);
	return (expanded_segment);
}

static char	*expand_with_quote_awareness(t_gc *gc, t_env *env, char *str,
		int exit_status)
{
	char	*result;
	char	*temp;
	char	*segment;
	int		i;

	result = gc_track(gc, ft_strdup(""));
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'')
			segment = process_single_quote_segment(gc, str, &i);
		else if (str[i] == '"')
			segment = process_double_quote_segment(gc, env, str, &i, exit_status);
		else
			segment = process_unquoted_segment(gc, env, str, &i, exit_status);
		temp = gc_track(gc, ft_strjoin(result, segment));
		result = temp;
	}
	return (result);
}

char	*expand_variables_in_double_quotes(t_gc *gc, t_env *env, char *str,
		int exit_status)
{
	char	*result;

	if (!str)
		return (NULL);
	result = replace_exit_status(gc, str, exit_status);
	result = replace_env_vars(gc, env, result);
	return (result);
}

char	*expand_variables(t_gc *gc, t_env *env, char *str, int exit_status)
{
	char	*result;

	if (!str)
		return (NULL);
	result = expand_with_quote_awareness(gc, env, str, exit_status);
	return (result);
}
