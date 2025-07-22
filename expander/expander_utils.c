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

char	*process_single_quote_segment(t_gc *gc, char *str, int *i)
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

char	*process_double_quote_segment(t_gc *gc, t_env *env, char *str,
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

char	*process_unquoted_segment(t_gc *gc, t_env *env, char *str,
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