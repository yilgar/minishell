#include "../minishell.h"

int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '$' || c == '\''
		|| c == '"');
}

int	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

int	is_redirect(char c)
{
	return (c == '<' || c == '>');
}

int	skip_whitespace(char *str, int *i)
{
	int	count;

	count = 0;
	while (str[*i] && is_whitespace(str[*i]))
	{
		(*i)++;
		count++;
	}
	return (count);
}
