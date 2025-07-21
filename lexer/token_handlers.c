#include "../minishell.h"

t_token	*handle_pipe_token(t_gc *gc, int *i)
{
	(*i)++;
	return (create_token(gc, TOKEN_PIPE, "|"));
}

t_token	*handle_redirect_token(t_gc *gc, char *input, int *i)
{
	t_token_type	type;

	type = get_redirect_type(input, i);
	if (type == TOKEN_REDIRECT_IN)
		return (create_token(gc, type, "<"));
	if (type == TOKEN_REDIRECT_OUT)
		return (create_token(gc, type, ">"));
	if (type == TOKEN_REDIRECT_APPEND)
		return (create_token(gc, type, ">>"));
	if (type == TOKEN_REDIRECT_HEREDOC)
		return (create_token(gc, type, "<<"));
	return (create_token(gc, TOKEN_ERROR, NULL));
}

t_token	*handle_variable_token(t_gc *gc, char *input, int *i)
{
	char	*var_name;
	char	*full_var;
	int		start;
	int		len;

	(*i)++;
	if (input[*i] == '?')
	{
		(*i)++;
		return (create_token(gc, TOKEN_EXIT_STATUS, "$?"));
	}
	
	start = *i;
	while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
		(*i)++;
	
	len = *i - start;
	if (len == 0)
	{
		(*i)++;
		return (create_token(gc, TOKEN_WORD, "$"));
	}
	
	var_name = gc_track(gc, malloc(len + 1));
	ft_strlcpy(var_name, input + start, len + 1);
	full_var = gc_track(gc, ft_strjoin("$", var_name));
	return (create_token(gc, TOKEN_ENV_VAR, full_var));
}

t_token	*handle_special_char(t_gc *gc, char *input, int *i)
{
	char	c;

	c = input[*i];
	if (c == '|')
		return (handle_pipe_token(gc, i));
	if (c == '<' || c == '>')
		return (handle_redirect_token(gc, input, i));
	if (c == '$')
		return (handle_variable_token(gc, input, i));
	return (create_token(gc, TOKEN_ERROR, NULL));
}

t_token_type	get_redirect_type(char *input, int *i)
{
	if (input[*i] == '<')
	{
		if (input[*i + 1] == '<')
		{
			(*i) += 2;
			return (TOKEN_REDIRECT_HEREDOC);
		}
		(*i)++;
		return (TOKEN_REDIRECT_IN);
	}
	if (input[*i] == '>')
	{
		if (input[*i + 1] == '>')
		{
			(*i) += 2;
			return (TOKEN_REDIRECT_APPEND);
		}
		(*i)++;
		return (TOKEN_REDIRECT_OUT);
	}
	return (TOKEN_ERROR);
}
