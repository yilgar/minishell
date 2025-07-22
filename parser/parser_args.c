#include "../minishell.h"

static void	skip_concatenable_tokens(t_token **current)
{
	while (*current && is_concatenable_token(*current))
	{
		*current = (*current)->next;
		if (*current && (*current)->has_space_before)
			break ;
	}
}

static void	skip_redirection_tokens(t_token **current)
{
	*current = (*current)->next;
	if (*current && is_concatenable_token(*current))
	{
		while (*current && is_concatenable_token(*current))
		{
			*current = (*current)->next;
			if (*current && (*current)->has_space_before)
				break ;
		}
	}
	else if (*current)
		*current = (*current)->next;
}

static int	count_args_tokens(t_token *tokens)
{
	t_token	*current;
	int		count;

	count = 0;
	current = tokens;
	while (current && current->type != TOKEN_PIPE 
		&& current->type != TOKEN_EOF)
	{
		if (is_concatenable_token(current))
		{
			count++;
			skip_concatenable_tokens(&current);
		}
		else if (is_redirection_token(current))
			skip_redirection_tokens(&current);
		else
			current = current->next;
	}
	return (count);
}



static void	extract_args_from_tokens(t_gc *gc, char **args, t_token **tokens)
{
	t_token	*current;
	int		i;

	i = 0;
	current = *tokens;
	while (current && current->type != TOKEN_PIPE 
		&& current->type != TOKEN_EOF)
	{
		if (is_concatenable_token(current))
			args[i++] = concatenate_tokens(gc, &current);
		else if (is_redirection_token(current))
			skip_redirection_tokens(&current);
		else
			current = current->next;
	}
	args[i] = NULL;
	*tokens = current;
}

char	**parse_args(t_gc *gc, t_token **tokens)
{
	char	**args;
	int		count;

	count = count_args_tokens(*tokens);
	args = gc_track(gc, malloc(sizeof(char *) * (count + 1)));
	if (!args)
		return (NULL);
	extract_args_from_tokens(gc, args, tokens);
	compact_args_array(args, count);
	return (args);
}
