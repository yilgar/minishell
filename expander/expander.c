#include "../minishell.h"

char	*replace_exit_status(t_gc *gc, char *str, int exit_status)
{
	char	*result;
	char	*exit_str;
	char	*pos;
	int		before_len;
	int		after_len;

	pos = ft_strnstr(str, "$?", ft_strlen(str));
	if (!pos)
		return (gc_track(gc, ft_strdup(str)));
	exit_str = gc_track(gc, ft_itoa(exit_status));
	before_len = pos - str;
	after_len = ft_strlen(pos + 2);
	result = gc_track(gc, malloc(before_len + ft_strlen(exit_str) + after_len
				+ 1));
	if (!result)
		return (NULL);
	ft_strlcpy(result, str, before_len + 1);
	ft_strlcat(result, exit_str, before_len + ft_strlen(exit_str) + after_len
		+ 1);
	ft_strlcat(result, pos + 2, before_len + ft_strlen(exit_str) + after_len
		+ 1);
	return (result);
}

static char	*expand_with_quote_awareness(t_gc *gc, t_env *env, char *str,
		int exit_status)
{
	char				*result;
	char				*temp;
	char				*segment;
	int					i;
	t_expand_context	ctx;

	ctx.gc = gc;
	ctx.env = env;
	ctx.exit_status = exit_status;
	result = gc_track(gc, ft_strdup(""));
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'')
			segment = process_single_quote_segment(gc, str, &i);
		else if (str[i] == '"')
			segment = process_double_quote_segment(&ctx, str, &i);
		else
			segment = process_unquoted_segment(&ctx, str, &i);
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
