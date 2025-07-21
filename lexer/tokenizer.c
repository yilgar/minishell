#include "../minishell.h"

char	*extract_word(t_gc *gc, char *input, int *i)
{
	int		start;
	int		len;
	char	*word;

	start = *i;
	while (input[*i] && !is_whitespace(input[*i])
		&& !is_special_char(input[*i]))
		(*i)++;
	len = *i - start;
	word = gc_track(gc, malloc(len + 1));
	if (!word)
		return (NULL);
	ft_strlcpy(word, input + start, len + 1);
	return (word);
}

static char	*extract_quoted_string(t_gc *gc, char *input, int *i, char quote)
{
	int		start;
	int		len;
	char	*str;

	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote)
		(*i)++;
	if (input[*i] != quote)
		return (NULL);
	len = *i - start;
	str = gc_track(gc, malloc(len + 1));
	if (!str)
		return (NULL);
	ft_strlcpy(str, input + start, len + 1);
	(*i)++;
	return (str);
}

static t_token	*create_token_from_input(t_gc *gc, char *input, int *i)
{
	char	*str;
	char	*word;

	if (input[*i] == '\'')
	{
		str = extract_quoted_string(gc, input, i, '\'');
		return (create_token(gc, TOKEN_SINGLE_QUOTE, str));
	}
	if (input[*i] == '"')
	{
		str = extract_quoted_string(gc, input, i, '"');
		return (create_token(gc, TOKEN_DOUBLE_QUOTE, str));
	}
	if (is_special_char(input[*i]))
		return (handle_special_char(gc, input, i));
	word = extract_word(gc, input, i);
	return (create_token(gc, TOKEN_WORD, word));
}

static int	handle_whitespace(char *input, int *i)
{
	int	had_whitespace;

	had_whitespace = 0;
	if (is_whitespace(input[*i]))
	{
		had_whitespace = 1;
		skip_whitespace(input, i);
	}
	return (had_whitespace);
}

t_token	*tokenize(t_gc *gc, char *input)
{
	t_token	*head;
	t_token	*token;
	int		i;
	int		had_whitespace;

	head = NULL;
	i = 0;
	while (input[i])
	{
		had_whitespace = handle_whitespace(input, &i);
		if (!input[i])
			break ;
		token = create_token_from_input(gc, input, &i);
		if (token)
		{
			token->has_space_before = had_whitespace;
			add_token(&head, token);
		}
	}
	add_token(&head, create_token(gc, TOKEN_EOF, NULL));
	return (head);
}
